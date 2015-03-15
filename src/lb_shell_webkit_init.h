/*
 * Copyright 2012 Google Inc. All Rights Reserved.
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
// Provides accessor functions for webkit_glue::WebKitClientImpl
// to provide access to various lb_shell implementations of WebKit
// client functions.

#ifndef SRC_LB_SHELL_WEBKIT_INIT_H_
#define SRC_LB_SHELL_WEBKIT_INIT_H_

#include "external/chromium/base/memory/scoped_ptr.h"
#include "external/chromium/base/threading/thread.h"
#include "external/chromium/webkit/glue/simple_webmimeregistry_impl.h"
#include "external/chromium/webkit/glue/webkit_glue.h"
#include "external/chromium/webkit/glue/webkitplatformsupport_impl.h"
#include "external/chromium/webkit/support/simple_database_system.h"
#include "external/chromium/webkit/tools/test_shell/simple_dom_storage_system.h"
#include "third_party/WebKit/Source/Platform/chromium/public/WebAudioDevice.h"
#include "third_party/WebKit/Source/WebCore/Modules/h5vcc/accountManager/AccountManager.h"
#include "third_party/WebKit/Source/WebCore/Modules/h5vcc/dial/DialNativeServiceHandler.h"
#include "third_party/WebKit/Source/WebCore/Modules/h5vcc/events/EventReporter.h"
#include "third_party/WebKit/Source/WebCore/Modules/h5vcc/home/HomePin.h"
#include "third_party/WebKit/Source/WebCore/Modules/h5vcc/log/LogPinger.h"
#include "third_party/WebKit/Source/WebCore/Modules/h5vcc/remote/RemoteMediaInfo.h"
#include "third_party/WebKit/Source/WebCore/Modules/h5vcc/system/System.h"
#include "third_party/WebKit/Source/WebCore/Modules/speech/SpeechRecognitionClient.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebStorageNamespace.h"
#include "webkit/glue/webfileutilities_impl.h"

#include "lb_shell.h"
#include "lb_shell_export.h"
#include "lb_webblobregistry_impl.h"
#include "lb_webcookiejar_impl.h"

// A base implementation of H5vcc::System which contains common code shared
// among all platforms.
class H5vccSystemCommonImpl : public H5vcc::System {
 public:
  virtual ~H5vccSystemCommonImpl() {}

  virtual bool AreKeysReversed() OVERRIDE {
    return false;  // no such concept on this platform.
  }

  virtual WTF::String GetLocalizedString(const WTF::String& key) OVERRIDE;

  virtual WTF::String GetVideoContainerSizeOverride() OVERRIDE {
    return "";  // no override on this platform
  }

  virtual WTF::String GetRegion() OVERRIDE {
    return "";  // hardware is not regioned.
  }

  virtual WTF::String GetViewState() OVERRIDE {
    return "";  // no such concept on this platform.
  }

  virtual void Minimize() OVERRIDE {
    // no such concept on this platform.
  }

  virtual void RegisterExperiments(const WTF::String& ids) OVERRIDE;

  virtual void StartPlatformAuthentication() OVERRIDE {
    // no such concept on this platform.
  }

  virtual bool TriggerHelp() OVERRIDE {
    // not supported on this platform.
    return false;
  }

  virtual bool TryUnsnapToFullscreen() OVERRIDE {
    return false;  // no such concept on this platform.
  }

  virtual bool HideSplashScreen() OVERRIDE {
    return LBWebViewHost::Get()->HideSplashScreen();
  }

 protected:
  // Protected constructor should make sure that every platform defines a
  // platform specific implementation of this class
  H5vccSystemCommonImpl() {}
};

class H5vccLogPingerImpl : public H5vcc::LogPinger {
 public:
  explicit H5vccLogPingerImpl(
      const scoped_refptr<base::MessageLoopProxy>& webkit_message_loop);
  virtual void Connect(WebCore::LogPinger* inst) OVERRIDE;
  virtual void Disconnect(WebCore::LogPinger* inst) OVERRIDE;
  virtual void InvokeCallback(const std::string& payload) OVERRIDE;
  virtual ~H5vccLogPingerImpl();

 private:
  void InvokeCallbackInternal(const std::string& payload);

  static const size_t kMaxPendingCallbackSize;
  WebCore::LogPinger* webkit_inst_;
  std::vector<std::string> pending_callbacks_;
  scoped_refptr<base::MessageLoopProxy> webkit_message_loop_;
};

// named in honor of TestShellWebKitInit which lives in
// external/chromium/webkit/tools/test_shell/test_shell_webkit_init.h
class LB_SHELL_EXPORT LBShellWebKitInit
    : public webkit_glue::WebKitPlatformSupportImpl {
 public:
  virtual ~LBShellWebKitInit();
  virtual WebKit::WebBlobRegistry* blobRegistry() OVERRIDE;
  virtual WebKit::WebMimeRegistry* mimeRegistry() OVERRIDE;
  virtual WebKit::WebFileUtilities* fileUtilities() OVERRIDE;
  virtual WebKit::WebCookieJar* cookieJar() OVERRIDE;
  virtual WebKit::WebString defaultLocale() OVERRIDE;
  virtual WebKit::WebStorageNamespace* createLocalStorageNamespace(
      const WebKit::WebString& path, unsigned quota) OVERRIDE;
  virtual WebCore::ObjectPositionReporter* createObjectPositionReporter()
      OVERRIDE;

  virtual string16 GetLocalizedString(int message_id) OVERRIDE {
    return string16();
  }

  virtual base::StringPiece GetDataResource(
      int resource_id, ui::ScaleFactor scale_factor) OVERRIDE;

  virtual void GetPlugins(bool refresh,
      std::vector<webkit::WebPluginInfo>* plugins) OVERRIDE {
    // return no plugins.
  }

  virtual webkit_glue::ResourceLoaderBridge* CreateResourceLoader(
      const webkit_glue::ResourceLoaderBridge::RequestInfo& request_info)
      OVERRIDE;
  virtual webkit_glue::WebSocketStreamHandleBridge* CreateWebSocketBridge(
      WebKit::WebSocketStreamHandle* handle,
      webkit_glue::WebSocketStreamHandleDelegate* delegate) OVERRIDE;

  void SetThemeEngine(WebKit::WebThemeEngine *engine) {
    theme_engine_ = engine;
  }

  virtual WebKit::WebThemeEngine * themeEngine() {
    return theme_engine_;
  }

  virtual WebKit::WebGraphicsContext3D* createOffscreenGraphicsContext3D(
    const WebKit::WebGraphicsContext3D::Attributes&) OVERRIDE;
  virtual bool canAccelerate2dCanvas() OVERRIDE;

  // WebAudio support, overriding Platform.h directly
  virtual double audioHardwareSampleRate() OVERRIDE;
  virtual size_t audioHardwareBufferSize() OVERRIDE;
  virtual int audioHardwareMaxChannels() OVERRIDE;
  virtual WebKit::WebAudioDevice* createAudioDevice(
      size_t bufferSize,
      unsigned numberOfChannels,
      double sampleRate,
      WebKit::WebAudioDevice::RenderCallback* callback) OVERRIDE;

  virtual H5vcc::AccountInfo *h5vccAccountInfo() OVERRIDE {
    return account_info_impl_.get();
  }

  virtual H5vcc::AccountManager *h5vccAccountManager() OVERRIDE {
    return account_manager_impl_.get();
  }

  virtual H5vcc::AudioConfigArray *h5vccAudioConfigArray() OVERRIDE {
    return audio_config_array_impl_.get();
  }

  virtual H5vcc::ClosedCaptionsSettings *h5vccClosedCaptionsSettings()
      OVERRIDE {
    return closed_captions_settings_impl_.get();
  }

  virtual H5vcc::DialNativeServiceHandler *h5vccDialNativeServiceHandler()
      OVERRIDE {
    return dial_native_service_handler_.get();
  }

  virtual H5vcc::DvrManager *h5vccDvrManager() OVERRIDE {
    return dvr_manager_impl_.get();
  }

  virtual H5vcc::EventReporter *h5vccEventReporter() OVERRIDE {
    return event_reporter_impl_.get();
  }

  virtual H5vcc::HomePinManager *h5vccHomePinManager() OVERRIDE {
    return home_pin_manager_impl_.get();
  }

  virtual H5vcc::LogPinger *h5vccLogPinger() OVERRIDE {
    return log_pinger_impl_.get();
  }

  virtual H5vcc::InputManager *h5vccInputManager() OVERRIDE {
    return input_manager_impl_.get();
  }

  virtual H5vcc::RemoteController *h5vccRemoteController() OVERRIDE {
    return remote_controller_impl_.get();
  }

  virtual H5vcc::SearchManager *h5vccSearchManager() OVERRIDE {
    return search_manager_impl_.get();
  }

  virtual H5vcc::Sso *h5vccSso() OVERRIDE {
    return sso_impl_.get();
  }

  virtual H5vcc::Storage *h5vccStorage() OVERRIDE {
    return storage_impl_.get();
  }

  virtual H5vcc::System *h5vccSystem() OVERRIDE {
    return system_impl_.get();
  }

#if __LB_ENABLE_WEB_SPEECH_API__
  virtual WebCore::SpeechRecognitionClient *createSpeechRecognitionClient()
      OVERRIDE {
    return speech_recognition_client_impl_.get();
  }
#endif

 protected:
  // Must be constructed using a derived class.
  LBShellWebKitInit();

  // These belong to the base class even if they are only given values by
  // subclasses.  This allows us to keep these objects alive during the base
  // class's call to WebKit::shutdown().
  scoped_ptr<H5vcc::AccountInfo> account_info_impl_;
  scoped_ptr<H5vcc::AccountManager> account_manager_impl_;
  scoped_ptr<H5vcc::AudioConfigArray> audio_config_array_impl_;
  scoped_ptr<H5vcc::ClosedCaptionsSettings> closed_captions_settings_impl_;
  scoped_ptr<H5vcc::DialNativeServiceHandler> dial_native_service_handler_;
  scoped_ptr<H5vcc::DvrManager> dvr_manager_impl_;
  scoped_ptr<H5vcc::EventReporter> event_reporter_impl_;
  scoped_ptr<H5vcc::HomePinManager> home_pin_manager_impl_;
  scoped_ptr<H5vcc::LogPinger> log_pinger_impl_;
  scoped_ptr<H5vcc::InputManager> input_manager_impl_;
  scoped_ptr<H5vcc::RemoteController> remote_controller_impl_;
  scoped_ptr<H5vcc::SearchManager> search_manager_impl_;
  scoped_ptr<H5vcc::Sso> sso_impl_;
  scoped_ptr<H5vcc::Storage> storage_impl_;
  scoped_ptr<H5vcc::System> system_impl_;
#if __LB_ENABLE_WEB_SPEECH_API__
  scoped_ptr<WebCore::SpeechRecognitionClient> speech_recognition_client_impl_;
#endif

 private:
  void InitializeCompositor();

  scoped_refptr<LBWebBlobRegistryImpl> blob_registry_;
  scoped_ptr<webkit_glue::SimpleWebMimeRegistryImpl> mime_registry_;
  WebKit::WebThemeEngine* theme_engine_;
  webkit_glue::WebFileUtilitiesImpl file_utilities_;
  SimpleDomStorageSystem dom_storage_system_;
  LBWebCookieJarImpl cookie_jar_;
  scoped_ptr<base::Thread> compositor_thread_;
};

#endif  // SRC_LB_SHELL_WEBKIT_INIT_H_
