#include <Servo.h>
#include <LiquidCrystal_I2C.h>
//#include <ArduinoJson.h>

// int mode_switcher = 0;
int data;
int data2;
int mode;
char buffer[8];
char mode_buffer[2];
bool lock = false;
double leap_data_confidence;
double glove_data_weight;

unsigned long LocalStartTime = millis();
unsigned long SystemStartTime = millis();
unsigned long CurrentTime;
unsigned long ElapsedTime;

int servo_thumb_angle;
int servo_pointer_angle;
int servo_middle_angle;
int servo_ring_angle;
int servo_pinky_angle;
int servo_wrist_angle;

int weighted_servo_thumb_angle;
int weighted_servo_pointer_angle;
int weighted_servo_middle_angle;
int weighted_servo_ring_angle;
int weighted_servo_pinky_angle;
int weighted_servo_wrist_angle;

Servo servo_thumb;
Servo servo_pointer;
Servo servo_middle;
Servo servo_ring;
Servo servo_pinky;
Servo servo_wrist;

//define the flex sensor input pins
int flex_5 = A5;
int flex_4 = A4;
int flex_3 = A3;
int flex_2 = A2;
int flex_1 = A1;

//define variables for flex sensor values
int flex_5_val;
int flex_4_val;
int flex_3_val;
int flex_2_val;
int flex_1_val;

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
  servo_middle.attach(6);
  servo_ring.attach(9);
  servo_pinky.attach(10);
  servo_wrist.attach(11);

  lcd.begin();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: -");
}

void loop()
{
  LocalStartTime = millis();

  // if (mode_switcher < 500)
  // {
  //   mode_switcher++;
  //   lcd.setCursor(0, 1);
  //   lcd.print("      ");
  //   lcd.setCursor(0, 1);
  //   lcd.print(mode_switcher);
  // }
  // put your main code here, to run repeatedly:
  // delay(100);
  // Serial.setTimeout(50);

  //  if (Serial.available() > 0 && Serial.read() == '\n' && lock == false) // Only run when serial data is received
  if (Serial.read() == '\n' && lock == false) // Only run when serial data is received
  {
    // Read bytes (5 in this case) until the end of the buffer array (i.e. when the newline character is reached)
    data2 = Serial.readBytesUntil('\n', mode_buffer, sizeof(mode_buffer) - 1);
    mode = mode_buffer[0];
    lock = true;

    lcd.setCursor(6, 0);
    lcd.print("          ");

    switch (mode)
    {
    case 1:
      lcd.setCursor(6, 0);
      lcd.print("Auto");
      break;
    case 2:
      lcd.setCursor(6, 0);
      lcd.print("Leap");
      break;
    case 3:
      lcd.setCursor(6, 0);
      lcd.print("Glove");
      break;
    case 4:
      lcd.setCursor(6, 0);
      lcd.print("Key");
      break;
    case 5:
      lcd.setCursor(6, 0);
      lcd.print("Combined");
      break;
    default:
      lcd.setCursor(6, 0);
      lcd.print("Error");
      break;
    }
    Serial.flush();
  }

  // Only read data when in mode 1 (Automatic control selection) or mode 2 (Leap Motion Control)
  // Only read serial data when availble and after the newline character is received. This ensures the the same bytes are read every loop in the same sequence
  //  if ((mode == 1 || mode == 2) && Serial.available() > 0 && Serial.read() == '\n' && lock == true) // Only run when serial data is received
  // if ((mode == 1 || mode == 2) && Serial.read() == '\n' && lock == true) // Only run when serial data is received
  if (mode == 2 && Serial.read() == '\n' && lock == true) // Only run when serial data is received
  {
    // mode_switcher = 0;
    // memset(buffer, 0, sizeof(buffer));
    // Read bytes (5 in this case) until the end of the buffer array (i.e. when the newline character is reached)
    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    // Multiply back values received to which were divided in Python to keep them within the require byte range of -128<=value<=128.
    servo_thumb_angle = buffer[0] * 3;
    servo_pointer_angle = buffer[1] * 2;
    servo_middle_angle = buffer[2] * 2;
    servo_ring_angle = buffer[3] * 2;
    servo_pinky_angle = buffer[4] * 2;
    servo_wrist_angle = buffer[5] * 2;

    // Rotate servo motors to the angles received through serial from Python
    servo_thumb.write(servo_thumb_angle);
    servo_pointer.write(servo_pointer_angle);
    servo_middle.write(servo_middle_angle);
    servo_ring.write(servo_ring_angle);
    servo_pinky.write(servo_pinky_angle);
    servo_wrist.write(servo_wrist_angle);

    CurrentTime = millis();
    ElapsedTime = CurrentTime - SystemStartTime;
    Serial.print('\n');
    Serial.print(ElapsedTime);
    SystemStartTime = millis();
    
//    Serial.print('\n');
    //    Serial.write(ElapsedTime/1000);

    lcd.print("        ");
    lcd.setCursor(7, 1);
    //    lcd.print("leap");
    lcd.print(Serial.available());
    // lcd.print(string(ElapsedTime));


    // Values sent to I2C LCD used for debugging (As cannot easily display serial values received by Arduino for debugging, as python code is occupying COM3)

    // lcd.clear();

    // lcd.setCursor(0, 1);
    // lcd.print(servo_thumb_angle);

    // lcd.setCursor(0, 0);
    // lcd.print(servo_pointer_angle);

    // lcd.setCursor(4, 0);
    // lcd.print(servo_middle_angle);

    // lcd.setCursor(8, 0);
    // lcd.print(servo_ring_angle);

    // lcd.setCursor(12, 0);
    // lcd.print(servo_pinky_angle);

    // lcd.setCursor(12, 1);
    // lcd.print(Serial.available()); // Number of bytes in the serial buffer (64 max - 0 -> 63)

    //   Serial.flush();
  }

  // Only read data when in mode 1 (Automatic control selection) or mode 2 (Leap Motion Control)
  // Only read serial data when availble and after the newline character is received. This ensures the the same bytes are read every loop in the same sequence
  // else if (mode == 5 && Serial.available() > 0 && Serial.read() == '\n' && lock == true) // Only run when serial data is received
  else if (mode == 5 && Serial.read() == '\n' && lock == true) // Only run when serial data is received
  {
    // mode_switcher = 0;
    // Read bytes (5 in this case) until the end of the buffer array (i.e. when the newline character is reached)
    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    // Multiply back values received to which were divided in Python to keep them within the require byte range of -128<=value<=128.
    servo_thumb_angle = buffer[0] * 3;
    servo_pointer_angle = buffer[1] * 2;
    servo_middle_angle = buffer[2] * 2;
    servo_ring_angle = buffer[3] * 2;
    servo_pinky_angle = buffer[4] * 2;
    servo_wrist_angle = buffer[5] * 2;

    leap_data_confidence = buffer[6] / 100;

    flex_5_val = analogRead(flex_5);
    flex_4_val = analogRead(flex_4);
    flex_3_val = analogRead(flex_3);
    flex_2_val = analogRead(flex_2);
    flex_1_val = analogRead(flex_1);

    flex_5_val = map(flex_5_val, 630, 730, 80, 20);
    flex_4_val = map(flex_4_val, 520, 710, 70, 175);
    flex_3_val = map(flex_3_val, 510, 680, 140, 10);
    flex_2_val = map(flex_2_val, 580, 715, 90, 175);
    flex_1_val = map(flex_1_val, 550, 700, 90, 175);

    // Weighted average combining Leap Motion Controller and Glove Control data
    weighted_servo_thumb_angle = ((leap_data_confidence * servo_thumb_angle) + (glove_data_weight * flex_1_val) / (servo_thumb_angle + flex_1_val));
    weighted_servo_pointer_angle = ((leap_data_confidence * servo_pointer_angle) + (glove_data_weight * flex_2_val) / (servo_pointer_angle + flex_2_val));
    weighted_servo_middle_angle = ((leap_data_confidence * servo_middle_angle) + (glove_data_weight * flex_3_val) / (servo_middle_angle + flex_3_val));
    weighted_servo_ring_angle = ((leap_data_confidence * servo_ring_angle) + (glove_data_weight * flex_4_val) / (servo_ring_angle + flex_4_val));
    weighted_servo_pinky_angle = ((leap_data_confidence * servo_pinky_angle) + (glove_data_weight * flex_5_val) / (servo_pinky_angle + flex_5_val));
    // weighted_servo_wrist_angle = ((leap_data_confidence*servo_wrist_angle)+(glove_data_weight*flex_2_val)/(servo_wrist_angle+flex_6_val));

    // Rotate servo motors to the angles received through serial from Python
    servo_thumb.write(weighted_servo_thumb_angle);
    servo_pointer.write(weighted_servo_pointer_angle);
    servo_middle.write(weighted_servo_middle_angle);
    servo_ring.write(weighted_servo_ring_angle);
    servo_pinky.write(weighted_servo_pinky_angle);
    servo_wrist.write(weighted_servo_wrist_angle);

    CurrentTime = millis();
    ElapsedTime = CurrentTime - SystemStartTime;
    Serial.print('\n');
    Serial.print(ElapsedTime);
    SystemStartTime = millis();

    lcd.print("        ");
    lcd.setCursor(7, 1);
    lcd.print("combined");
  }

  // True if in mode 1 (Automatic control selection) or mode 3 (Glove control)
  // True if serial data is not availble
  // else if ((mode == 1 || mode == 3) && mode_switcher >= 500)
  else if (mode == 3)
  {
    // Put glove control code here
    // Can use LCD message to debug whether these mode if statements are working
    // Main check will be to see whether this "else if" is triggered at the correct times in mode 1 (Automatic control).
    // Also need to make sure this mode doesnt trigger inbetween serial signals! (maybe/probably remove if too buggy?!?!)

    flex_5_val = analogRead(flex_5);
    flex_5_val = map(flex_5_val, 630, 730, 80, 20);

    flex_4_val = analogRead(flex_4);
    flex_4_val = map(flex_4_val, 520, 710, 70, 175);

    flex_3_val = analogRead(flex_3);
    flex_3_val = map(flex_3_val, 510, 680, 140, 10);

    flex_2_val = analogRead(flex_2);
    flex_2_val = map(flex_2_val, 580, 715, 90, 175);

    flex_1_val = analogRead(flex_1);
    flex_1_val = map(flex_1_val, 550, 700, 90, 175);

    servo_thumb.write(flex_1_val);   //A1
    servo_pointer.write(flex_2_val); //A2
    servo_middle.write(flex_3_val);  //A3
    servo_ring.write(flex_4_val);    //A4
    servo_pinky.write(flex_5_val);   //A5

    CurrentTime = millis();
    ElapsedTime = CurrentTime - SystemStartTime;
    Serial.print('\n');
    Serial.print(ElapsedTime);
    SystemStartTime = millis();
//    delay(100);

//    //    mode_switcher++;
//    lcd.setCursor(0, 1);
//    lcd.print("                ");
//    lcd.setCursor(0, 1);
//    lcd.print("glovemode");
  }

  else if (mode == 4)
  {
    // Read bytes (5 in this case) until the end of the buffer array (i.e. when the newline character is reached)
    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    // Multiply back values received to which were divided in Python to keep them within the require byte range of -128<=value<=128.
    servo_thumb_angle = buffer[0] * 2;
    servo_pointer_angle = buffer[1] * 2;
    servo_middle_angle = buffer[2] * 2;
    servo_ring_angle = buffer[3] * 2;
    servo_pinky_angle = buffer[4] * 2;
    servo_wrist_angle = buffer[5] * 2;

    // Rotate servo motors to the angles received through serial from Python
    servo_thumb.write(servo_thumb_angle);
    servo_pointer.write(servo_pointer_angle);
    servo_middle.write(servo_middle_angle);
    servo_ring.write(servo_ring_angle);
    servo_pinky.write(servo_pinky_angle);
    servo_wrist.write(servo_wrist_angle);

    CurrentTime = millis();
    ElapsedTime = CurrentTime - SystemStartTime;
    Serial.print('\n');
    Serial.print(ElapsedTime);
    SystemStartTime = millis();
    
  }
}
