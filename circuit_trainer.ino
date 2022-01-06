// circuit_trainer.ino
// Made to automatize a training session:
// Plays a song, displays and times each exercice, and beeps when each is over.


#include <LiquidCrystal.h>
#include <Thread.h>
#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"

// Pin def
const int buzzer_pin 3;
const int SD_ChipSelectPin = 8;                             // Needed for the sd card reader
const int rs = 7, en = 6, d4 = 2, d5 = 10, d6 = 4, d7 = 5;  // For the LCD display


// Music player object
TMRpcm tmrpcm;
// Thread, that will trigger the buzzer everytime the exercice changes.
Thread myThread = Thread();
//LCD display
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


String warming_up_msg = "Warming up!";
String break_msg = "Break";
// Stores the name of all exercices
String exercices[] = {"Right Knee", "Waving abs", "Iron Fist", "Jump n Jack", "Lunges", "Cladding", "Right Kick", "Deeps", "Squat", "Push up", "Left Kick", "Lift Weight", "Left Knee", "Head Lift", "Mountain Climbers"};

const int exercices_duration = 20;
const int warming_up_duration = 60;
const int break_duration = 10;
const int initial_delay = 3;

int time_ref;   // Holds the time of the last state change (from exercice to break or vice versa), in order to set the timer correctly
int timer;      // It will keep decresing from 'exercice_duration' to 0 and from 'break_duration' to 0

int exercice_index = 0;
bool warming_up = true;
bool break_time = false;


void setup() {
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  if (!SD.begin(SD_ChipSelectPin)) {
    lcd.print("SD failed");
    return;
  }

  // Setting up the player
  tmrpcm.speakerPin = 9;
  tmrpcm.setVolume(6);
  tmrpcm.play("test.wav");
  if (tmrpcm.isPlaying())
    lcd.print("Tabata!!!");
  else
    lcd.print("Issue with track");
  

  // Set the function to be called in a thread
  myThread.onRun(buzzerCallback);

  delay(initial_delay * 1000);

  time_ref = millis()/1000;
}


void loop() {
  if(warming_up){
    timer = time_ref + warming_up_duration - millis()/1000;
    update_display(warming_up_msg, timer, exercices[exercice_index], exercices_duration);
  }
  else if(break_time){
    timer = time_ref + break_duration - millis()/1000;
    update_display(break_msg, timer, exercices[exercice_index], exercices_duration);
  }
  else{
    timer = time_ref + exercices_duration - millis()/1000;
    update_display(exercices[exercices_duration], timer, break_msg, break_duration);
  }

  // When switching from 2 digits (10) to 1 (9), we need to erase the first digit, otherwise the '1' would still be displayed
  if(timer == 9){
    lcd.setCursor(15, 0);
    lcd.print(" ");
  }
    
  // When timer reaches 0, change state.
  if(timer == 0){
    if(warming_up){
      warming_up = false;
    }
    else{
      if(!break_time) 
        i++;
      
      break_time = !break_time;
    }

    time_ref = millis()/1000;
    lcd.clear();
    myThread.run();
  }
}


void update_display(std::string msg_1, int timer, std::String msg_2, int duration){ //msg_1 is the current exercice, and msg_2 is the next one, with 
  lcd.setCursor(0, 0);                                                              // its duration
  lcd.print(msg_1);
  lcd.setCursor(14, 0);
  lcd.print(timer);
  lcd.setCursor(0,1);
  lcd.print(msg_2);
  lcd.setCursor(14,1);
  lcd.print(duration);
}


void buzzerCallback(){
  tone(buzzer_pin, 1000);
  delay(100);
  noTone(buzzer_pin);
}