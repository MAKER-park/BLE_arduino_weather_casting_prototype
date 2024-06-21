/*####### arduino ble weather cast LED system code ##########

auth : park jae wan
date : 2024-06-20

file_list (1~9) : combination mp3 file
contorol BLE : send BLE serial 1~9
REV : 0.01
*/

//import header
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
#define DELAYVAL 50  // Time (in milliseconds) to pause between pixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//global variable
SdFat sd;
SFEMP3Shield MP3player;
int volume = 15,val = 0, pirState = LOW;
SoftwareSerial BTSerial(4, 5);  // 소프트웨어 시리얼 (TX,RX)

//global function
void setInitialVolume() {
  MP3player.setVolume(volume, volume);
}

void LED_show(int R, int G, int B){
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(R, G, B));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
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
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  LED_show(0,0,0);//white?
  pixels.clear(); // Set all pixel colors to 'off'
}

void loop() {
  char *fileList1[] = { "1.greet.mp3", "2-1.clear.mp3", "3-1.temp_down.mp3" };
  char *fileList2[] = { "1.greet.mp3", "2-1.clear.mp3", "3-1.temp_down.mp3" };
  char *fileList3[] = { "1.greet.mp3", "2-1.clear.mp3", "3-1.temp_down.mp3" };
  char *fileList4[] = { "1.greet.mp3", "2-1.clear.mp3", "3-1.temp_down.mp3" };
  char *fileList5[] = { "1.greet.mp3", "2-1.clear.mp3", "3-1.temp_down.mp3" };
  char *fileList6[] = { "1.greet.mp3", "2-1.clear.mp3", "3-1.temp_down.mp3" };
  char *fileList7[] = { "1.greet.mp3", "2-1.clear.mp3", "3-1.temp_down.mp3" };
  uint8_t result;
  // put your main code here, to run repeatedly:

  while (BTSerial.available()) {  //come in ble serial data
    byte data = BTSerial.read();
    Serial.write(data);
    if (data == '1') {
      Serial.println("simulation case 1");
      for (int i = 0; i < 3; i++) {
        Serial.print(F("Playing: "));
        Serial.println(fileList1[i]);
        if(i == 0){
          pixels.clear();
          LED_show(50,50,50);//white?
        }else if(i == 1){
          pixels.clear();
          LED_show(0,50,50);//clear?
        }else if(i == 2){
          pixels.clear();
          LED_show(0,0,50);//down?
        }
        result = MP3player.playMP3(fileList1[i]);
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
    LED_show(0,0,0);//white?
  }

  while (Serial.available()) {  //come in pc or serial data
    byte data = Serial.read();
    BTSerial.write(data);  //for debug
  }

  val = digitalRead(A0);
  if(val == HIGH){
    if(pirState == LOW){
      Serial.println("detect!");
      LED_show(50,50,50);//white?
      pirState = HIGH;
    }
  }else{
    if(pirState == HIGH){
      Serial.println("dissapear!");
      LED_show(0,0,0);//white?
      pirState = LOW;
    }
  }
}
