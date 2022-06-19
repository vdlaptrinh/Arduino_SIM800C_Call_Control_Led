//Điều khiển LED bằng cách thực hiện cuộc gọi
#define onModulePin 9
#define buzzer 10
#define ledPin 11

int8_t answer;

char aux_string[30];

int flag = 0;
char number[20];
char realnumber[10];
char mynumber[10];
int a=0;
int b=0;
int c=0;
//Your phone number
char phone_number[]="0919005015";
  
void power_on();
void power_onSMS();
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout);
void sendSMS();

void setup(){
    mynumber[0]='0';
    mynumber[1]='9';
    mynumber[2]='1';
    mynumber[3]='9';
    mynumber[4]='0';
    mynumber[5]='0';
    mynumber[6]='5';
    mynumber[7]='0';
    mynumber[8]='1';
    mynumber[9]='5';
    
    pinMode(onModulePin, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(ledPin, OUTPUT);
    
    digitalWrite(buzzer,LOW);
    digitalWrite(ledPin,LOW);
    
    Serial.begin(9600);
    power_on();
    power_onSMS();
    delay(5000);
    delay(3000);  
    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 1000) || sendATcommand("AT+CREG?", "+CREG: 0,5", 1000)) == 0 );

    sendATcommand("AT+CLIP=1", "OK", 1000); 
    
    //delay(1000);
    digitalWrite(buzzer,HIGH);  delay(200);
    digitalWrite(buzzer,LOW);   delay(200);
    digitalWrite(buzzer,HIGH);  delay(200);
    digitalWrite(buzzer,LOW);   delay(200);

    
    while(Serial.available() != 0)
    {
      Serial.read();  
    }
}
void loop(){
    
    answer = sendATcommand("", "+CLIP", 1000);
    //Detect incomming call
    if (answer == 1)
    {
        Serial.println("Incoming call");          
          
          if ( flag == 0){
            
              for (int i=0; i<19; i++){
               // read the incoming byte:
               while (Serial.available() == 0)
               {                 
                 delay (50);
               }
               //Stores phone number
               number[i] = Serial.read();  
               
              }
              Serial.flush();
              flag = 1; 
          }          
          //Stores phone calling number
          for (int i=0; i<=14; i++){
            if(number[i]== '"'){
              i++;
              realnumber[0]=number[i];
              i++;
              realnumber[1]=number[i];
              i++;
              realnumber[2]=number[i];
              i++;
              realnumber[3]=number[i];
              i++;
              realnumber[4]=number[i];
              i++;
              realnumber[5]=number[i];
              i++;
              realnumber[6]=number[i];
              i++;
              realnumber[7]=number[i];
              i++;
              realnumber[8]=number[i];
              i++;
              realnumber[9]=number[i];
              break;
            }            
    }
    //Check phone number
    for (int i=0;i<10;i++){ 
      if (realnumber[i] == mynumber[i]){
          a++;       
          if( a==10){  
             Serial.println("Correct number");
             sendATcommand("ATH", "OK", 1000);
               if(b==0){
                  b=1;
                }else{                  
                  b=0;
                  break;
                }
                }else{             
                  Serial.println("Wrong number");
                  break;
                 }
      }          
     a=0;
     answer=0;
     flag = 0;     
    }
    //Send SMS once and position to HTTP                   
    if (b==1){
      digitalWrite(ledPin,HIGH);
    }
    else digitalWrite(ledPin,LOW);
  }
}

void power_on(){

    uint8_t answer=0;

    digitalWrite(onModulePin,HIGH);
    delay(3000);
    digitalWrite(onModulePin,LOW);

    while(answer == 0){     // Send AT every two seconds and wait for the answer
        answer = sendATcommand("AT", "OK", 2000);    
    }
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialice the string
    
    delay(100);
    
    while( Serial.available() > 0) Serial.read();    // Clean the input buffer
    
    if (ATcommand[0] != '\0')
    {
        Serial.println(ATcommand);    // Send the AT command 
    }


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(Serial.available() != 0){    // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = Serial.read();
            //Serial.print(response[x]);
            x++;
            if (strstr(response, expected_answer) != NULL)    // check if the desired answer (OK) is in the response of the module
            {
                answer = 1;
            }
        }
    }while((answer == 0) && ((millis() - previous) < timeout));    // Waits for the asnwer with time out

    return answer;
}

void power_onSMS(){

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

void sendSMS(){
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
        Serial.print("Help me! I've been stolen. Find me in:");
        //Serial.print("Latitude: ");
              //int i = 0;
              //while(latitude[i]!=0){
              //Serial.print(latitude[i]);
              //i++;
              //}
              //Serial.print(" / Longitude: ");
              //i = 0;
              //while(longitude[i]!=0){
              //Serial.print(longitude[i]);
              //i++;
              //}
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
