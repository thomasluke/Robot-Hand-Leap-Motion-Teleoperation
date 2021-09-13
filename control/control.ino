#include <Servo.h>
#include <LiquidCrystal_I2C.h>
//#include <ArduinoJson.h>

// int mode_switcher = 0;
int data;
int data2;
int data3;
int mode;
char buffer[8];
char buffer1[2];
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

int servo_thumb_angle_serial;
int servo_pointer_angle_serial;
int servo_middle_angle_serial;
int servo_ring_angle_serial;
int servo_pinky_angle_serial;
int servo_wrist_angle_serial;

int servo_thumb_angle_interp;
int servo_pointer_angle_interp;
int servo_middle_angle_interp;
int servo_ring_angle_interp;
int servo_pinky_angle_interp;
int servo_wrist_angle_interp;

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

int flex_5_val_serial;
int flex_4_val_serial;
int flex_3_val_serial;
int flex_2_val_serial;
int flex_1_val_serial;

int flex_5_val_interp;
int flex_4_val_interp;
int flex_3_val_interp;
int flex_2_val_interp;
int flex_1_val_interp;

int count = 0;
int count2 = 0;

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

void ModeSelection()
{

  //  if (Serial.available() > 0 && Serial.read() == '\n' && lock == false) // Only run when serial data is received
  if (Serial.read() == '\n') // Only run when serial data is received
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
        lcd.print("Combined 1");
        break;
      case 6:
        lcd.setCursor(6, 0);
        lcd.print("Combined 2");
        break;
      default:
        lcd.setCursor(6, 0);
        lcd.print("Error");
        break;
    }
    Serial.flush();
  }
}

void Mode2()
{
  // Leap Motion Control Mode
//  int test = Serial.read();
  if (Serial.read() == 0) // Only run when serial data is received
  {

    // Read bytes (7 in this case) until the end of the buffer array (i.e. when the newline character is reached)
    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    // Multiply back values received to which were divided in Python to keep them within the require byte range of -128<=value<=128.
    servo_thumb_angle = buffer[0] * 4;
    servo_pointer_angle = buffer[1] * 4;
    servo_middle_angle = buffer[2] * 4;
    servo_ring_angle = buffer[3] * 4;
    servo_pinky_angle = buffer[4] * 4;
    servo_wrist_angle = buffer[5] * 4;

    // Rotate servo motors to the angles received through serial from Python
    servo_thumb.write(servo_thumb_angle);
    servo_pointer.write(servo_pointer_angle);
    servo_middle.write(servo_middle_angle);
    servo_ring.write(servo_ring_angle);
    servo_pinky.write(servo_pinky_angle);
    servo_wrist.write(servo_wrist_angle);

    LatencyMeasure();
  }
//  lcd.print("   ");
//  lcd.setCursor(11, 1);
//  // lcd.print("combined");
//  lcd.print(test);
}

void Mode3()
{
  // Glove Control Mode

  flex_5_val = analogRead(flex_5);
  flex_5_val = map(flex_5_val, 200, 450, 0, 180);

  flex_4_val = analogRead(flex_4);
  flex_4_val = map(flex_4_val, 200, 450, 0, 180);

  flex_3_val = analogRead(flex_3);
  flex_3_val = map(flex_3_val, 200, 450, 0, 180);

  flex_2_val = analogRead(flex_2);
  flex_2_val = map(flex_2_val, 180, 350, 0, 180);

  flex_1_val = analogRead(flex_1);
  flex_1_val = map(flex_1_val, 200, 450, 0, 180);

  servo_thumb.write(flex_1_val);   //A1
  servo_pointer.write(flex_2_val); //A2
  servo_middle.write(flex_3_val);  //A3
  servo_ring.write(flex_4_val);    //A4
  servo_pinky.write(flex_5_val);   //A5

  //  LatencyMeasure();

//  lcd.clear();
//
//  lcd.setCursor(0, 1);
//  //   lcd.print("                ");
//  //   lcd.setCursor(0, 1);
//  //  lcd.print("glovemode");
//  lcd.print(flex_3_val);
}

void Mode4()
{
  // Keyboard Control Mode

  // Read bytes (5 in this case) until the end of the buffer array (i.e. when the newline character is reached)
  data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

  // Multiply back values received to which were divided in Python to keep them within the require byte range of -128<=value<=128.
  servo_thumb_angle = buffer[0] * 4;
  servo_pointer_angle = buffer[1] * 4;
  servo_middle_angle = buffer[2] * 4;
  servo_ring_angle = buffer[3] * 4;
  servo_pinky_angle = buffer[4] * 4;
  servo_wrist_angle = buffer[5] * 4;

  // Rotate servo motors to the angles received through serial from Python
  servo_thumb.write(servo_thumb_angle);
  servo_pointer.write(servo_pointer_angle);
  servo_middle.write(servo_middle_angle);
  servo_ring.write(servo_ring_angle);
  servo_pinky.write(servo_pinky_angle);
  servo_wrist.write(servo_wrist_angle);

  LatencyMeasure();
}

void Mode5()
{
  // Combined Control Mode

  flex_5_val = analogRead(flex_5);
  flex_5_val = map(flex_5_val, 200, 450, 0, 180);

  flex_4_val = analogRead(flex_4);
  flex_4_val = map(flex_4_val, 200, 450, 0, 180);

  flex_3_val = analogRead(flex_3);
  flex_3_val = map(flex_3_val, 200, 450, 0, 180);

  flex_2_val = analogRead(flex_2);
  flex_2_val = map(flex_2_val, 180, 350, 0, 180);

  flex_1_val = analogRead(flex_1);
  flex_1_val = map(flex_1_val, 200, 450, 0, 180);

  //    if (Serial.read() == '\n' && lock == true)
  if (Serial.read() == 0 && lock == true)

  {

    // mode_switcher = 0;
    // Read bytes (7 in this case) until the end of the buffer array (i.e. when the newline character is reached)
    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    // Multiply back values received to which were divided in Python to keep them within the require byte range of -128<=value<=128.
    servo_thumb_angle_serial = buffer[0] * 4;
    servo_pointer_angle_serial = buffer[1] * 4;
    servo_middle_angle_serial = buffer[2] * 4;
    servo_ring_angle_serial = buffer[3] * 4;
    servo_pinky_angle_serial = buffer[4] * 4;
    servo_wrist_angle_serial = buffer[5] * 4;

    leap_data_confidence = (double(buffer[6]) / 100);

    lcd.print("   ");
    lcd.setCursor(11, 1);
    // lcd.print("combined");
    lcd.print(leap_data_confidence);

    glove_data_weight = 2 - leap_data_confidence;

    // Weighted average combining Leap Motion Controller and Glove Control data
    servo_thumb_angle = ((leap_data_confidence * servo_thumb_angle_serial) + (glove_data_weight * flex_1_val)) / (leap_data_confidence + glove_data_weight);
    servo_pointer_angle = ((leap_data_confidence * servo_pointer_angle_serial) + (glove_data_weight * flex_2_val)) / (leap_data_confidence + glove_data_weight);
    servo_middle_angle = ((leap_data_confidence * servo_middle_angle_serial) + (glove_data_weight * flex_3_val)) / (leap_data_confidence + glove_data_weight);
    servo_ring_angle = ((leap_data_confidence * servo_ring_angle_serial) + (glove_data_weight * flex_4_val)) / (leap_data_confidence + glove_data_weight);
    servo_pinky_angle = ((leap_data_confidence * servo_pinky_angle_serial) + (glove_data_weight * flex_5_val)) / (leap_data_confidence + glove_data_weight);
    // servo_wrist_angle = ((leap_data_confidence*servo_wrist_angle)+(glove_data_weight*flex_2_val)/(servo_wrist_angle+flex_6_val));

    lcd.print("   ");
    lcd.setCursor(7, 1);
    // lcd.print("combined");
    lcd.print(servo_middle_angle);

    lcd.print("  ");
    lcd.setCursor(1, 1);
    // lcd.print("combined");
    lcd.print(flex_3_val);
  }

  // Rotate servo motors to the angles received through serial from Python
  servo_thumb.write(servo_thumb_angle);
  servo_pointer.write(servo_pointer_angle);
  servo_middle.write(servo_middle_angle);
  servo_ring.write(servo_ring_angle);
  servo_pinky.write(servo_pinky_angle);
  servo_wrist.write(servo_wrist_angle);

  LatencyMeasure();
}

void Mode6()
{
  // Combined Control Interpolated Leap Data

  flex_5_val = analogRead(flex_5);
  flex_4_val = analogRead(flex_4);
  flex_3_val = analogRead(flex_3);
  flex_2_val = analogRead(flex_2);
  flex_1_val = analogRead(flex_1);

  //    if (Serial.read() == '\n' && lock == true)
  if (Serial.read() == 0 && lock == true)

  {

    // mode_switcher = 0;
    // Read bytes (5 in this case) until the end of the buffer array (i.e. when the newline character is reached)
    data = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);

    // Multiply back values received to which were divided in Python to keep them within the require byte range of -128<=value<=128.
    servo_thumb_angle_serial = buffer[0] * 4;
    servo_pointer_angle_serial = buffer[1] * 4;
    servo_middle_angle_serial = buffer[2] * 4;
    servo_ring_angle_serial = buffer[3] * 4;
    servo_pinky_angle_serial = buffer[4] * 4;
    servo_wrist_angle_serial = buffer[5] * 4;

    leap_data_confidence = (double(buffer[6]) / 100);

    lcd.print("   ");
    lcd.setCursor(11, 1);
    // lcd.print("combined");
    lcd.print(leap_data_confidence);

    glove_data_weight = 2 - leap_data_confidence;

    flex_5_val_serial = map(flex_5_val, 200, 450, 0, 180);
    flex_4_val_serial = map(flex_4_val, 200, 450, 0, 180);
    flex_3_val_serial = map(flex_3_val, 200, 450, 0, 180);
    flex_2_val_serial = map(flex_2_val, 180, 350, 0, 180);
    flex_1_val_serial = map(flex_1_val, 200, 450, 0, 180);

    // Weighted average combining Leap Motion Controller and Glove Control data
    servo_thumb_angle = ((leap_data_confidence * servo_thumb_angle_serial) + (glove_data_weight * flex_1_val_serial)) / (leap_data_confidence + glove_data_weight);
    servo_pointer_angle = ((leap_data_confidence * servo_pointer_angle_serial) + (glove_data_weight * flex_2_val_serial)) / (leap_data_confidence + glove_data_weight);
    servo_middle_angle = ((leap_data_confidence * servo_middle_angle_serial) + (glove_data_weight * flex_3_val_serial)) / (leap_data_confidence + glove_data_weight);
    servo_ring_angle = ((leap_data_confidence * servo_ring_angle_serial) + (glove_data_weight * flex_4_val_serial)) / (leap_data_confidence + glove_data_weight);
    servo_pinky_angle = ((leap_data_confidence * servo_pinky_angle_serial) + (glove_data_weight * flex_5_val_serial)) / (leap_data_confidence + glove_data_weight);
    // servo_wrist_angle = ((leap_data_confidence*servo_wrist_angle)+(glove_data_weight*flex_2_val)/(servo_wrist_angle+flex_6_val));

    // Rotate servo motors to the angles received through serial from Python
    servo_thumb.write(servo_thumb_angle);
    servo_pointer.write(servo_pointer_angle);
    servo_middle.write(servo_middle_angle);
    servo_ring.write(servo_ring_angle);
    servo_pinky.write(servo_pinky_angle);
    //    servo_wrist.write(servo_wrist_angle);

    count = 0;
    lcd.print("   ");
    lcd.setCursor(7, 1);
    // lcd.print("combined");
    lcd.print(servo_middle_angle);

    //      lcd.print("  ");
    //      lcd.setCursor(1, 1);
    //      // lcd.print("combined");
    //      lcd.print(servo_middle_angle);
  }

  else
  {

    flex_5_val_interp = map(flex_5_val, 200, 450, 0, 180);
    flex_4_val_interp = map(flex_4_val, 200, 450, 0, 180);
    flex_3_val_interp = map(flex_3_val, 200, 450, 0, 180);
    flex_2_val_interp = map(flex_2_val, 180, 350, 0, 180);
    flex_1_val_interp = map(flex_1_val, 200, 450, 0, 180);

    servo_middle_angle_interp = (double(flex_3_val_interp) / double(flex_3_val_serial)) * (double(servo_middle_angle_serial) / double(flex_3_val_serial)) * servo_middle_angle_serial;

    // Weighted average combining Leap Motion Controller and Glove Control data
    servo_thumb_angle_interp = ((leap_data_confidence * servo_thumb_angle_serial) + (glove_data_weight * flex_1_val_interp)) / (leap_data_confidence + glove_data_weight);
    servo_pointer_angle_interp = ((leap_data_confidence * servo_pointer_angle_serial) + (glove_data_weight * flex_2_val_interp)) / (leap_data_confidence + glove_data_weight);
    servo_middle_angle_interp = ((leap_data_confidence * servo_middle_angle_interp) + (glove_data_weight * flex_3_val_interp)) / (leap_data_confidence + glove_data_weight);
    servo_ring_angle_interp = ((leap_data_confidence * servo_ring_angle_serial) + (glove_data_weight * flex_4_val_interp)) / (leap_data_confidence + glove_data_weight);
    servo_pinky_angle_interp = ((leap_data_confidence * servo_pinky_angle_serial) + (glove_data_weight * flex_5_val_interp)) / (leap_data_confidence + glove_data_weight);

    // Rotate servo motors to the angles received through serial from Python
    servo_thumb.write(servo_thumb_angle_interp);
    servo_pointer.write(servo_pointer_angle_interp);
    servo_middle.write(servo_middle_angle_interp);
    servo_ring.write(servo_ring_angle_interp);
    servo_pinky.write(servo_pinky_angle_interp);
    servo_wrist.write(servo_wrist_angle_interp);

    //      if (leap_data_confidence >= 0.1)
    //      {
    //        leap_data_confidence = leap_data_confidence - 0.001;
    //      }
    //      else
    //      {
    //        leap_data_confidence = 0;
    //      }

    // Changes to 100% glove control mode if Leap Motion controller is not receiving hand tracking data
    if (count >= 3000)
    {
      leap_data_confidence = 0;
    }
    count++;

    lcd.print("   ");
    lcd.setCursor(1, 1);
    // lcd.print("combined");
    lcd.print(servo_middle_angle_interp);
  }

  LatencyMeasure();
}

void ServoChecker()
{
  if (count >=500)
  {
    count = 0;
  lcd.setCursor(0, 1);  
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(servo_thumb.read());
  
  lcd.setCursor(3, 1);
  lcd.print(servo_pointer.read());
  
  lcd.setCursor(6, 1);
  lcd.print(servo_middle.read());
  
  lcd.setCursor(9, 1);
  lcd.print(servo_ring.read());
  
  lcd.setCursor(12, 1);
  lcd.print(servo_pinky.read());
  }
  count ++;

//  //  Serial.print('\n');
//    Serial.print(servo_thumb.read());
//    Serial.print(' ');
//    Serial.print(servo_pointer.read());
//      Serial.print(' ');
//  
//    Serial.print(servo_middle.read());
//      Serial.print(' ');
//  
//    Serial.print(servo_ring.read());
//    Serial.print(' ');
//  
//    Serial.print(servo_pinky.read());
//    Serial.print(' ');
//  
//    Serial.print(servo_wrist.read());
//      Serial.print(' ');

}

void LatencyMeasure()
{
  CurrentTime = millis();
  ElapsedTime = CurrentTime - LocalStartTime;
  Serial.print('\n');
  Serial.print(ElapsedTime);
  SystemStartTime = millis();
}

void loop()
{

  LocalStartTime = millis();

  if (lock == false)
  {
    ModeSelection();
  }
  else if (lock == true)
  {

    switch (mode)
    {
      // case 1:
      //   Mode1();
      //   break;
      case 2:
        Mode2();
        break;
      case 3:
        Mode3();
        break;
      case 4:
        Mode4();
        break;
      case 5:
        Mode5();
        break;
      case 6:
        Mode6();
        break;
    }
            ServoChecker();
  }
}
