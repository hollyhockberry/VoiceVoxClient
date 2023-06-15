// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "utils.h"

namespace tts_tasks {

bool GetResponseBody(const String& url, WiFiClientSecure& client, HTTPClient& https, DynamicJsonDocument& out) {
  if (!https.begin(client, url)) {
    return false;
  }
  const auto _ = https.GET();
  const auto payload = https.getString();
  https.end();
  if (::deserializeJson(out, payload.c_str())) {
    return false;
  }
  return true;
}

}  // tts_tasks
