#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>

Encoder myEnc(2, 3);
float Position_KP = 5, Position_KI = 0, Position_KD = 1000;
int motor = 6;
int order;
int target;
int direct = 7;
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
float temperature;

void setup() {
  pinMode (motor, OUTPUT);
  pinMode (direct, OUTPUT);
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");

}

long nowPosition  = -999;

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != nowPosition) {
    nowPosition = newPosition;
    Serial.println(nowPosition);
  }

  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    delay(1000);
    return;
  }

  temperature = baro.getTemperature();
  target = map(temperature, 10, 30, 0, 475);
  Serial.print(temperature); Serial.println(target);
  turn(nowPosition, target);
  delay(10);
}

int PWM_PID (int nowPosition, int target)
{
  static float Bias, Pwm, Integral_bias, Last_Bias;
  Bias = target - nowPosition;
  Integral_bias += Bias;
  Pwm = Position_KP * Bias + Position_KI * Integral_bias + Position_KD * (Bias - Last_Bias);
  Last_Bias = Bias;
  if (Pwm > 255) {
    return 255;
  }
  else if (Pwm < -255) {
    return -255;
  }
  else {
    return Pwm;
  }
}

void turn(int nowPosition, int target) {
  if (target > nowPosition) {
    digitalWrite(direct, HIGH);
  } else {
    digitalWrite(direct, LOW);
  }
  analogWrite(motor, PWM_PID (nowPosition, target));
}
