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

#ifndef SRC_PLATFORM_LINUX_LB_SHELL_SHELL_AUDIO_STREAMER_LINUX_H_
#define SRC_PLATFORM_LINUX_LB_SHELL_SHELL_AUDIO_STREAMER_LINUX_H_

#include <map>

#include "base/synchronization/lock.h"
#include "lb_pulse_audio.h"
#include "media/audio/shell_audio_streamer.h"

class PulseAudioHost;

class ShellAudioStreamerLinux : public media::ShellAudioStreamer {
 public:
  ShellAudioStreamerLinux();
  ~ShellAudioStreamerLinux();

  virtual Config GetConfig() const OVERRIDE;
  virtual bool AddStream(media::ShellAudioStream* stream) OVERRIDE;
  virtual void RemoveStream(media::ShellAudioStream* stream) OVERRIDE;
  virtual bool HasStream(media::ShellAudioStream* stream) const OVERRIDE;
  virtual bool SetVolume(media::ShellAudioStream* stream, double volume)
      OVERRIDE;

 private:
  typedef std::map<media::ShellAudioStream*, PulseAudioHost*> StreamMap;
  StreamMap streams_;
  mutable base::Lock streams_lock_;
  LBPulseAudioContext* pulse_audio_context_;

  DISALLOW_COPY_AND_ASSIGN(ShellAudioStreamerLinux);
};

#endif  // SRC_PLATFORM_LINUX_LB_SHELL_SHELL_AUDIO_STREAMER_LINUX_H_
