// include the library code:
#include "Wire.h" // For I2C
#include "LCD.h" // For LCD
#include "LiquidCrystal_I2C.h" // Added library*


int relayTrigger = 12;
 int relayStatus;
 char incomingMessage;

//Set the pins on the I2C chip used for LCD connections
//ADDR,EN,R/W,RS,D4,D5,D6,D7
LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7); 

void setup() {
     // Set off LCD module
   lcd.begin (16,2); // 16 x 2 LCD module
   lcd.setBacklightPin(3,POSITIVE); // BL, BL_POL
   lcd.setBacklight(HIGH);

    Serial.begin(250000);
    Serial.println("Setting Up GSM Receive Text Mode");
    Serial.println("AT\r");
    delay(1000);
    Serial.println("AT+CMGF=1\r");
    delay(1000);
    Serial.println("AT+CNMI=2,2,0,0,0\r");

}

void loop() {
 
  if(Serial.available() > 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("New SMS Received");
    delay(1000);
    incomingMessage = Serial.read();
    Serial.println(incomingMessage);
    if(incomingMessage=='0'){
      checkRelayStatus();
      if(relayStatus == LOW){
        lcd.setCursor(0,0);
        lcd.print("Gen. off already");
        lcd.setCursor(0,1);
        lcd.print("Text 1 to turnON");
           delay(1000);
          // sendSMS(1);
        //Serial.println("Generator already off. Text 1 to turn on");
      }
      else{
        digitalWrite(relayTrigger, LOW);
        lcd.setCursor(0,0);
        lcd.print("Gen switched OFF");
           delay(1000);
           //sendSMS(2);
        //Serial.println("Generator has been switched OFF successfully");
      }
    }
    else if(incomingMessage == '1'){
      checkRelayStatus();
      if(relayStatus == LOW){
        digitalWrite(relayTrigger, HIGH);
        lcd.setCursor(0,0);
        lcd.print("Gen switched ON ");
        delay(1000);
        //sendSMS(3);
       //  Serial.println("Generator has been switched ON successfully");
        
      }
      else{
        lcd.setCursor(0,0);
        lcd.print("Gen. ON already");
        lcd.setCursor(0,1);
        lcd.print("Txt 0 to turnOFF");
         delay(1000);
        // sendSMS(4);
       // Serial.println("Generator already ON. Text 0 to switch OFF");
      }
    }
    else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("INVALID MESSAGE");
      lcd.setCursor(0,1);
      lcd.print("Text 0 or 1");
       delay(1000);
      // sendSMS(5);
      //  Serial.println("INVALID SMS. Please Text 0 or 1");
    }
  }

}

int checkRelayStatus(){

  relayStatus = digitalRead(relayTrigger);
  return (relayStatus);
}
