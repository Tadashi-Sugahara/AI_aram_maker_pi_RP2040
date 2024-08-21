#include <Arduino.h>

#ifndef PIN_BUZZER
#define PIN_BUZZER    A0
#endif


const int ultrasonicPin = 1; // GPIO1に接続
uint8_t const pin_buzzer = PIN_BUZZER;

#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

#define WHOLE         2200       // Length of time in milliseconds of a whole note (i.e. a full bar).
#define HALF          WHOLE/2
#define QUARTER       HALF/2
#define EIGHTH        QUARTER/2
#define EIGHTH_TRIPLE QUARTER/3
#define SIXTEENTH     EIGHTH/2

void playNote(int frequency, int duration, bool hold = false, bool measure = true) {
  (void) measure;

  if (hold) {
    // For a note that's held play it a little longer than the specified duration
    // so it blends into the next tone (but there's still a small delay to
    // hear the next note).
    tone(pin_buzzer, frequency, duration + duration / 32);
  } else {
    // For a note that isn't held just play it for the specified duration.
    tone(pin_buzzer, frequency, duration);
  }

  delay(duration + duration / 16);
}

void wakeupMusic() {
  // Play a little charge melody, from:
  //  https://en.wikipedia.org/wiki/Charge_(fanfare)
  // Note the explicit boolean parameters in particular the measure=false
  // at the end.  This means the notes will play without any breath measurement
  // logic.  Without this false value playNote will try to keep waiting for candles
  // to blow out during the celebration song!
  playNote(NOTE_G4, EIGHTH_TRIPLE, true, false);
  playNote(NOTE_C5, EIGHTH_TRIPLE, true, false);
  playNote(NOTE_E5, EIGHTH_TRIPLE, false, false);
  playNote(NOTE_G5, EIGHTH, true, false);
  playNote(NOTE_E5, SIXTEENTH, false);
  playNote(NOTE_G5, HALF, false);
}

void setup() {
  Serial.begin(9600); // シリアル通信を9600bpsで開始
  pinMode(ultrasonicPin, OUTPUT); // 最初は送信モードに設定

  // ブザー関連設定
  pinMode(pin_buzzer, OUTPUT);
  digitalWrite(pin_buzzer, LOW);
}

void loop() {
  char key; // シリアル通信の受信文字
  long duration, distance;


  // シリアル通信の文字列チェック
  if ( Serial.available()){
    key = Serial.read();
  }

  // シリアル通信で文字”１”を受信するまでは待機
  if (key == '1'){
    
    // 超音波センサで対象者との距離を10cm以下になるまで測定し、測定中はブザーを鳴らし続ける

    while (distance > 10){

      Serial.println("Wake up music start.");
      wakeupMusic();   // 目覚まし音楽を鳴らす
      
      // 超音波信号を送信
      digitalWrite(ultrasonicPin, LOW); // 送信前にピンをLOWに設定
      delayMicroseconds(2); // 2マイクロ秒待つ
      digitalWrite(ultrasonicPin, HIGH); // HIGHにして信号を送信
      delayMicroseconds(10); // 10マイクロ秒待つ
      digitalWrite(ultrasonicPin, LOW); // 送信終了
  
      // 受信モードに切り替え
      pinMode(ultrasonicPin, INPUT);
  
      // 超音波が反射して戻ってくるまでの時間を計測
      duration = pulseIn(ultrasonicPin, HIGH);
  
      // 距離を計算 (音速 = 343m/s、音速の半分を使用して計算)
      distance = (duration / 2) / 29.1;
  
      // 距離をシリアルモニタに出力
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println("cm");
  
      // 送信モードに戻す
      pinMode(ultrasonicPin, OUTPUT);
  
      // 次の測定まで1秒待つ
      delay(1000);
    }

    Serial.println("Good morning!");
  }
 
  

}
