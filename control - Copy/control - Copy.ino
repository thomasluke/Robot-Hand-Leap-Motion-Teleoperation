int data = 1;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
if (Serial.available() > 0) // Only run when serial data is received
{
    data = Serial.read();

    if (data == 100 && digitalRead(LED_BUILTIN) == LOW)
    {
      //Serial.println("yay");
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if (data == 0 && digitalRead(LED_BUILTIN) == HIGH)
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
}
}
