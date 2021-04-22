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
const int buzzertime = 5; //Time that each tone is played (MS)

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

//Change this if you are connecting the LCD to different pins than the ones showed in the readme file and diagram
LiquidCrystal lcd(lcd1, lcd2, lcd3, lcd4, lcd5, lcd6);

//This function detects if a button is being pressed
bool buttonpressed(int pin)
{
  if(digitalRead(pin) == LOW)
  {
    return true;
  }
  else
  {
    return false;
  }
}

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
      if(buttonpressed(button1))
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
      if(buttonpressed(button2))
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
      if(buttonpressed(button3))
      {
        tone(buzzerpin, buzzerfreq, buzzertime);
        return(time);
      }
    }
}

void completeset()
{
  DateTime now = myRTC.now();
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
}

void setup() {
  bool setloop = true;
  //Initiate the connection to the RTC module and buttons
  Wire.begin();
  DateTime now = myRTC.now();
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);

  //This may be different for your LCD.
  //Change to (number of characters on one line, number of lines)
  lcd.begin(16, 2);

  lcd.createChar(1, alarmIcon); //Creating the alarmIcon LCD character in slot 1
}

void loop() {
  DateTime now = myRTC.now();
  int hour, day, month, year;
  bool alarm;

  if(buttonpressed(button1)) //Button 1 is the alarm toggle button
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    if(alarm == true)
    {
      //Turns the alarm off
      alarm = false;
      lcd.print("Alarm OFF");
      //Put code here that changes the RTC alarm to OFF
    }
    else //If alarm == false
    {
      //Turns the alarm on
      alarm = true;
      lcd.print("Alarm ON");
      //Put code here that changes the RTC alarm to ON
    }
    delay(2000);
  }

  if(buttonpressed(button2)) //Button 2 is the alarm set button
  {
    //Put alarm set code here
  }

  if(buttonpressed(button3)) //Button 3 will set the time
  {
    lcd.clear();
    completeset();
  }

  //This code block just displays the current time and date onto the lcd
  lcd.setCursor(0, 0);
  lcd.print(now.hour());
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
