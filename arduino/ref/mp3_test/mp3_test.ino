#include <SPI.h>
#include <SdFat.h>
#include <FreeStack.h>
#include <SFEMP3Shield.h>

SdFat sd;
SFEMP3Shield MP3player;

// 전역 변수로 볼륨을 설정합니다.
int volume = 15; // 초기 볼륨 (0 ~ 15 범위에서 설정 가능)

// 초기 볼륨 설정 함수
void setInitialVolume() {
  MP3player.setVolume(volume, volume);
}

void setup() {
  Serial.begin(115200);

  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

  uint8_t result = MP3player.begin();
  if(result != 0) {
    Serial.print(F("Error code: "));
    Serial.print(result);
    Serial.println(F(" when trying to start MP3 player"));
  }

  setInitialVolume();

  Serial.println(F("Ready to play. Send '1' to start playing files."));
  Serial.println(F("Use 'u' to increase volume and 'd' to decrease volume."));
}

void loop() {
  if(Serial.available()) {
    char command = Serial.read();
    if(command == '1') {
      playFiles();
    } else if(command == 'u') {
      adjustVolume(1);
    } else if(command == 'd') {
      adjustVolume(-1);
    }
  }
}

void playFiles() {
  char *fileList[] = {"1.greet.mp3", "2-1.clear.mp3", "3-1.temp_down.mp3"};
  uint8_t result;

  for(int i = 0; i < 3; i++) {
    Serial.print(F("Playing: "));
    Serial.println(fileList[i]);

    result = MP3player.playMP3(fileList[i]);
    if(result != 0) {
      Serial.print(F("Error code: "));
      Serial.print(result);
      Serial.println(F(" when trying to play track"));
    }

    // Wait for the file to finish playing
    while(MP3player.isPlaying()) {
      delay(100);
    }
  }

  Serial.println(F("Finished playing all files."));
}

void adjustVolume(int change) {
  volume += change;
  if(volume > 255) volume = 255;
  if(volume < 0) volume = 0;
  MP3player.setVolume(volume, volume);
  Serial.print(F("Volume set to: "));
  Serial.println(volume);
}
