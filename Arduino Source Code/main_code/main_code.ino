// include the library code:
#include "Wire.h" // For I2C
#include "LCD.h" // For LCD
#include "LiquidCrystal_I2C.h" // Added library*


int relayTrigger = 12;
 int relayStatus;


//GSM Module initializations
int8_t answer;
int onModulePin= 10;
char aux_string[30];
char phone_number[]="0503064768";

int checkRelayStatus();
void setupGSMReceiveText();
void power_on();
void setupGSMReceiveText();
void sendSMS(int a);

//Set the pins on the I2C chip used for LCD connections
//ADDR,EN,R/W,RS,D4,D5,D6,D7
LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7); 

void setup() {

  pinMode(relayTrigger, OUTPUT);
  
Serial.begin(9600);

   // Set off LCD module
   lcd.begin (16,2); // 16 x 2 LCD module
   lcd.setBacklightPin(3,POSITIVE); // BL, BL_POL
   lcd.setBacklight(HIGH);

   lcd.home();
  lcd.print("Setting SMS");
  lcd.setCursor(0,1);
  lcd.print("Receive Mode");
  delay(1000);
  setupGSMReceiveText();
  lcd.clear();
  lcd.home();
  lcd.print("SUCCESS!!");
  delay(2000);

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
           sendSMS(1);
        //Serial.println("Generator already off. Text 1 to turn on");
      }
      else{
        digitalWrite(relayTrigger, LOW);
        lcd.setCursor(0,0);
        lcd.print("Gen switched OFF");
           delay(1000);
           sendSMS(2);
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
        sendSMS(3);
       //  Serial.println("Generator has been switched ON successfully");
        
      }
      else{
        lcd.setCursor(0,0);
        lcd.print("Gen. ON already");
        lcd.setCursor(0,1);
        lcd.print("Txt 0 to turnOFF");
         delay(1000);
         sendSMS(4);
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
       sendSMS(5);
      //  Serial.println("INVALID SMS. Please Text 0 or 1");
    }
  }
}


int checkRelayStatus(){

  relayStatus = digitalRead(relayTrigger);
  return (relayStatus);
}




/*======================================================
THIS FUNCTION SENDS DIFFERENT SMS MESSAGES TO THE 
AUTHORIZED PHONE NUMBER DEPENDING ON THE INNPUT IT RECEIVES
========================================================*/


//Accepts an integer parameter to determine what message should be sent.

void sendSMS(int a){
   //sets the PIN code

 
    sendATcommand("AT+CPIN=****", "OK", 2000);
    
    delay(3000);
    
    Serial.println("Connecting to the network...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GSM Connecting..");
    delay(500);

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.print("Setting SMS mode...");
    lcd.setCursor(0, 1);
    lcd.print("Setting SMS mode");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
     lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sending Info");
    delay(500);
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    { 
          switch (a) {
            case 1:
              Serial.println("Generator already off. Text 1 to turn on");
              break;
            case 2:
              Serial.println("Generator has been switched OFF successfully");
              break;
            case 3:
              Serial.println("Generator has been switched ON successfully");
              break;
            case 4:
              Serial.println("Generator already ON. Text 0 to switch OFF");
              break;
            default: 
              Serial.println("INVALID MESSAGE. Please Text 0 to turn off or 1 to turn ON");
              break;
          }

        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
         lcd.clear();
            Serial.print("Sent ");  
             lcd.setCursor(0,0);
            lcd.print("SMS Sent ");
           lcd.setCursor(0,1);
            lcd.print("Successfully");
             delay(1000); 
        }
        else
        {
            Serial.print("error ");
            lcd.setCursor(0,0);
            lcd.print("Error Sending ");
           lcd.setCursor(0,1);
            lcd.print("Text Message");
             delay(2000);
        }
    }
    else
    {
        Serial.print("error ");
        lcd.clear();
          lcd.setCursor(0,0);
            lcd.print("Error Setting ");
           lcd.setCursor(0,1);
            lcd.print("SMS Mode");
             delay(2000);

             lcd.clear();
          lcd.setCursor(0,0);
            lcd.print("Retrying to ");
           lcd.setCursor(0,1);
            lcd.print("Set SMS Mode");
             delay(2000);
        Serial.println(answer, DEC);
    }

}


/*======================================
THIS FUNCTION STARTS ON THE GSM MODULE
======================================*/
void power_on(){

    uint8_t answer=0;
    
    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Setting up GSM..");
        
    
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);
    
        // waits for an answer from the module
        while(answer == 0){     // Send AT every two seconds and wait for the answer
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }
    
}


/*==============================
THIS FUNCTION SENDS AT COMMANDS
===============================*/
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialice the string
    
    delay(100);
    
    while( Serial.available() > 0) Serial.read();    // Clean the input buffer
    
    Serial.println(ATcommand);    // Send the AT command 


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(Serial.available() != 0){    
            response[x] = Serial.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
                     
            }
        }
    // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}


/*=============================================================
THIS FUNCTION CONFIGURES THE GSM MODULE TO RECEIVE TEXT MESSAGES
================================================================*/
void setupGSMReceiveText(){
    Serial.begin(57600);
    Serial.println("Setting Up GSM Receive Text Mode");
    Serial.println("AT\r");
    delay(1000);
    Serial.println("AT+CMGF=1\r");
    delay(1000);
    Serial.println("AT+CNMI=2,2,0,0,0\r");
    return;
}
