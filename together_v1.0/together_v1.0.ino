#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>

Encoder myEnc(19, 18);
float Position_KP = 0.1, Position_KI = 0, Position_KD = 10000;
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
  Serial.println("Start Test:");
  cli(); // Désactive l'interruption globale
  bitClear (TCCR2A, WGM20); // WGM20 = 0
  bitClear (TCCR2A, WGM21); // WGM21 = 0
  TCCR2B = 0b00000101; // Clock / 128 soit 16 micro-s et WGM22 = 0
  TIMSK2 = 0b00000001; // Interruption locale autorisée par TOIE2
  sei(); // Active l'interruption globale
}
byte varCompteur = 0; // La variable compteur
long nowPosition  = -999;

// Routine d'interruption
ISR(TIMER2_OVF_vect) {
  TCNT2 = 128 - 125; // 125 x 8 µS = 1 ms
  if (varCompteur++ > 10) { // 50 * 1 ms = 50 ms (demi-période)
    varCompteur = 0;
    long newPosition = myEnc.read();
    if (newPosition - nowPosition > 20 || newPosition - nowPosition < -20) {
      nowPosition = newPosition;
      Serial.print("nowPosition: "); Serial.println(nowPosition);
      //turn(nowPosition, target);turn放在这里就不能正常运行，好神奇
    }
    //Serial.print("nowPosition: "); Serial.println(nowPosition);
    turn(nowPosition, target);
  }
}

void loop() {
  if (! baro.begin()) {
    Serial.println("Couldnt find sensor");
    delay(1000);
    return;
  }

  temperature = baro.getTemperature();
  Serial.print("temperature: "); Serial.println(temperature);
  target = map(temperature, 10, 30, 0, 475);
  Serial.print("target: "); Serial.println(target);
  delay(1000);
}

void turn (int nowPosition, int target)
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
  if (Pwm > 255) {
    Pwm = 255;
  }
  else if (Pwm < -255) {//此部分删除会导致电机震荡，原因不明
    Pwm = -255;
  }
  analogWrite(motor, Pwm);
}
