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

const int secstillsleep = 180; //The device will sleep after this many seconds after last interaction

int oldhour = -1; //Used for limiting the polling of day, month, and year from RTC

//Needed to be declared as false at initiation
bool alarm = false;

//Used for determining when to enter sleep mode
int sleepcountdown = secstillsleep;

//Initiating alarm time as a negative number to prevent people from turning on the alarm before the time is set
int alarmmin = -1; int alarmhour = -1;

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
  B00000};

//Initiating LCD with pins set above
LiquidCrystal lcd(lcd1, lcd2, lcd3, lcd4, lcd5, lcd6);

//Returns a boolean answer on whether a button on a specific pin is currently being pressed.
bool buttonpressed(int pin) {if (digitalRead(pin) == HIGH) return true; else return false;}

//The following block of functions are commonly used commands that have been turned into functions to save space
void resetlcdcursor() {lcd.setCursor(0, 0);}
void printzero(int number) {if (number < 10) lcd.print("0");}

//The settime function is used for setting the time.
//This had to go before setup() and loop() for it to work with older versions of the Arduino IDE
int settime(String timeunit, int currentnumber, int maxnumber = 59, int minnumber = 0) {
  int time;

  //If the RTC's year is below 2000 it will set it to 2000 to prevent having to manually scroll from year 0 to 2000
  if(currentnumber < minnumber) time = minnumber;
  else time = currentnumber;

  lcd.clear();
  resetlcdcursor();
  lcd.print(F("Set "));
  lcd.print(timeunit);
  bool dodelay;
  while(true) {
      lcd.setCursor(0, 1);
      printzero(time);
      lcd.print(time);
      if(dodelay) {delay(250); dodelay = !dodelay;}
      //If there is a delay requested it will do the delay and turn off the request

      if(buttonpressed(button1)) {
        tone(buzzerpin, buzzerfreq, buzzertime);
        dodelay = true;
        if(time == maxnumber) time = minnumber;
        else time++; }

      if(buttonpressed(button2)) {
        tone(buzzerpin, buzzerfreq, buzzertime);
        dodelay = true;
        if(time == minnumber) time = maxnumber;
        else time--; }

      //Button3 returns the value (SET BUTTON)
      if(buttonpressed(button3)) {tone(buzzerpin, buzzerfreq, buzzertime); return(time);} } }

void completeset() {
  DateTime now = myRTC.now();
  //All of this is for setting the time
  Clock.setClockMode(false); //Sets to 24 hour mode
  Clock.setHour(settime(F("Hour"), now.hour(), 23)); //The settime function has a default max value of 60, so we only have to declare it here
  delay(750);
  Clock.setMinute(settime(F("Minute"), now.minute()));
  delay(750);
  Clock.setSecond(settime(F("Second"), now.second()));
  delay(750);
  Clock.setDate(settime(F("Day"), now.day(), 31, 1));
  delay(750);
  Clock.setMonth(settime(F("Month"), now.month(), 12, 1));
  delay(750);
  Clock.setYear(settime(F("Year"), now.year(), -1, 2000) - 48); } //Had to subtract 48 because the RTC module's system time starts in 1952

void playalarm() {
  int alarmdelay = alarmtime;
  lcd.clear();
  resetlcdcursor();
  lcd.print(F("Alarm"));
  while(true) {
    tone(buzzerpin, alarmfreq);
    //When button is pressed alarm is stopped. This might make a harmless error when compiling but can be ignored
    if (buttonpressed(button1) ||  buttonpressed(button2) || buttonpressed(button3)) {noTone(buzzerpin); return 0;}
    delay(alarmdelay);
    noTone(buzzerpin);
    delay(alarmdelay); } }

void resetsleepcounter() {sleepcountdown = secstillsleep;}

void setup() {
  //Initiate the connection to the RTC module and buttons
  Wire.begin();
  DateTime now = myRTC.now();
  pinMode(button1, INPUT); pinMode(button2, INPUT); pinMode(button3, INPUT);

  //This may be different for your LCD.
  //Change to (number of characters on one line, number of lines)
  lcd.begin(16, 2);
  lcd.createChar(1, alarmIcon); }//Creating the alarmIcon LCD character in slot 1

void loop() {
  DateTime now = myRTC.now();
  int second, hour, minute, day, month, year;

  //Fetches time data from RTC
  hour = now.hour(); minute = now.minute(); second = now.second();

  //Made to limit polling for month, day and year to preserve RTC battery
  if(hour != oldhour) {month = now.month(); day = now.day(); year = now.year();}

  //Alarm toggle button
  if(buttonpressed(button1)) {
    if(sleepcountdown != 0) {
      lcd.clear();
      resetlcdcursor();
      if(alarmhour != -1 || alarmmin != -1) {
        alarm = !alarm;
        if(alarm) lcd.print(F("Alarm ON"));
        else lcd.print(F("Alarm OFF")); }
      else lcd.print(F("No alarm set"));
      delay(2000); }
    resetsleepcounter(); }

  //Alarm set button
  if(buttonpressed(button2)) {
    if(sleepcountdown != 0) {
      lcd.clear();
      resetlcdcursor();
      lcd.print(F("Set Alarm"));
      delay(1500);
      lcd.clear();
      resetlcdcursor();
      alarmhour = settime(F("Alarm Hour"), alarmhour, 23);
      delay(750);
      alarmmin = settime(F("Alarm Minute"), alarmmin);
      lcd.clear();

      resetlcdcursor();
      lcd.print(F("Alarm Set For:"));
      lcd.setCursor(0, 1);
      lcd.print(alarmhour);
      lcd.print(":");
      printzero(alarmmin);
      lcd.print(alarmmin);
      alarm = true;
      delay(1500);
      lcd.clear(); }
    resetsleepcounter(); }

  //Time set button
  if(buttonpressed(button3)) {
    if(sleepcountdown != 0) {
      lcd.clear();
      resetlcdcursor();
      lcd.print(F("Set time"));
      delay(1500);
      completeset(); }
    resetsleepcounter(); }

  if(alarm && hour == alarmhour && minute == alarmmin && second <= 3) {playalarm(); resetsleepcounter();}

  //Code block manages printing to lcd and sleep mode
  //After a predetermined amount of time the LCD will go blank to preserve the life of it.
  if(sleepcountdown != 0){
    resetlcdcursor();
    lcd.print(hour);
    lcd.print(":");
    printzero(minute);
    lcd.print(minute);
    lcd.print(":");
    printzero(second);
    lcd.print(second);
    lcd.print("  ");

    //Prints alarm symbol if alarm is on and blanks area if it is not
    lcd.setCursor(15, 0);
    if(alarm) lcd.write(1);
    else lcd.print(" ");

    lcd.setCursor(0, 1);

    //To change the format just comment out the US code block and uncomment the EU code block

    //This code prints the US date format MM/DD/YYYY
    lcd.print(month);
    lcd.print("/");
    lcd.print(day);
    lcd.print("/");
    lcd.print(year);
    lcd.print("  ");

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

    if (sleepcountdown == 1) lcd.clear();
    sleepcountdown -= 1; }
    //Actual time counting is done on the RTC so this can be changed if you want a higher polling rate
  delay(1000); }
