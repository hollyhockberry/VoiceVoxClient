// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include "VoiceVoxClient.h"
#include "tasks/utils.h"

namespace {
constexpr const char* ROOT_CA = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB\n" \
"AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC\n" \
"ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL\n" \
"wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D\n" \
"LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK\n" \
"4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5\n" \
"bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y\n" \
"sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ\n" \
"Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4\n" \
"FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc\n" \
"SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql\n" \
"PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND\n" \
"TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw\n" \
"SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1\n" \
"c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx\n" \
"+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB\n" \
"ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu\n" \
"b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E\n" \
"U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu\n" \
"MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC\n" \
"5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW\n" \
"9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG\n" \
"WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O\n" \
"he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC\n" \
"Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5\n" \
"-----END CERTIFICATE-----\n";
}  // namespace

VoiceVoxClient::VoiceVoxClient(AudioOutput& output, const char* rootCACertificate)
: _output(output), _rootCACertificate(rootCACertificate), _apiKey() {
}

void VoiceVoxClient::begin() {
  const char* rootCA = _rootCACertificate == nullptr ? ROOT_CA : _rootCACertificate;
  auto queue = ::xQueueCreate(1, sizeof(void*));

  _synthesis = new tts_tasks::SynthesisTask(queue, rootCA);
  _playMp3 = new tts_tasks::PlayMp3Task(_output, queue, rootCA);

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

  client->setInsecure();
  String url = "https://static.tts.quest/voicevox_speakers.json";
  if (tts_tasks::GetResponseBody(url, *client, *https, *doc)) {
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
  client->setInsecure();
  String url = "https://static.tts.quest/voicevox_speakers_by_id/";
  url += String(id) + ".txt";
  if (!tts_tasks::GetResponseBody(url, *client, *https, payload)) {
    payload = "";
  }
  delete client;
  delete https;

  return payload;
}
