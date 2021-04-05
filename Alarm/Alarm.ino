/*
Arduino Alarmclock
Created by Owen Ryan

Please refer to REAME.md for instructions and documentation

Licensed under The Unlicense
Feel free to copy, modify, publish, use, compile, sell, or distribute this software
For more information, please refer to <http://unlicense.org/>

Some sample code was used from the DS3231.h github repository which is also licensed under The Unlicense
https://github.com/NorthernWidget/DS3231

To switch the date format from MM/DD/YY to DD/MM/YY, go down to the bottom of the script
*/

#include <LiquidCrystal.h>
#include <Wire.h>
#include <DS3231.h> //Get DS3231 from the Tools > Manage Libraries > DS3231

RTClib myRTC;
DS3231 Clock;

//Declaring data pins
const int lcd1 = 2;
const int lcd2 = 3;
const int lcd3 = 4;
const int lcd4 = 5;
const int lcd5 = 6;
const int lcd6 = 7;
const int button1 = 8; //Up and alarm toggle button
const int button2 = 9; //Down and alarm set button
const int button3 = 10; //Set button
const int buzzerpin = 11; //Buzzer pin
const int buzzerfreq = 250; //defualt buzzer frequency
const int buzzertime = 5; //Time that each tone is played

//Custom alarm icon created using https://maxpromer.github.io/LCD-Character-Creator/
//This can be changed to whatever icon you want
byte alarmIcon[] = {
  B00000,
  B01110,
  B10101,
  B10101,
  B10111,
  B10001,
  B01110,
  B00000
};

//Change this if you are connecting the LCD to different pins than the ones showed in the readme file
LiquidCrystal lcd(lcd1, lcd2, lcd3, lcd4, lcd5, lcd6);

//The settime function is used for setting the time.
//This had to go before setup() and loop() for it to work with older versions of the Arduino IDE
int settime(String timeunit, int maxnumber = 59, int minnumber = 0)
{
  int time = minnumber;
  int settime = 1;
  while(settime == 1)
    {
      int upstate, downstate, setstate;
      bool dodelay;
      lcd.setCursor(0, 0);
      lcd.print("Set ");
      lcd.print(timeunit);
      lcd.setCursor(0, 1);
      lcd.print(time);
      lcd.print(" ");
      if(dodelay == true)
      {
        delay(250);
        dodelay = false;
      }
      upstate = digitalRead(button1); //The button# variables are global and can be accessed from inside of a function
      downstate = digitalRead(button2);
      setstate = digitalRead(button3);
      if(upstate == LOW)
      {
        tone(buzzerpin, buzzerfreq, buzzertime);
        dodelay = true;
        if(time == maxnumber)
        {
          time = minnumber;
        }
        else
        {
          time = time + 1;
        }
      }
      if(downstate == LOW)
      {
        tone(buzzerpin, buzzerfreq, buzzertime);
        dodelay = true;
        if(time == minnumber)
        {
          if(maxnumber > 0)
          {
            time = maxnumber;
          }
        }
        else
        {
          time = time - 1;
        }
      }
      if(setstate == LOW)
      {
        tone(buzzerpin, buzzerfreq, buzzertime);
        return(time);
      }
    }
}

void setup() {

  //Initiate the connection to the RTC module and buttons
  Wire.begin();
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);

  //This may be different for your LCD.
  //Change to (number of characters on one line, number of lines)
  lcd.begin(16, 2);

  lcd.createChar(1, alarmIcon); //Creating the alarmIcon LCD character in slot 1

  //All of this is for setting the time
  Clock.setClockMode(false); //Sets to 24 hour mode
  Clock.setHour(settime("Hour  ", 23)); //The settime function has a default max value of 60, so we only have to declare it here
  delay(500);
  Clock.setMinute(settime("Minute"));
  delay(500);
  Clock.setSecond(settime("Second"));
  delay(500);
  Clock.setDate(settime("Day   ", 31, 1));
  delay(500);
  Clock.setMonth(settime("Month", 12, 1));
  delay(500);
  Clock.setYear(settime("Year  ", -1, 2000) - 48); //Had to subtract 48 because the RTC module's system time starts in 1973
  lcd.clear();
}

void loop() {
  DateTime now = myRTC.now();
  int hour, day, month, year;
  bool alarm;
  int button1state = digitalRead(button1); //Button 1 is the alarm toggle button
  int button2state = digitalRead(button2); //Button 2 is the alarm set button

  if(button1state == LOW)
  {
    lcd.setCursor(0, 0); //We are going to be printing something to the LCD no matter whhat the outcome of the if statement is
    if(alarm == true)
    {
      //Turns the alarm off
      alarm = false;
      lcd.print("Alarm OFF");
    }
    else //If alarm == false
    {
      //Turns the alarm on
      alarm = true;
      lcd.print("Alarm ON");
    }
    delay(2000);
  }

  if(button2state == LOW)
  {
    tone
    //Put alarm set code here
  }

  hour = now.hour();
  day = now.day();
  month = now.month();
  year = now.year();

  //This code block just displays the current time and date onto the lcd
  lcd.setCursor(0, 0);
  lcd.print(hour);
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
  lcd.print("     ");
  lcd.setCursor(15, 0);
  if(alarm == true)
  {
    //Prints the alarm indocator in the top corner
    lcd.write(1);
  }
  else
  {
    lcd.print("  ");
  }

  lcd.setCursor(0, 1); //Moves down to second line for printing the date

  //To change the format just comment out the US code block and uncomment the EU code block

  //This code prints the US date format MM/DD/YY
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.year());

  //This code prints in the EU date format DD/MM/YY
  /*
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());
  */

  delay(1000);
}
