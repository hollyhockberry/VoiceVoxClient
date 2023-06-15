// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef TASKS_PLAYMP3TASK_H_
#define TASKS_PLAYMP3TASK_H_

#include <WString.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutput.h>
#include "Task.h"

namespace tts_tasks {

class PlayMp3Task : public Task {
  AudioOutput& _output;
  AudioGeneratorMP3 _mp3;
  QueueHandle_t _queue = nullptr;
  const char* _rootCACertificate;
  uint8_t* _buffer;
  bool _busy = false;

 public:
  PlayMp3Task(AudioOutput& output, QueueHandle_t recvQueue, const char* rootCACertificate);
  bool busy() const;

 protected:
  void loop() override;
};

}  // tts_tasks

#endif  // TASKS_PLAYMP3TASK_H_
