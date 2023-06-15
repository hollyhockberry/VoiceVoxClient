// Copyright (c) 2023 Inaba (@hollyhockberry)
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <AudioFileSourceBuffer.h>
#include "PlayMp3Task.h"
#include "audio/AudioFileSourceHTTPSStream.h"

namespace tts_tasks {

namespace {
constexpr const int bufferSize = 30 * 1024;
}  // namespace

PlayMp3Task::PlayMp3Task(AudioOutput& output, QueueHandle_t recvQueue, const char* rootCACertificate)
:  Task("mp3", 4096, 100), _output(output), _mp3(), _queue(recvQueue), _rootCACertificate(rootCACertificate) {
  _buffer = static_cast<uint8_t *>(malloc(bufferSize));
}

bool PlayMp3Task::busy() const {
  return _busy;
}

void PlayMp3Task::loop() {  
  String* url;
  const auto ret = ::xQueueReceive(_queue, &url, portMAX_DELAY);

  _busy = true;
  auto audioFile = new AudioFileSourceHTTPSStream(url->c_str(), _rootCACertificate);
  auto audiobuffer = new AudioFileSourceBuffer(audioFile, _buffer, bufferSize);
  _mp3.begin(audiobuffer, &_output);

  while (true) {
    if (_mp3.isRunning()) {
      if (!_mp3.loop()) {
        _mp3.stop();
        break;
      }
    }
    ::vTaskDelay(1);
  }
  delete audiobuffer;
  delete audioFile;
  delete url;
  _busy = false;
}

}  // tts_tasks
