#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
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
