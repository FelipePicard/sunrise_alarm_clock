#include <Encoder.h>
#include "RTClib.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define r 3
#define g 5
#define b 6
#define mic 7
#define btn 4
int btnState = HIGH;
long count = 1;

RTC_DS1307 rtc;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels


#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int micToggled = 0;
int volume;

// previous time
long pt = 0;

int claps = 0;

// wake up hours
int h = 6;
// wake up minutes
int m = 15;

// time till alarm is fully lit
int timeout = 30*60;

int rval;
int gval;
int bval;

// initial values for each color
int ri = 0;
int gi = 0;
int bi = 0;

// threshold values for each color
int rf = 255;
int gf = 40;
int bf = 20;

int sub = 0;

void setup() {
  delay(10);
  
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(mic, INPUT);
  pinMode(btn, INPUT_PULLUP);

  Serial.begin(57600);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  
  //rtc.adjust(DateTime(2022, 5, 7, 12, 28, 20));
  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
}

void loop() {
  micToggled = digitalRead(mic);

  // absolute time
  DateTime now = rtc.now();

  btnState = digitalRead(btn);

  display.clearDisplay();

  // current relative time
  unsigned long ct = millis();

  long timer = ct - pt;

  if(btnState == LOW){
    count++;
    delay(600);
  }


  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  /////// LIGHT UP WITH CLAPS LOGIC ////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  if(now.hour() > 8 || now.hour() < 3){
  if (micToggled == HIGH){
    // start timer
    pt = ct;
    if(timer > 100 && timer < 800){
      if (micToggled == HIGH){
        //claps++;
        count++;
      }
    }
  }
/*
  if (claps % 2 == 0){
    setColor(0, 0, 0);
  }

  else if (claps % 2 != 0){
    setColor(255, 255, 255);
  }
*/
  if(count%2 == 0){
    setColor(255, 255, 255);
  }
  else if(count%2 != 0){
    setColor(0,0,0);
  }
  }
  
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  /////// SUNRISE ALARM CLOCK LOGIC ////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////


  else if(now.hour() > 3 && now.hour() < 8){
  int elapsed_time = (now.hour() - h)*360 + (now.minute() - m)*60 + (now.second());
  int percentage = map(elapsed_time, 0, timeout, 0, 100);
  
  if(elapsed_time < timeout){
    rval = map(percentage, 0, 60, ri, rf);
    gval = map(percentage, 25, 100, gi, gf);
    bval = map(percentage, 10, 70, bi, bf);

    if(rval<0){
      rval=0;
    }
    if(gval<0){
      gval=0;
    }
    if(bval<0){
      bval=0;
    }

    if(rval>rf){
      rval=rf;
    }
    if(gval>gf){
      gval=gf;
    }
    if(bval>bf){
      bval=bf;
    }
  }

  else if (elapsed_time >= timeout && elapsed_time < (timeout + 15*60)){
    rval = rf;
    gval = gf;
    bval = bf;
  }

  else if (elapsed_time >= (timeout + 15*60)){
    setColor(0,0,0);
  }

  setColor(rval, gval, bval);
  }

  //Serial.print(rval); Serial.print(",    ");Serial.print(gval); Serial.print(",    ");Serial.println(bval);

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////


  // display current time
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(now.hour(), DEC);
  display.print(':');
  display.print(now.minute(), DEC);
  display.print(':');
  display.print(now.second(), DEC);
  
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 15);
  display.print(daysOfTheWeek[now.dayOfTheWeek()]);

  display.display();

  //Serial.print(now.hour(), DEC);
  //Serial.print(':');
  //Serial.print(now.minute(), DEC);
  //Serial.print(':');
  //Serial.print(now.second(), DEC);
  //Serial.println();
}
/*
void button(long count){
  if(count%2 == 0){
    setColor(255,0,0);
  }
  else if(count%2 != 0){
    setColor(0,0,0);
  }
}*/

void setColor(int red, int green, int blue)
{
  analogWrite(r, red);
  analogWrite(g, green);
  analogWrite(b, blue);  
}
