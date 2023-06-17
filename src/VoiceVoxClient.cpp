// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "VoiceVoxClient.h"
#include "tasks/utils.h"

VoiceVoxClient::VoiceVoxClient(AudioOutput& output, const char* rootCACertificate)
: _output(output), _rootCACertificate(rootCACertificate), _apiKey() {
}

void VoiceVoxClient::begin() {
  auto queue = ::xQueueCreate(1, sizeof(void*));

  _synthesis = new tts_tasks::SynthesisTask(queue, _rootCACertificate);
  _playMp3 = new tts_tasks::PlayMp3Task(_output, queue, _rootCACertificate);

  _synthesis->begin();
  _playMp3->begin();
}

bool VoiceVoxClient::busy() const {
  if (_synthesis && _synthesis->busy()) {
    return true;
  }
  if (_playMp3 && _playMp3->busy()) {
    return true;
  }
  return false;
}

void VoiceVoxClient::apiKey(const char* key) {
  if (_synthesis) {
    _synthesis->apiKey(key);
  }
}

int VoiceVoxClient::speaker() const {
  return _synthesis ? _synthesis->speaker() : -1;
}

void VoiceVoxClient::speaker(int id) {
  if (_synthesis) {
    _synthesis->speaker(id);
  }
}

void VoiceVoxClient::queue(const char* message) {
  if (_synthesis) {
    _synthesis->queue(message);
  }
}

std::vector<String> VoiceVoxClient::speakerNames() const {
  std::vector<String> result;

  auto doc = new DynamicJsonDocument(4096);
  auto client = new WiFiClientSecure();
  auto https = new HTTPClient();
  String url = "https://static.tts.quest/voicevox_speakers.json";
  if (tts_tasks::GetResponseBody(url, *client, *https, nullptr, *doc)) {
    for  (const auto& d : doc->as<JsonArray>()) {
      result.push_back((const char*)d);
    }
  }
  delete client;
  delete https;
  delete doc;
  return result;
}

String VoiceVoxClient::speakerName(int id) const {
  if (id < 0) {
    return "";
  }
  String payload;
  auto client = new WiFiClientSecure();
  auto https = new HTTPClient();
  String url = "https://static.tts.quest/voicevox_speakers_by_id/";
  url += String(id) + ".txt";
  if (!tts_tasks::GetResponseBody(url, *client, *https, nullptr, payload)) {
    payload = "";
  }
  delete client;
  delete https;

  return payload;
}
