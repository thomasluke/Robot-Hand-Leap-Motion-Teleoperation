#include <Servo.h>
#include <LiquidCrystal_I2C.h>

int data = 0;
String data_string;
double data_conversion_factor;
double data_angle_pointer;
bool finished = true;

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
 if (Serial.available() && finished == true) // Only run when serial data is received
 {
  
    // finished == false;
    data = Serial.read();
    // data = data_string.toInt();
    // finished == true;
    // Serial.println(data_string);
    // data = Serial.read();
    // Serial.print("Received Value: ");
    // Serial.println(a);
//    data = data_string.toInt();
    // data = int(Serial.read()-'0');

    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print(data);
    // data_conversion_factor = (180/(1.9-0.2));
    // data_angle_pointer = (data) * data_conversion_factor;
// while(1)
// {
//    servo_pointer.write(atoi(data));
// }
// }
//}    delay(100);
 }
     servo_pointer.write(data);

}
