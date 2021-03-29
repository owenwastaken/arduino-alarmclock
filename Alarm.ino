#include <LiquidCrystal.h>

const int lcd1 = 2;
const int lcd2 = 3;
const int lcd3 = 4;
const int lcd4 = 5;
const int lcd5 = 6;
const int lcd6 = 7;
const int button1 = 8;
const int button2 = 9;
const int button3 = 10;


LiquidCrystal lcd(lcd1, lcd2, lcd3, lcd4, lcd5, lcd6);
int scn, mte, hr;

void setup() {
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  lcd.begin(16, 2);
  settime(hr, "Hour", button1, button2, button3);
  settime(mte, "Minure", button1, button2, button3);
  settime(scn, "Second", button1, button2, button3);
}

void loop() {
  scn = scn + 1;
  if(scn == 60)
  {
    scn = 0;
    mte = mte + 1;
    if(mte == 60)
    {
      mte = 0;
      hr = hr + 1;
      if(hr == 24)
      {
        hr = 0;
      }
    }
  }
  lcd.setCursor(0, 0);
  lcd.print(hr);
  lcd.print(":");
  lcd.print(mte);
  lcd.print(":");
  lcd.print(scn);
  lcd.print("     ");
  delay(1000);
}


void settime(int var, String varname, int uppin, int downpin, int setpin)
{
  int setvar = 1;
  while(setvar == 1)
    {
      int upstate, downstate, setstate;
      lcd.setCursor(0, 0);
      lcd.print("Set ");
      lcd.print(varname);
      lcd.setCursor(0, 1);
      lcd.print(var);
      upstate = digitalRead(uppin);
      downstate = digitalRead(downpin);
      setstate = digitalRead(setpin);
      if(upstate == LOW)
      {
        var = var + 1;
      }
      if(downstate == LOW)
      {
        var = var - 1;
      }
      if(setstate == LOW)
      {
        setvar = 0;
      }
      delay(500);
    }
}
