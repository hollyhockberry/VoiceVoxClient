// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef VOICEVOXCLIENT_H_
#define VOICEVOXCLIENT_H_

#include <vector>
#include <AudioOutput.h>
#include <WString.h>
#include "tasks/SynthesisTask.h"
#include "tasks/PlayMp3Task.h"

class VoiceVoxClient {
  AudioOutput& _output;
  const char* _rootCACertificateApi = nullptr;
  const char* _rootCACertificateStatic = nullptr;
  String _apiKey;
  tts_tasks::SynthesisTask* _synthesis = nullptr;
  tts_tasks::PlayMp3Task* _playMp3 = nullptr;

 public:
  VoiceVoxClient(AudioOutput& output,
                 const char* rootCACertificateApi = nullptr,
                 const char* rootCACertificateStatic = nullptr);

  bool busy() const;

  void apiKey(const char* key);
  int speaker() const;
  void speaker(int id);

  void begin();
  void queue(const char* message);

  std::vector<String> speakerNames() const;
  String speakerName(int id) const;

 private:
  VoiceVoxClient(const VoiceVoxClient&) = delete;
  void operator=(const VoiceVoxClient&) = delete;
};

#endif  // VOICEVOXCLIENT_H_
