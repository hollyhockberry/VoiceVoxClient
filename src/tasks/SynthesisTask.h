// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef TASKS_SYNTHESISTASK_H_
#define TASKS_SYNTHESISTASK_H_

#include <queue>
#include <WString.h>
#include "Task.h"

namespace tts_tasks {

class SynthesisTask : public Task {
  QueueHandle_t _sendQueue;
  const char* _rootCACertificate;
  const char* _apiKey = nullptr;
  int _speaker = 3;
  bool _busy = false;
  std::queue<String> _queue;

 public:
  SynthesisTask(QueueHandle_t sendQueue, const char* rootCACertificate);

  int speaker() const;
  void speaker(int id);
  void apiKey(const char* key);

  bool busy() const;
  void queue(const char* message);

 protected:
  void loop() override;
  void synthesis();
};

}  // tts_tasks

#endif  // TASKS_SYNTHESISTASK_H_