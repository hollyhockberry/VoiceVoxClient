// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef TASKS_UTILS_H_
#define TASKS_UTILS_H_

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

namespace tts_tasks {

bool GetResponseBody(const String& url, WiFiClientSecure& client, HTTPClient& https, DynamicJsonDocument& out);

}  // tts_tasks

#endif  // TASKS_UTILS_H_
