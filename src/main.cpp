#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#define lenght 10

#define exp1 key !=NO_KEY && key != 'F' && key != 'G' && key != '#' && key !='*' && key !='U' && key != 'D' && key != 'E' && key !='E'\
  && key != 'N' && key != 'R' && key != 'L' && count < lenght
short count=0;

void(* resetFunc) (void) =0;
int SerialEvent();
int SerialEvent2(void);
int network(void);
LiquidCrystal_I2C lcd(0x27, 16, 2 );  // Change the LCD address if needed

const byte ROWS = 5; // Number of rows in the button matrix
const byte COLS = 4; // Number of columns in the button matrix

int checker=0;
char lecturer_id[lenght+1];
char exitl;
char key;
int  net =0;

int lecturerVerified,reader,studentVerified=0;
// Define the keymap
char keys[ROWS][COLS] = {
  {'F', 'G', '#', '*'},
  {'1', '2', '3', 'U'},
  {'4', '5', '6', 'D'},
  {'7', '8', '9', 'E'},
  {'L', '0', 'R', 'N'}
};

// Define the keypad connection pins
byte rowPins[ROWS] = {13,12 ,11,10,9};       // Connect to the row pinouts of the keypad
byte colPins[COLS] = {3,4,7,8};     // Connect to the column pinouts of the keypad

// Create an instance  of Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
SoftwareSerial espSerial(5, 6);
void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);
  pinMode(2,OUTPUT);
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  analogWrite(A0,((255/3.3)*3.3));
  analogWrite(A1,((255/3.3)*3.3));
  analogWrite(A2,((255/3.3)*3.3));
  delay(2000);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.blink_on();
  byte i =13;
  byte ti=11;
  lcd.setCursor(0,0);
  lcd.print("Connecting."); 
  lcd.blink_off();
  while(!network())
  {
    lcd.setCursor(ti,0);
    lcd.print(".");
    ti++;
    delay(250);
    if(ti>=17)
    {
      ti=10;
      lcd.setCursor(0,0);
      lcd.print("Connecting.     "); 
      delay(10);
    }
  } 
  lcd.setCursor(0, 0);
  lcd.print("Loading wait.");
  while(i<15)
  {
  lcd.setCursor(i, 0);
  lcd.print('.');
  i++;
  delay(500);
  }
}

void loop() {
 
 
start:
  //lcd.clear();
  lcd.setCursor(0,0);
  delay(1);
  lcd.print("   Enter ID:    ");
  
  lcd.setCursor(0,1);
  key = keypad.getKey();
  
  if (exp1)
  {
    if(count>lenght)
      {
        lcd.noCursor();
        lcd.cursor_off();
        lcd.blink_off();
        count=9;
      }else{
        if(count==-1)count=0;
        lecturer_id[count]=key;
        lcd.setCursor(count,1);
        lcd.print(lecturer_id[count]);
        count++;
      }
  }


  //Backspace
  if(key =='L' && count > -1){ 
    if (count<=0){
      count=0;
      lcd.setCursor(count,1);
      lcd.print(' ');
    }else{
      lcd.setCursor(count--,1);
      lcd.print(' ');
      lecturer_id[count]='\0';
      lcd.setCursor(count,1);
      Serial.print("Count:");
      Serial.print(count);
      Serial.print("Lecurer ID: ");
      Serial.println(lecturer_id);

    }
  }
  //Enter or Send 
  if(key =='N'&& count >0 && lecturer_id[0] !='0' )
  {
    //amount[9]='\n';
    Serial.println("am here");
   espSerial.write(lecturer_id,strlen(lecturer_id));
   Serial.println(lecturer_id);
   Serial.println(lecturer_id);
    Serial.println("Sent");
    while (count>-1)
    {
      lcd.setCursor(count--,1);
      lcd.print(' ');
      lecturer_id[count]='\0';
      lcd.setCursor(count,1);
    }
    if(count==-1)
    {
      count=0;
      lecturer_id[count]='\0';
    }
     /* lcd.setCursor(0,1);
      lcd.print(' ');
      Serial.println("SWIPE CARD");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SWIPE CARD");
      Serial.flush();
      //CardRead = SerialEvent(1);
     // Serial.println(CardRead);*/
      lecturerVerified = SerialEvent();
      Serial.println(lecturerVerified);
      if(lecturerVerified==1)
      {
        analogWrite(A2,((255/3.3)*0));
        reader = 1;
      }
      if(lecturerVerified==2){
        lcd.setCursor(0,0);
        analogWrite(A0,((255/3.3)*0));
        lcd.print(" Network error! ");
        delay(2000);
        analogWrite(A0,((255/3.3)*3.2));
        goto start;
      }
      if(lecturerVerified==7){
        lcd.setCursor(0,0);
        analogWrite(A0,((255/3.3)*0));
        lcd.print("Connection error");
        delay(2000);
        analogWrite(A0,((255/3.3)*3.2));
        goto start;
      }
      lcd.setCursor(0,0);
      lcd.print("    Scan tag    ");
     }
     
     while(reader==1)
     {
      lcd.setCursor(0,1);
      key = keypad.getKey();
      if(key)
      {
        exitl=key;
      //  lcd.print(key);
        Serial.print(key);
        reader =false;
        if(exitl=='*'){
          espSerial.write('2');
        Serial.println(key);

        }
      } 
      studentVerified = SerialEvent2();
      Serial.println(studentVerified);
      if(studentVerified==4)
      {
        lcd.setCursor(0,0);
        lcd.print("    Scan tag    ");
        digitalWrite(2,HIGH);
        analogWrite(A2,((255/3.3)*3.3));
        analogWrite(A1,((255/3.3)*0));
        delay(250);
        digitalWrite(2,LOW);
        analogWrite(A1,((255/3.3)*3.3));
        analogWrite(A2,((255/3.3)*0));
        delay(250);
      }
      else if(studentVerified==5)
      {
        lcd.setCursor(0,0);
        lcd.print("  Record taken  ");
        analogWrite(A2,((255/3.3)*0));
        delay(1000);
        analogWrite(A2,((255/3.3)*3.3));
        
      }
       if(studentVerified==9 ){
      lcd.setCursor(0,0);
      lcd.print("     Rescan     ");
      studentVerified=0;
     }
   
    if( studentVerified == 6)
      {
        lcd.setCursor(0,0);
        lcd.print("Network Err Rsc");
        studentVerified=0;
     }
     if( studentVerified == 8)
      {
        lcd.setCursor(0,0);
        lcd.print("   Card read   ");
        studentVerified=0;
     }
  }
   
}























int SerialEvent(void)
{
  while(!Serial.available())
  {
    //return 0;
  };
  int  no = (int)Serial.read();
  
  Serial.print("error");
  Serial.println(no);
 
    if(no<=0){
       //Serial.flush();
        return 0;
    }
   
    return no;
 }
int SerialEvent2(void)
{
  int  no;
  if(Serial.available()){
  no = (int)Serial.read();
      if(no>0){
       //Serial.flush();
        return no;
    }
  }
  return 0;
 }

int network(void)
{
  if(!Serial.available())
  {
    return 0;
  }
  int  no = (int)Serial.read();
  
  Serial.print("error");
  Serial.println(no);
 
    if(no<=0){
       //Serial.flush();
        return 0;
    }
   
    return no;
 }