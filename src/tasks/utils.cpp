// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "utils.h"

namespace tts_tasks {

bool GetResponseBody(const String& url, WiFiClientSecure& client, HTTPClient& https, const char* rootCA, String& out) {
  if (rootCA) {
    client.setCACert(rootCA);
  } else {
    client.setInsecure();
  }
  if (!https.begin(client, url)) {
    return false;
  }
  const auto _ = https.GET();
  out = https.getString();
  https.end();
  return true;
}

bool GetResponseBody(const String& url, WiFiClientSecure& client, HTTPClient& https, const char* rootCA, DynamicJsonDocument& out) {
  String payload;
  if (!GetResponseBody(url, client, https, rootCA, payload)) {
    return false;
  }
  if (::deserializeJson(out, payload.c_str())) {
    return false;
  }
  return true;
}

}  // tts_tasks
