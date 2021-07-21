#include <Servo.h>
#include <LiquidCrystal_I2C.h>
//#include <ArduinoJson.h>

int data;
int test_data;
String data_string;
double data_conversion_factor;
double data_angle_pointer;
bool finished = true;
char buffer[6];
char test_buffer[2];
int servo_angle = 0;

int servo_thumb_angle;
int servo_pointer_angle;
int servo_index_angle;
int servo_ring_angle;
int servo_pinky_angle;
int servo_wrist_angle;

Servo servo_thumb;
Servo servo_pointer;
Servo servo_index;
Servo servo_ring;
Servo servo_pinky;
Servo servo_wrist;


LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  servo_thumb.attach(3);
  servo_pointer.attach(5);
  servo_index.attach(6);
  servo_ring.attach(9);
  servo_pinky.attach(10);
  servo_wrist.attach(11);

  lcd.begin();
  lcd.backlight();
}

void loop()
{

  // put your main code here, to run repeatedly:
  // delay(100);
  if (Serial.available() > 0 && Serial.read() == '\n') // Only run when serial data is received
  {

    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    servo_thumb_angle = buffer[0] * 3;
    servo_pointer_angle = buffer[1] * 2 ;
    servo_index_angle = buffer[2] * 2;
    servo_ring_angle = buffer[3] * 2;
    servo_pinky_angle = buffer[4] * 2;

    //    lcd.clear();
    //    lcd.setCursor(0, 1);
    //    lcd.print(servo_thumb_angle);
    //    lcd.setCursor(0, 0);
    //    lcd.print(servo_pointer_angle);
    //    lcd.setCursor(4, 0);
    //    lcd.print(servo_index_angle);
    //    lcd.setCursor(8, 0);
    //    lcd.print(servo_ring_angle);
    //    lcd.setCursor(12, 0);
    //    lcd.print(servo_pinky_angle);
    //
    //    lcd.setCursor(12, 1);
    //    lcd.print(Serial.available()); // Number of bytes in the serial buffer (64 max - 0 -> 63)

    //    Serial.flush();
  }

  servo_thumb.write(servo_thumb_angle);
  servo_pointer.write(servo_pointer_angle);
  servo_index.write(servo_index_angle);
  servo_ring.write(servo_ring_angle);
  servo_pinky.write(servo_pinky_angle);

}
