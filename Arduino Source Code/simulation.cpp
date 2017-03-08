/*THIS PROGRAM CHECKS TO SEE IF AN SMS HAS BEEN RECEIVED
IF IT HAS, THE RELAY IS ENERGIZED WHICH CAUSES THE LAMP TO TURN ON
AFTERWARDS AN SMS IS SENT TO AUTHORIZED PHONE NUMBERS AND A LOCAL 
LCD DISPLAYS THE STATUS OFF THE RELAY

By: Isaac Sesi 
Date: 22nd February, 2017
*/
#include "Wire.h" // For I2C
#include "LCD.h" // For LCD
#include "LiquidCrystal_I2C.h" // Added library*
#include <SoftwareSerial.h>
 //SoftwareSerial mySerial(10, 11);

int8_t answer;
int onModulePin= 2;
char aux_string[30];
int relayTrigger = 12;

char phone_number1[]="0503064768";
char phone_number2[]="0501359999";
char phone_number3[]="0500011505";

 void power_on();
//Set the pins on the I2C chip used for LCD connections
//ADDR,EN,R/W,RS,D4,D5,D6,D7
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the default I2C bus address of the backpack-see article

    

void setup(){


    pinMode(onModulePin, OUTPUT);

   lcd.begin (16,2); // 16 x 2 LCD module
   lcd.setBacklightPin(3,POSITIVE); // BL, BL_POL
   lcd.setBacklight(HIGH);
    Serial.begin(57600);  
    Serial.begin(57600);  
        
    Serial.println("Starting...");
    power_on();
    
    delay(3000);
    
    //sets the PIN code
    sendATcommand("AT+CPIN=****", "OK", 2000);
    
    delay(3000);
    
    Serial.println("Connecting to the network...");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.print("Setting SMS mode...");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    {
        Serial.println("Test-Arduino-Hello World");
        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
            Serial.print("Sent ");    
        }
        else
        {
            Serial.print("error ");
        }
    }
    else
    {
        Serial.print("error ");
        Serial.println(answer, DEC);
    }

}


void loop(){

}





//GSM FUNCTIONS

void sendSMS(int a, float b){
   //sets the PIN code

  // float *gpsInfo;
 //  gpsInfo
    sendATcommand("AT+CPIN=****", "OK", 2000);
    
    delay(3000);
    
    Serial.println("Connecting to the network...");
    lcd.setCursor(0, 0);
    lcd.println("GSM Connecting..");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.print("Setting SMS mode...");
    lcd.setCursor(0, 1);
    lcd.print("Setting SMS mode");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");

    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer == 1)
    { 


         lcd.clear();
         if(a == 1){
         Serial.println("Significant Level Change Detected");
        Serial.print("New Level is "); Serial.print(b); Serial.print(" ml");         
         }
         else if(a == 2){
           Serial.println("Reply to Query");
        Serial.print("New Level is "); Serial.print(b); Serial.print(" ml"); 
         }

         else if(a == 3){
           Serial.println("Status Update:");
        Serial.print("Current Level is "); Serial.print(b); Serial.print(" ml"); 
         }
        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
            Serial.print("Sent ");    
        }
        else
        {
            Serial.print("error ");
        }
    }
    else
    {
        Serial.print("error ");
        Serial.println(answer, DEC);
    }

}


void power_on(){

    uint8_t answer=0;
    
    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);
    
        // waits for an answer from the module
        while(answer == 0){     // Send AT every two seconds and wait for the answer
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }
    
}

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
