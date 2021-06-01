/*
Arduino Alarmclock
Created by Owen Ryan

Please refer to REAME.md for instructions and documentation

Licensed under The Unlicense
Feel free to copy, modify, publish, use, compile, sell, or distribute this software
For more information, please refer to <http://unlicense.org/>

Some sample code was used from the DS3231.h github repository which is also licensed under The Unlicense
https://github.com/NorthernWidget/DS3231

To switch the date format from MM/DD/YY to DD/MM/YY or YYYY/MM/DD, go down to the bottom of the script
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

//These two variables are for the clicks when you press a button
const int buzzerfreq = 250; //defualt buzzer frequency
const int buzzertime = 5; //Time that each tone is played (MS)

const int alarmtime = 500; //How long the alarm beep is (MS)
const int alarmfreq = 450; //Alarm frequency

int oldhour = -1; //Used for limiting the polling of day, month, and year from RTC

//Custom  alarm icon created using https://maxpromer.github.io/LCD-Character-Creator/
//This can be changed to whatever icon you want
byte alarmIcon[] = {
  B00000,
  B01110,
  B10101,
  B10101,
  B10111,
  B10001,
  B01110,
  B00000};

//Change this if you are connecting the LCD to different pins than the ones showed in the readme file and diagram
LiquidCrystal lcd(lcd1, lcd2, lcd3, lcd4, lcd5, lcd6);

//This function detects if a button is being pressed
bool buttonpressed(int pin)
{
  if (digitalRead(pin) == LOW) return true;
  else return false;
}

//The settime function is used for setting the time.
//This had to go before setup() and loop() for it to work with older versions of the Arduino IDE
int settime(String timeunit, int currentnumber, int maxnumber = 59, int minnumber = 0)
{
  int time;

  //Might not be needed, may be removed for final release
  if(currentnumber < minnumber) time = minnumber;
  else time = currentnumber;

  lcd.clear();
  while(true)
    {
      bool dodelay;
      lcd.setCursor(0, 0);
      lcd.print("Set ");
      lcd.print(timeunit);
      lcd.setCursor(0, 1);
      lcd.print(time);
      lcd.print(" ");
      if(dodelay)
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
          time++;
        }
      }
      if(buttonpressed(button2))
      {
        tone(buzzerpin, buzzerfreq, buzzertime);
        dodelay = true;
        if(time == minnumber)
        {
          time = maxnumber;
        }
        else
        {
          time--;
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
  Clock.setHour(settime("Hour", now.hour(), 23)); //The settime function has a default max value of 60, so we only have to declare it here
  delay(500);
  Clock.setMinute(settime("Minute", now.minute()));
  delay(500);
  Clock.setSecond(settime("Second", now.second()));
  delay(500);
  Clock.setDate(settime("Day", now.day(), 31, 1));
  delay(500);
  Clock.setMonth(settime("Month", now.month(), 12, 1));
  delay(500);
  Clock.setYear(settime("Year", now.year(), -1, 2000) - 48); //Had to subtract 48 because the RTC module's system time starts in 1973
}

void playalarm(){
  int alarmdelay = alarmtime;
  bool runalarm = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm");
  while(runalarm){
    tone(buzzerpin, alarmfreq);
    if (buttonpressed(button1) ||  buttonpressed(button2) || buttonpressed(button3))
    {
      //When button is pressed alarm is stopped
      runalarm = false;
      alarmdelay = 0;
    }
    delay(alarmdelay);
    noTone(buzzerpin);
    delay(alarmdelay);
  }
}

void setup() {
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
  int second, hour, minute, day, month, year, alarmhour, alarmmin;
  bool alarm;

  hour = now.hour();
  minute = now.minute();
  second = now.second();

  //Made to limit polling for month, day and year to preserve RTC battery
  if(hour != oldhour){
    month = now.month();
    day = now.day();
    year = now.year();
  }

  if(buttonpressed(button1)) //Button 1 is the alarm toggle button
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    if(alarm)
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

  if(buttonpressed(button2)) //Button 2 is the alarm set button
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Alarm");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0 ,0);
    int alarmhour = settime("Alarm Hour", alarmhour, 23);
    delay(500);
    int alarmmin = settime("Alarm Minute", alarmmin);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm Set For:");
    lcd.setCursor(0, 1);
    lcd.print(alarmhour);
    lcd.print(":");
    lcd.print(alarmmin);
    delay(1500);
    lcd.clear();
  }

  if(buttonpressed(button3)) //Button 3 will set the time
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set time");
    delay(1500);
    completeset();
  }

  //This code block just displays the current time and date onto the lcd
  lcd.setCursor(0, 0);
  lcd.print(hour);
  lcd.print(":");
  lcd.print(minute);
  lcd.print(":");
  lcd.print(now.second());
  lcd.print("     ");
  //lcd.setCursor(15, 0);

  if(alarm && hour == alarmhour && minute == alarmmin && second <= 3)
  {
    playalarm();
  }

  lcd.setCursor(15, 0);
  if(alarm){
    lcd.write(1);
  }
  else{
    lcd.print("  ");
  }
  //Print Alarm symbol

  lcd.setCursor(0, 1); //Moves down to second line for printing the date

  //To change the format just comment out the US code block and uncomment the EU code block

  //This code prints the US date format MM/DD/YYYY
  lcd.print(month);
  lcd.print("/");
  lcd.print(day);
  lcd.print("/");
  lcd.print(year);

  //This code prints in the EU date format DD/MM/YYYY
  /*
  lcd.print(day);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);
  */

  //This code prints in the ISO date format YYYY/MM/DD
  /*
  lcd.print(year);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(day);
  */

  delay(1000);
}
