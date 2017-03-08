// include the library code:
#include <LiquidCrystal.h>

int relayTrigger = 13;
 int relayStatus;
int checkRelayStatus();

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {

  pinMode(relayTrigger, OUTPUT);
  
Serial.begin(9600);
lcd.begin(16, 2);
lcd.setCursor(0,0);
lcd.println("Setting SMS");
lcd.setCursor(0,1);
lcd.print("Receive Mode");
delay(1000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Generator is Off... ");
  lcd.setCursor(0,1);
  lcd.print("Text 1 to turnON");
}



void loop() {

  char incomingMessage;
  if(Serial.available() > 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("New SMS Received");
    delay(1000);
    incomingMessage = Serial.read();
    if(incomingMessage=='0'){
      checkRelayStatus();
      if(relayStatus == LOW){
        lcd.setCursor(0,0);
        lcd.print("Gen. off already");
        lcd.setCursor(0,1);
        lcd.print("Text 1 to turnON");
           delay(1000);
        Serial.println("Generator already off. Text 1 to turn on");
      }
      else{
        digitalWrite(relayTrigger, LOW);
        lcd.setCursor(0,0);
        lcd.print("Gen switched OFF");
           delay(1000);
        Serial.println("Generator has been switched OFF successfully");
      }
    }
    else if(incomingMessage == '1'){
      checkRelayStatus();
      if(relayStatus == LOW){
        digitalWrite(relayTrigger, HIGH);
        lcd.setCursor(0,0);
        lcd.print("Gen switched ON ");
        delay(1000);
         Serial.println("Generator has been switched ON successfully");
        
      }
      else{
        lcd.setCursor(0,0);
        lcd.print("Gen. ON already");
        lcd.setCursor(0,1);
        lcd.print("Txt 0 to turnOFF");
         delay(1000);
        Serial.println("Generator already ON. Text 1 to switch OFF");
      }
    }
    else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("INVALID MESSAGE");
      lcd.setCursor(0,1);
      lcd.print("Text 0 or 1");
       delay(1000);
        Serial.println("INVALID SMS. Please Text 0 or 1");
    }
  }
}


int checkRelayStatus(){

  relayStatus = digitalRead(relayTrigger);
  return (relayStatus);
}


