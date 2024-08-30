#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

LiquidCrystal_I2C lcd(0x27, 20, 4);
byte pin_rows[ROW_NUM] = {11, 10, 9, 8};
byte pin_column[COLUMN_NUM] = {7, 6, 5, 4};
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

int sensorInterrupt = 0;
int solenoidValve = 5;
unsigned int SetPoint = 400;
String code = "";
byte indikator = 13;
int printdebit;

byte sensorInt = 0; 
byte flowsensor = 2;
int printmlt;

float konstanta = 0.00000045; //konstanta flow meter

int pulseCount;

float debit;
unsigned int flowmlt;
unsigned long totalmlt;

unsigned long oldTime;
const int relais_moteur = 3; //relay

const int SensorProximty = 13;

int volume = 0;  // Added missing variable

Servo myservo;
//Servo myservo2;

int angle = 90;
//int angle2 = 60; 

void setup()
{
    totalmlt = 0;
    pinMode(relais_moteur, OUTPUT);
    lcd.init();
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Enter Volume:");

    Serial.begin(9600);

    pinMode(indikator, OUTPUT);
    digitalWrite(indikator, HIGH); 

    pinMode(flowsensor, INPUT);
    digitalWrite(flowsensor, HIGH);

    pinMode(SensorProximty, INPUT);
    
    myservo.attach(12);

    pulseCount = 0;
    debit = 0.0;
    flowmlt = 0;
    totalmlt = 0;
    oldTime = 0;

    attachInterrupt(sensorInt, pulseCounter, FALLING);
}

void loop()
{
    int hasil = digitalRead(SensorProximty);
    if (hasil == LOW){
      char key= keypad.getKey();

    if (key)
    {
        code += key;
        lcd.setCursor(0, 1);
        lcd.print(code);
        delay(100);
    }

    if (key == 'D')
    {
        if (code.toInt() <= 1500)
        {
            volume = code.toInt();
        }
        else
        {
            lcd.clear();
            lcd.backlight();
            lcd.setCursor(0, 0);
            lcd.print("Enter Volume:");
        }
        code = "";
    }
    }

    if (totalmlt < volume)
    {
        digitalWrite(relais_moteur, HIGH); //start the water pump
        
        if((millis() - oldTime) > 1000)
        { 
        detachInterrupt(sensorInt);
        debit = ((1000.0 / (millis() - oldTime)) * pulseCount) / konstanta;
        printdebit = debit *1000;
        oldTime = millis();
        flowmlt = (debit / 60) * 1000;
        totalmlt += flowmlt;
        printmlt = totalmlt*1000;

        unsigned int frac;
        Serial.print("Debit: ");
        Serial.print(debit);
        Serial.print(" ml/s | Volume: ");
        Serial.println(totalmlt);
        Serial.print(pulseCount);

        lcd.clear();
        lcd.backlight(); 
        lcd.setCursor(0, 0); 
        lcd.print("debit:");
        lcd.print(flowmlt);  // Show the flow rate on the lcd display   
        lcd.print(" ml/s");  

        lcd.setCursor(0, 1); 
        lcd.print("volume:");           
        lcd.print(totalmlt);  // Show quantity filled   
        lcd.print(" ml");

        // Move the servo to 180 degrees when filling the drink
        angle = 30;
        myservo.write(angle);


        pulseCount = 0;

        attachInterrupt(sensorInt, pulseCounter, FALLING);
        }
    }
    else if (totalmlt>volume){
      digitalWrite(relais_moteur, LOW);
      volume = 0;
      totalmlt = 0;
        lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Minuman-mu Terisi");
      delay(3000);
      lcd.clear();

      angle = 90;
      myservo.write(angle);
    }
    else{       
        lcd.backlight();
        lcd.setCursor(0, 0);
        lcd.print("Enter Volume:");
        lcd.setCursor(0, 1);
        lcd.print(code);
    }
}

void pulseCounter()
{
    pulseCount++; //increment the pulse counter
}