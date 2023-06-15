// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "SynthesisTask.h"
#include "utils.h"

namespace tts_tasks {

SynthesisTask::SynthesisTask(QueueHandle_t sendQueue, const char* rootCACertificate)
:  Task("synthesis", 4096, 100), _sendQueue(sendQueue), _rootCACertificate(rootCACertificate), _queue() {
}

int SynthesisTask::speaker() const {
  return _speaker;
}

void SynthesisTask::speaker(int id) {
  _speaker = id;
}

void SynthesisTask::apiKey(const char* key) {
  _apiKey = key;
}

bool SynthesisTask::busy() const {
  return _busy;
}

void SynthesisTask::queue(const char* message) {
  _queue.push(message);
}

void SynthesisTask::loop() {
  if (_queue.size() <= 0) {
    return;
  }
  if (::uxQueueSpacesAvailable(_sendQueue) <= 0) {
    return;
  }
  _busy = true;
  synthesis();
  _busy = false;
}

namespace {

void URLEncode(const String& message, String& out) {
  const char *hex = "0123456789ABCDEF";
  const char* msg = message.c_str();
  out = "";
  while (*msg != '\0') {
    if ( ('a' <= *msg && *msg <= 'z')
         || ('A' <= *msg && *msg <= 'Z')
         || ('0' <= *msg && *msg <= '9')
         || *msg  == '-' || *msg == '_' || *msg == '.' || *msg == '~' ) {
      out += *msg;
    } else {
      out += '%';
      out += hex[*msg >> 4];
      out += hex[*msg & 0xf];
    }
    msg++;
  }
}

void MakeUrl(const String& message, const char* apiKey, int speaker, String& out) {
  out = "https://api.tts.quest/v3/voicevox/synthesis";
  String sep = "?";
  if (apiKey) {
    out += sep + "key=" + apiKey;
    sep = "&";
  }
  String body;
  URLEncode(message, body);
  out += sep + "text=" + body;
  sep = "&";

  if (speaker >= 0) {
    out += sep + "speaker=" + String(speaker);
    sep = "&";
  }
}

enum struct TtsResult { SUCCESS, RETRY, ERROR };

TtsResult GetStreamingUrl(const String& url, WiFiClientSecure& client, HTTPClient& https, DynamicJsonDocument& doc, String& out) {
  if (!GetResponseBody(url, client, https, doc)) {
    return TtsResult::ERROR;
  }
  const bool success = doc["success"];
  const long retryAfter = doc["retryAfter"];
  const char* mp3 = doc["mp3StreamingUrl"];
  if (retryAfter > 0) {
    ::vTaskDelay(retryAfter * 1000);
    return TtsResult::RETRY;
  }
  if (!success || mp3 == nullptr) {
    return TtsResult::ERROR;
  }
  out = String(mp3);
  return TtsResult::SUCCESS;
}

TtsResult GetStreamingUrl(const String& url, String& out, const char* rootCA) {
  auto doc = new DynamicJsonDocument(1024);
  auto client = new WiFiClientSecure();
  auto https = new HTTPClient();
  client->setCACert(rootCA);
  const auto ret = GetStreamingUrl(url, *client, *https, *doc, out);
  delete client;
  delete https;
  delete doc;
  return ret;
}

}  // namespace

void SynthesisTask::synthesis() {
  const String& message = _queue.front();
  String api;
  MakeUrl(message, _apiKey, _speaker, api);

  auto url = new String();
  switch (GetStreamingUrl(api, *url, _rootCACertificate)) {
  case TtsResult::SUCCESS:
    ::xQueueSend(_sendQueue, &url, 0);
    _queue.pop();
    return;
  case TtsResult::RETRY:
    break;
  case TtsResult::ERROR:
  default:
    std::queue<String> empty;
    std::swap(_queue, empty);
    break;
  }
  // in case of no processing
  delete url;
}

}  // tts_tasks
