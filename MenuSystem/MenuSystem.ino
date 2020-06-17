//www.elegoo.com
//2016.12.9

/*
  LiquidCrystal Library - Hello World

 Demonstrates the use of a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 Library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 Example added 9 Jul 2009
 by Tom Igoe
 Modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

unsigned long startTime = millis();

void restartTimer () {
  startTime = millis();
}

unsigned long getTimeElapsed () {
  return millis() - startTime;
}

boolean hasTimedOut () {
  return getTimeElapsed() > 200000
}

void showMenu () {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("* Frequency");
  
  lcd.setCursor(0, 1);
  lcd.print("  Duration");
}

void wait () {
  // get input from user but continue counting timeout
  if (false)  { //TurnDetected
    
  } else {
    
  }
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(14, 1);
  lcd.print("WATER BOT  ◕‿◕");
  delay(2000);

  showMenu();
}

void loop() {
  wait();
}