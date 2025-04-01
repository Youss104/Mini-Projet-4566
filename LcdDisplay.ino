#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//SDA->21,SCL->22 
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int count = 0;

void setup()
{
  lcd.init();// initialize the lcd 
  lcd.backlight(); // Turns on the LCD backlight.  // Print a message to the LCD.
  delay(2000);
}

void lcdWrite(String word){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(word);
}

void loop()
{
  String word1="Test";
  lcdWrite(word1);
  delay(1000);
}