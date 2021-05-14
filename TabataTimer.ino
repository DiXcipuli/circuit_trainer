
#include <LiquidCrystal.h>
#include <Thread.h>
#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"

TMRpcm tmrpcm;

Thread myThread = Thread();

int SD_ChipSelectPin = 8;
int buzzer_pin = 3;

String exercices[] = {"Warming up", "Right Knee", "Waving abs", "Iron Fist", "Jump n Jack", "Lunges", "Cladding", "Right Kick", "Deeps", "Squat", "Push up", "Left Kick", "Lift Weight", "Left Knee", "Head Lift", "Mountain Climbers"};
const int exercices_duration = 20;
const int warming_up_duration = 60;
const int break_duration = 10;
const int initial_delay = 3;
int time_ref;
int timer;

int i = 0;
bool break_time = false;

const int rs = 7, en = 6, d4 = 2, d5 = 10, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void buzzerCallback(){
  tone(buzzer_pin, 1000);
  delay(100);
  noTone(buzzer_pin);
}

void setup() {

  myThread.onRun(buzzerCallback);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  tmrpcm.speakerPin = 9;
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin)) {
  // Print a message to the LCD.
  lcd.print("SD failed");
  return;
  }
  
  tmrpcm.setVolume(6);
  tmrpcm.play("test.wav");
  if (tmrpcm.isPlaying()) {
  // Print a message to the LCD.
  lcd.print("Tabata!!!");
  }
  else{
  // Print a message to the LCD.
  lcd.print("Issue with track");
  }

  delay(initial_delay * 1000);

  time_ref = millis()/1000;
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0): 

  if(break_time){
    timer = time_ref + break_duration - millis()/1000;
    lcd.setCursor(0, 0);
    lcd.print("Break");
    lcd.setCursor(14, 0);
    lcd.print(timer);
    lcd.setCursor(0,1);
    lcd.print(exercices[i]);
    lcd.setCursor(14,1);
    lcd.print(exercices_duration);
  }
  else{
    if(i == 0)
      timer = time_ref + warming_up_duration - millis()/1000;
    else{
      timer = time_ref + exercices_duration - millis()/1000;
    }
      
    lcd.setCursor(0, 0);
    lcd.print(exercices[i]);
    lcd.setCursor(14, 0);
    lcd.print(timer);
    lcd.setCursor(0, 1);
    lcd.print("Break");
    lcd.setCursor(14, 1);
    lcd.print(break_duration);
  }

  if(timer == 9){
    lcd.setCursor(15, 0);
    lcd.print(" ");
  }
    
  if(timer == 0){
    if(!break_time)
      i++;
    break_time = !break_time;
    time_ref = millis()/1000;
    lcd.clear();
    myThread.run();
  }
}
