// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#ifndef TASKS_TASK_H_
#define TASKS_TASK_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

namespace tts_tasks {

struct TaskContext {
  void* task;
  TickType_t tick;
};

class Task {
  const char* _taskName;
  const uint32_t _stackSize;
  TaskContext _context;

 public:
  Task() : Task(nullptr, 2048, 10) {
  }
  Task(const char* taskName, uint32_t stackSize, TickType_t tick) : _taskName(taskName), _stackSize(stackSize), _context() {
    _context.task = this;
    _context.tick = tick;
  }
 
  void begin() {
    ::xTaskCreate(main, _taskName == nullptr ? "" : _taskName, _stackSize, &_context, 1, nullptr);
  }

 protected:
  virtual void loop() = 0;

 private:
  static void main(void* arg) {
    TaskContext* context = (TaskContext*)arg;
    Task* task = (Task*)(context->task);

    while (true) {
      task->loop();
      ::vTaskDelay(context->tick);
    }
  }

  Task(const Task&) = delete;
  void operator=(const Task&) = delete;
};

} // tts_tasks

#endif  // TASKS_TASK_H_
