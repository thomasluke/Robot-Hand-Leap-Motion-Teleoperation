#include <Servo.h>
#include <LiquidCrystal_I2C.h>
//#include <ArduinoJson.h>

int data;
char buffer[7];

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

// Setup the I2C LCD display
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Attach servo motors to their respective Arduino PWM pins.
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
  // Only read serial data after the newline character is received. This ensures the the same bytes are read every loop in the same sequence
  if (Serial.available() > 0 && Serial.read() == '\n') // Only run when serial data is received
  {

    // Read bytes (5 in this case) until the end of the buffer array (i.e. when the newline character is reached)
    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    // Multiply back values received to which were divided in Python to keep them within the require byte range of -128<=value<=128.
    servo_thumb_angle = buffer[0] * 3;
    servo_pointer_angle = buffer[1] * 2;
    servo_index_angle = buffer[2] * 2;
    servo_ring_angle = buffer[3] * 2;
    servo_pinky_angle = buffer[4] * 2;
    servo_wrist_angle = buffer[5] * 2;

    // Rotate servo motors to the angles received through serial from Python
    servo_thumb.write(servo_thumb_angle);
    servo_pointer.write(servo_pointer_angle);
    servo_index.write(servo_index_angle);
    servo_ring.write(servo_ring_angle);
    servo_pinky.write(servo_pinky_angle);
    servo_wrist.write(servo_wrist_angle);

    // Values sent to I2C LCD used for debugging (As cannot easily display serial values received by Arduino for debugging, as python code is occupying COM3)

    // lcd.clear();

    // lcd.setCursor(0, 1);
    // lcd.print(servo_thumb_angle);

    // lcd.setCursor(0, 0);
    // lcd.print(servo_pointer_angle);

    // lcd.setCursor(4, 0);
    // lcd.print(servo_index_angle);

    // lcd.setCursor(8, 0);
    // lcd.print(servo_ring_angle);

    // lcd.setCursor(12, 0);
    // lcd.print(servo_pinky_angle);

    // lcd.setCursor(12, 1);
    // lcd.print(Serial.available()); // Number of bytes in the serial buffer (64 max - 0 -> 63)

    // Serial.flush();
  }

}
