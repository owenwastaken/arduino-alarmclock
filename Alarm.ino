#include <LiquidCrystal.h>

int lcd1 = 2;
int lcd2 = 3;
int lcd3 = 5;
int lcd4 = 4;
int lcd5 = 6;
int lcd6 = 7;

LiquidCrystal lcd(lcd1, lcd2, lcd3, lcd4, lcd5, lcd6);
int scn, mte, hr;

void setup() {
  lcd.begin(16, 2);
  // start init system
  scn = -1;
  mte = 0;
  hr = 0;
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
