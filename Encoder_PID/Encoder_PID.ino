#include <Encoder.h>


Encoder myEnc(19, 18);
float Position_KP = 0.5, Position_KI = 0, Position_KD = 10000;
int motor = 6;
int order;
int target;
int direct = 7;

void setup() {
  pinMode (motor, OUTPUT);
  pinMode (direct, OUTPUT);
  Serial.begin(9600);
  //Serial.println("Basic Encoder Test:");

}

long nowPosition  = -999;

void loop() {
  long newPosition = myEnc.read();
  if (newPosition - nowPosition > 50 || newPosition - nowPosition < -50) {
    nowPosition = newPosition;
    Serial.print(target);
    Serial.print(",");
    Serial.println(nowPosition);
  }

  target = 1000;
  turn(nowPosition, target);
  //  Serial.print(target);
  //  Serial.print(",");
  //  Serial.println(nowPosition);
  delay(10);
}

int turn (int nowPosition, int target)
{
  static float Bias, Pwm, Integral_bias, Last_Bias;
  Bias = target - nowPosition;
  if (Bias > 0) {
    digitalWrite(direct, HIGH);
  } else {
    Bias = -Bias;
    digitalWrite(direct, LOW);
  }
  Integral_bias += Bias;
  Pwm = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
  Last_Bias = Bias;
  if (Pwm > 50) {
    Pwm = 50;
  }
  analogWrite(motor, Pwm);
}

