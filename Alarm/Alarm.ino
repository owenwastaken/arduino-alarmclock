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

If you are using a LCD display that is not 16 by 2, you will need to do some changing to the UI.
*/

#include <LiquidCrystal.h>
#include <Wire.h>
#include <DS3231.h> //Get DS3231 from the Tools > Manage Libraries > DS3231

RTClib myRTC;
DS3231 Clock;

//Declaring data pins
const int lcd1 = 2;
const int lcd2 = 4;
const int lcd3 = 7;
const int lcd4 = 8;
const int lcd5 = 12;
const int lcd6 = 13;
const int button1 = 3; //Up and alarm toggle button
const int button2 = 5; //Down and alarm set button
const int button3 = 6; //Set button
const int buzzerpin = 9; //Buzzer pin

//These two variables are for the clicks when you press a button
const int buzzerfreq = 250; //defualt buzzer frequency
const int buzzertime = 5; //Time that each tone is played (MS)

const int alarmtime = 500; //How long the alarm beep is (MS)
const int alarmfreq = 450; //Alarm frequency

int oldhour = -1; //Used for limiting the polling of day, month, and year from RTC

//Needed to be declared as false at initiation
bool alarm = false;

//Initiating alarm time as a negative number to prevent people from turning on the alarm before the time is set
int alarmmin = -1; int alarmhour = -1;

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

//Initiating LCD with pins set above
LiquidCrystal lcd(lcd1, lcd2, lcd3, lcd4, lcd5, lcd6);

//Returns a boolean answer on whether a button on a specific pin is currently being pressed.
bool buttonpressed(int pin) {if (digitalRead(pin) == HIGH) return true; else return false;}

//The settime function is used for setting the time.
//This had to go before setup() and loop() for it to work with older versions of the Arduino IDE
int settime(String timeunit, int currentnumber, int maxnumber = 59, int minnumber = 0)
{
  int time;

  //Only used if the RTC's year is below 2000
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
      if(dodelay) {delay(250); dodelay = !dodelay;}
      //If there is a delay requested it will do the delay and turn off the request
      if(buttonpressed(button1))
      {
        tone(buzzerpin, buzzerfreq, buzzertime);
        dodelay = true;
        if(time == maxnumber) time = minnumber;
        else time++;
      }
      if(buttonpressed(button2))
      {
        tone(buzzerpin, buzzerfreq, buzzertime);
        dodelay = true;
        if(time == minnumber) time = maxnumber;
        else time--;
      }
      //Button3 returns the value (SET BUTTON)
      if(buttonpressed(button3)) {tone(buzzerpin, buzzerfreq, buzzertime); return(time);}
    }
}

void completeset()
{
  DateTime now = myRTC.now();
  //All of this is for setting the time
  Clock.setClockMode(false); //Sets to 24 hour mode
  Clock.setHour(settime("Hour", now.hour(), 23)); //The settime function has a default max value of 60, so we only have to declare it here
  delay(750);
  Clock.setMinute(settime("Minute", now.minute()));
  delay(750);
  Clock.setSecond(settime("Second", now.second()));
  delay(750);
  Clock.setDate(settime("Day", now.day(), 31, 1));
  delay(750);
  Clock.setMonth(settime("Month", now.month(), 12, 1));
  delay(750);
  Clock.setYear(settime("Year", now.year(), -1, 2000) - 48); //Had to subtract 48 because the RTC module's system time starts in 1952
}

void playalarm(){
  int alarmdelay = alarmtime;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm");
  while(true){
    tone(buzzerpin, alarmfreq);
    //When button is pressed alarm is stopped. This might make a harmless error when compiling but can be ignored
    if (buttonpressed(button1) ||  buttonpressed(button2) || buttonpressed(button3)) return 0;
    delay(alarmdelay);
    noTone(buzzerpin);
    delay(alarmdelay);
  }
}

void setup() {
  //Initiate the connection to the RTC module and buttons
  Wire.begin();
  DateTime now = myRTC.now();
  pinMode(button1, INPUT); pinMode(button2, INPUT); pinMode(button3, INPUT);

  //This may be different for your LCD.
  //Change to (number of characters on one line, number of lines)
  lcd.begin(16, 2);
  lcd.createChar(1, alarmIcon); //Creating the alarmIcon LCD character in slot 1
}

void loop() {
  DateTime now = myRTC.now();
  int second, hour, minute, day, month, year;

  //Fetches time data from RTC
  hour = now.hour(); minute = now.minute(); second = now.second();

  //Made to limit polling for month, day and year to preserve RTC battery
  if(hour != oldhour){month = now.month(); day = now.day(); year = now.year();}

  if(buttonpressed(button1)) //Button 1 is the alarm toggle button
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    if(alarmhour != -1 || alarmmin != -1){
      alarm = !alarm;
      if(alarm) lcd.print("Alarm ON");
      else lcd.print("Alarm OFF");
    }
    else
    {
      lcd.print("No alarm set");
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
    alarmhour = settime("Alarm Hour", alarmhour, 23);
    delay(750);
    alarmmin = settime("Alarm Minute", alarmmin);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Alarm Set For:");
    lcd.setCursor(0, 1);
    lcd.print(alarmhour);
    lcd.print(":");
    if (alarmmin < 10) lcd.print("0");
    lcd.print(alarmmin);
    alarm = true;
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
  if (minute < 10) lcd.print("0");
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) lcd.print("0");
  lcd.print(second);
  lcd.print("     ");

  //Checks if it is time to play alarm and if so it runs the alarm function
  if(alarm && hour == alarmhour && minute == alarmmin && second <= 3) playalarm();

  //Prints alarm symbol if alarm is on and blanks area if it is not
  lcd.setCursor(15, 0);
  if(alarm) lcd.write(1);
  else lcd.print(" ");

  lcd.setCursor(0, 1); //Moves down to second line for printing the date

  //To change the format just comment out the US code block and uncomment the EU code block

  //This code prints the US date format MM/DD/YYYY
  lcd.print(month);
  lcd.print("/");
  lcd.print(day);
  lcd.print("/");
  lcd.print(year);
  lcd.print("   ");

  //This code prints in the EU date format DD/MM/YYYY
  /*
  lcd.print(day);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);
  lcd.print("   ");
  */

  //This code prints in the ISO date format YYYY/MM/DD
  /*
  lcd.print(year);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(day);
  lcd.print("   ");
  */

  //Actual time counting is done on the RTC so this can be changed if you want a higher polling rate
  delay(1000);
}
