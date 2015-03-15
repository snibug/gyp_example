/*
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "shell_audio_streamer_linux.h"

#include "base/logging.h"
#include "lb_platform.h"
#include "lb_pulse_audio.h"
#include "media/audio/audio_parameters.h"
#include "media/base/audio_bus.h"
#include "media/mp4/aac.h"

namespace {

ShellAudioStreamerLinux* instance = NULL;

}  // namespace

class PulseAudioHost : public LBPulseAudioStream::Host {
 public:
  PulseAudioHost(LBPulseAudioContext* pulse_audio_context,
                 media::ShellAudioStream* stream, int rate, int channels);
  ~PulseAudioHost();
  virtual void RequestFrame(size_t length, WriteFunc write) OVERRIDE;

 private:
  enum StreamState {
    STATE_INVALID,
    STATE_PAUSED,    // Voice is paused, will play when unpaused
    STATE_RUNNING,   // Voice is playing, reading new data when possible
  };

  LBPulseAudioContext* pulse_audio_context_;
  uint32 played_frames_;  // frames played by the audio driver
  uint32 written_frames_;  // frames written to the audio driver
  StreamState state_;
  media::ShellAudioStream* lb_audio_stream_;
  LBPulseAudioStream* pulse_audio_stream_;
};

namespace media {

void ShellAudioStreamer::Initialize() {
  CHECK(!instance);
  new ShellAudioStreamerLinux();
}

void ShellAudioStreamer::Terminate() {
  CHECK(instance);
  delete instance;
}

ShellAudioStreamer* ShellAudioStreamer::Instance() {
  CHECK(instance);
  return instance;
}

}  // namespace media

media::ShellAudioStreamer::Config ShellAudioStreamerLinux::GetConfig() const {
  const uint32 initial_rebuffering_frames_per_channel =
      media::mp4::AAC::kSamplesPerFrame * 32;
  const uint32 sink_buffer_size_in_frames_per_channel =
      initial_rebuffering_frames_per_channel * 8;
  const uint32 max_hardware_channels = 2;

  return Config(Config::INTERLEAVED,
      initial_rebuffering_frames_per_channel,
      sink_buffer_size_in_frames_per_channel,
      max_hardware_channels, sizeof(float)  /* bytes_per_sample */);
}

bool ShellAudioStreamerLinux::AddStream(media::ShellAudioStream* stream) {
  base::AutoLock lock(streams_lock_);

  if (pulse_audio_context_ == NULL) {
    pulse_audio_context_ = new LBPulseAudioContext;
    DCHECK(pulse_audio_context_->Initialize());
  }

  // other basic checks, it is assumed that the decoder or renderer algorithm
  // will have rejected invalid configurations before creating a sink, so
  // here they are asserts instead of run-time errors
  const media::AudioParameters& params = stream->GetAudioParameters();
  DCHECK(params.channels() == 1 || params.channels() == 2);
  DCHECK_EQ(params.bits_per_sample(), 32);

  media::AudioBus* audio_bus = stream->GetAudioBus();
  const media::AudioParameters& audio_parameters = stream->GetAudioParameters();
  const int frames_per_channel = audio_bus->frames();
  const int sample_rate = audio_parameters.sample_rate();

  streams_[stream] = new PulseAudioHost(pulse_audio_context_, stream,
                                        sample_rate, params.channels());

  return true;
}

void ShellAudioStreamerLinux::RemoveStream(media::ShellAudioStream* stream) {
  base::AutoLock lock(streams_lock_);

  StreamMap::iterator it = streams_.find(stream);
  if (it == streams_.end())
    return;
  delete it->second;
  streams_.erase(it);

  if (streams_.empty()) {
    delete pulse_audio_context_;
    pulse_audio_context_ = NULL;
  }
}

bool ShellAudioStreamerLinux::HasStream(media::ShellAudioStream* stream) const {
  base::AutoLock lock(streams_lock_);
  return streams_.find(stream) != streams_.end();
}

bool ShellAudioStreamerLinux::SetVolume(media::ShellAudioStream* stream,
                                        double volume) {
  if (volume != 1.0) {
    NOTIMPLEMENTED();
  }
  return volume != 1.0;
}

ShellAudioStreamerLinux::ShellAudioStreamerLinux()
    : pulse_audio_context_(NULL) {
  instance = this;
}

ShellAudioStreamerLinux::~ShellAudioStreamerLinux() {
  instance = NULL;
}

PulseAudioHost::PulseAudioHost(LBPulseAudioContext* pulse_audio_context,
                               media::ShellAudioStream* stream,
                               int rate, int channels)
    : pulse_audio_context_(pulse_audio_context),
      played_frames_(0),
      written_frames_(0),
      state_(STATE_PAUSED),
      lb_audio_stream_(stream) {
  pulse_audio_stream_ = pulse_audio_context->CreateStream(this, rate, channels);
}

PulseAudioHost::~PulseAudioHost() {
  if (pulse_audio_stream_) {
    pulse_audio_context_->DestroyStream(pulse_audio_stream_);
  }
}

void PulseAudioHost::RequestFrame(size_t length, WriteFunc write) {
  uint64 time_played = pulse_audio_stream_->GetPlaybackCursorInMicroSeconds();
  int sample_rate = lb_audio_stream_->GetAudioParameters().sample_rate();
  uint64 frames_played = time_played * sample_rate / 1000000;
  uint32 frame_consumed = 0;
  uint32 frame_pulled;

  if (frames_played > written_frames_)
    frames_played = written_frames_;
  if (frames_played > played_frames_)
    frame_consumed = frames_played - played_frames_;
  played_frames_ += frame_consumed;

  // each frame is 16 bytes (FLTLE * 2 channels)
  const int kBytesPerFrame = sizeof(float) * 2;
  DCHECK_EQ(length % kBytesPerFrame, 0);
  length /= kBytesPerFrame;
  const media::AudioBus* audio_bus = lb_audio_stream_->GetAudioBus();

  lb_audio_stream_->ConsumeFrames(frame_consumed);
  lb_audio_stream_->PullFrames(NULL, &frame_pulled);

  if (played_frames_ + frame_pulled > written_frames_ && length) {
    frame_pulled = played_frames_ + frame_pulled - written_frames_;
    frame_pulled = std::min<size_t>(frame_pulled, length);

    uint32 frames = audio_bus->frames() / 2;
    uint32 frame_offset = written_frames_ % frames;

    uint32 frame_to_write =
        std::min<size_t>(frame_pulled, frames - frame_offset);
    const float* buffer = audio_bus->channel(0) + frame_offset * 2;
    write.Run(reinterpret_cast<const uint8*>(buffer),
              frame_to_write * kBytesPerFrame);
    written_frames_ += frame_to_write;
  }

  switch (state_) {
    case STATE_PAUSED:
      if (!lb_audio_stream_->PauseRequested()) {
        pulse_audio_stream_->Play();
        state_ = STATE_RUNNING;
      }
      break;
    case STATE_RUNNING:
      if (lb_audio_stream_->PauseRequested()) {
        pulse_audio_stream_->Pause();
        state_ = STATE_PAUSED;
      }
      break;
    case STATE_INVALID:
      break;
  }
}

