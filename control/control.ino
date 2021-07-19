
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
//#include <ArduinoJson.h>

int data;
String data_string;
double data_conversion_factor;
double data_angle_pointer;
bool finished = true;
char buffer[6];
int servo_angle = 0;

Servo servo_thumb;
Servo servo_pointer;
Servo servo_index;
Servo servo_ring;
Servo servo_pinky;

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  servo_pointer.attach(3);

  lcd.begin();
  lcd.backlight();
}

void loop()
{

  //    lcd.setCursor(1,0);
  //    lcd.print("test");
  // put your main code here, to run repeatedly:
//  delay(100);
Serial.flush();
  if (Serial.available()>5) // Only run when serial data is received
  {   
    
    // finished == false;
    // data = Serial.read();
    
    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
//    data = Serial.read();
    // data = [ord(data) for data in frame]
    
    // buffer[5] = data;
    // Serial.println(i);
    // data = data_string.toInt();
    // finished == true;
    // Serial.println(data_string);
    // data = Serial.read();
    // Serial.print("Received Value: ");
    // Serial.println(a);
    //    data = data_string.toInt();
    // data = int(Serial.read()-'0');

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(abs(buffer[2]-'0'));
    lcd.setCursor(1, 1);
//      Serial.flush();
    lcd.print(abs(buffer[1]-'0'));
    lcd.setCursor(5, 1);
    lcd.print(Serial.available());
    // lcd.setCursor(1, 1);
    // lcd.print(Serial.available());
    // lcd.print(abs(buffer[1]-'0'));
    // data_conversion_factor = (180/(1.9-0.2));
    // data_angle_pointer = (data) * data_conversion_factor;
    // while(1)
    // {
    //    servo_pointer.write(atoi(data));
    // }
    // }
    //}    delay(100);
    servo_angle = abs(buffer[1]-'0');
  }
  servo_pointer.write(servo_angle);
}
