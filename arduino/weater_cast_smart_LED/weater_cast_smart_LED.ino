/*####### arduino ble weather cast LED system code ##########

auth : park jae wan
date : 2024-06-20

file_list (1~9) : combination mp3 file
contorol BLE : send BLE serial 1~9
REV : 0.01
*/

//import header
#include <string.h>
#include <SPI.h>
#include <SdFat.h>
#include <FreeStack.h>
#include <SFEMP3Shield.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

//define
#define PIN 3         // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 36  // Popular NeoPixel ring size
#define DELAYVAL 50   // Time (in milliseconds) to pause between pixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define FILE_LIST(num) fileList##num  //thank you for OGS!!

//global variable
SdFat sd;
SFEMP3Shield MP3player;
int volume = 15, val = 0, pirState = LOW, select_case = 1, R = 0, G = 0, B = 0;
SoftwareSerial BTSerial(4, 5);  // 소프트웨어 시리얼 (TX,RX)
  //눈 비 맑음 흐림 바람 - 날씨 (눈 - 50,50,50/ 비 - 10,10,50 / 맑음 - 50,50,0 / 흐림 - 25,25,25 / 바람 - 0,0,25)
  //기온 어제보다 높다 낮다 (높다 - 50,0,0/ 낮다 - 0,0,50)
char *fileList1[] = { "1.greet.mp3", "2-1.snow.mp3", "3-1.tmep_up.mp3" };//눈 / 온도 높다
char *fileList2[] = { "1.greet.mp3", "2-1.snow.mp3", "3-2.temp_down.mp3" };//눈 / 온도 낮다
char *fileList3[] = { "1.greet.mp3", "2-2.rain.mp3", "3-1.tmep_up.mp3" };//비 / 온도 높다
char *fileList4[] = { "1.greet.mp3", "2-2.rain.mp3", "3-2.temp_down.mp3" };//비 / 온도 낮다
char *fileList5[] = { "1.greet.mp3", "2-3.clear.mp3", "3-1.tmep_up.mp3" };//맑음 / 온도 높다
char *fileList6[] = { "1.greet.mp3", "2-3.clear.mp3", "3-2.temp_down.mp3" };//맑음 / 온도 낮다
char *fileList7[] = { "1.greet.mp3", "2-4.cloudy.mp3", "3-1.tmep_up.mp3" };//흐림 / 온도 높다
char *fileList8[] = { "1.greet.mp3", "2-4.cloudy.mp3", "3-2.temp_down.mp3" };//흐림 / 온도 낮다
char *fileList9[] = { "1.greet.mp3", "2-5.windy.mp3", "3-1.tmep_up.mp3" };//바람 / 온도 높다
char *fileList10[] = { "1.greet.mp3", "2-5.windy.mp3", "3-2.temp_down.mp3" };//바람 / 온도 낮다
uint8_t result;

char **fileLists[] = { fileList1, fileList2, fileList3, fileList4, fileList5, fileList6, fileList7, fileList8, fileList9, fileList10 };

//global function
void setInitialVolume() {
  MP3player.setVolume(volume, volume);
}

void play_audio(int case_num) {
  // if (case_num < 1 || case_num > 10) {
  //   Serial.println(F("Invalid case number"));
  //   return;
  // }
  char **fileList = fileLists[case_num - 1];

  for (int i = 0; i < 3; i++) {
    Serial.print(F("Playing: "));
    Serial.println(fileList[i]);
    if (i == 0) {
      pixels.clear();
      LED_show(fileList[i]);  //white?
    } else if (i == 1) {
      pixels.clear();
      LED_show(fileList[i]);  //white?
      // LED_show(0, 50, 50);  //clear?
    } else if (i == 2) {
      pixels.clear();
      LED_show(fileList[i]);  //white?
      // LED_show(0, 0, 50);  //down?
    }
    result = MP3player.playMP3(fileList[i]);
    if (result != 0) {
      Serial.print(F("Error code: "));
      Serial.print(result);
      Serial.println(F(" when trying to play track"));
    }
    // Wait for the file to finish playing
    while (MP3player.isPlaying()) {
      delay(100);
    }
  }
}


void LED_show(String LED_case) {
  //눈 비 맑음 흐림 바람 - 날씨 (눈 - 50,50,50/ 비 - 10,10,50 / 맑음 - 50,50,0 / 흐림 - 25,25,25 / 바람 - 0,0,25)
  //기온 어제보다 높다 낮다 (높다 - 50,0,0/ 낮다 - 0,0,50)
  /*
  1.greet.mp3
  2-1.snow.mp3
  2-2.rain.mp3
  2-3.clear.mp3
  2-4.cloudy.mp3
  2-5.windy.mp3
  3-1.tmep_up.mp3
  3-2.temp_down.mp3
  */
  Serial.print("LED_case : ");
  Serial.println(LED_case);
  if(LED_case == "init"){
    R = 0, G = 0, B = 0;
  }else if(LED_case == "1.greet.mp3"){
    R = 100, G = 100, B = 100;
  }else if(LED_case == "2-1.snow.mp3"){
    R = 50, G = 50, B = 50;
  }else if(LED_case == "2-2.rain.mp3"){
    R = 10, G = 10, B = 50;
  }else if(LED_case == "2-3.clear.mp3"){
    R = 50, G = 50, B = 50;
  }else if(LED_case == "2-4.cloudy.mp3"){
    R = 25, G = 25, B = 25;
  }else if(LED_case == "2-5.windy.mp3"){
    R = 0, G = 10, B = 25;
  }else if(LED_case == "3-1.tmep_up.mp3"){
    R = 50, G = 0, B = 0;
  }else if(LED_case == "3-2.temp_down.mp3"){
    R = 0, G = 0, B = 50;
  }
  for (int i = 0; i < NUMPIXELS; i++) {  // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(R, G, B));

    pixels.show();  // Send the updated pixel colors to the hardware.

    delay(DELAYVAL);  // Pause before next pass through loop
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  Serial.begin(115200);
  BTSerial.begin(9600);

  if (!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  uint8_t result = MP3player.begin();
  if (result != 0) {
    Serial.print(F("Error code: "));
    Serial.print(result);
    Serial.println(F(" when trying to start MP3 player"));
  }

  setInitialVolume();

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();     // INITIALIZE NeoPixel strip object (REQUIRED)
  LED_show("init");  //white?
  pixels.clear();     // Set all pixel colors to 'off'
}

void loop() {
  // put your main code here, to run repeatedly:
  while (BTSerial.available()) {  //come in ble serial data
    byte data = BTSerial.read();
    Serial.write(data);
    // if (data == '1') {
    //   Serial.println("simulation case 1");
    //   select_case = int(data);
    // }
    switch (data) {
      case '0':
        select_case = 1;
        break;
      case '1':
        select_case = 2;
        break;
      case '2':
        select_case = 3;
        break;
      case '3':
        select_case = 4;
        break;
      case '4':
        select_case = 5;
        break;
      case '5':
        select_case = 6;
        break;
      case '6':
        select_case = 7;
        break;
      case '7':
        select_case = 8;
        break;
      case '8':
        select_case = 9;
        break;
      case '9':
        select_case = 10;
        break;
      default:
        Serial.print("unkonw parameter data! : ");
        Serial.println(data);
        break;
    }
    LED_show("init");  //white?
  }

  while (Serial.available()) {  //come in pc or serial data
    byte data = Serial.read();
    BTSerial.write(data);  //for debug
  }

  val = digitalRead(A0);
  if (val == HIGH) {
    if (pirState == LOW) {
      Serial.println("detect!");
      // LED_show(50, 50, 50);  //greeting and detect human
      pirState = HIGH;
      //run_play audio
      Serial.print("your case number : ");
      Serial.println(select_case);
      play_audio(select_case);  //play sound and show led color
    }
  } else {
    if (pirState == HIGH) {
      Serial.println("dissapear!");
      LED_show("init");  //white?
      pirState = LOW;
    }
  }
}
