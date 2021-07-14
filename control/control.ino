#include <Servo.h>

double data;
double data_conversion_factor;
double data_angle_pointer;

Servo servo_thumb;
Servo servo_pointer;
Servo servo_index;
Servo servo_ring;
Servo servo_pinky;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  servo_pointer.attach(3);

}

void loop()
{
  // put your main code here, to run repeatedly:
if (Serial.available() > 0) // Only run when serial data is received
{
    data = Serial.read();
    data_conversion_factor = (180/(1.9-0.2));
    data_angle_pointer = (data) * data_conversion_factor;

    servo_pointer.write(data_angle_pointer);
}
}
