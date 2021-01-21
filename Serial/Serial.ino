char comchar;

int motor = 6;
int direct = 7;

void setup() {
  Serial.begin(9600);
  while (Serial.read() >= 0) {} //clear serialbuffer
  pinMode (motor, OUTPUT);
  pinMode (direct, OUTPUT);
}

void loop() {
  // read data from serial port

  while (Serial.available() > 0) {
    comchar = Serial.read();//读串口第一个字节
    Serial.print("Serial.read: ");
    Serial.println(comchar);
    if (comchar == '-') {
      digitalWrite(direct, LOW);
      analogWrite(motor, 50);
      delay(20);
      analogWrite(motor, 0);
      while (Serial.read() >= 0) {}
    } else if (comchar == '+') {
      digitalWrite(direct, HIGH);
      analogWrite(motor, 50);
      delay(20);
      analogWrite(motor, 0);
      while (Serial.read() >= 0) {}
    } else if (comchar == '1') {
      digitalWrite(direct, HIGH);
      analogWrite(motor, 50);
      delay(100);
      analogWrite(motor, 0);
      while (Serial.read() >= 0) {}
    }else if (comchar == '2') {
      digitalWrite(direct, LOW);
      analogWrite(motor, 50);
      delay(100);
      analogWrite(motor, 0);
      while (Serial.read() >= 0) {}
    }else if (comchar == '=') {
      while (Serial.read() >= 0) {}
      return;
    } else {
      Serial.println("Error!");
      while (Serial.read() >= 0) {}
    }
    delay(100);
  }
}
