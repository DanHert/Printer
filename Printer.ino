#define encoder0PinA  2
#define encoder0PinB  3
#define motorRight  7
#define motorLeft  6
#define laser  8
#define stepper  PA12

// 1 position of encoder0Pos is equivalent to 0.04233 mm (2.54/60)
// 1 position of stepper is equivalent to 0.0132 mm

volatile int encoder0Pos = 5000;
int target = 6000;
bool turn;
int movement = 0;
int8_t in;
uint16_t it = 0;
bool canstop = 0;
bool line[600];
int longstep = 0;
int shortstep = 0;

void setup() {
  pinMode(motorRight, OUTPUT);
  pinMode(motorLeft, OUTPUT);
  pinMode(laser, OUTPUT);
  pinMode(stepper, OUTPUT);
  digitalWrite(motorRight, LOW);
  digitalWrite(motorLeft, LOW);
  digitalWrite(laser, LOW);
  digitalWrite(stepper, LOW);

  pinMode(13, OUTPUT);
  //  pinMode(A3, INPUT);
  //  pinMode(A4, INPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);

  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);

  Serial.begin (115200);
  motorTurnLeft();
  delay(2000);
  encoder0Pos = 1000;
  motorStop();
  delay(500);
  attachInterrupt(2, doEncoderA, CHANGE);
  attachInterrupt(3, doEncoderB, CHANGE);
  //attachInterrupt(39, doEncoderA, CHANGE);
  //attachInterrupt(40, doEncoderB, CHANGE);
  movement = 0;

}


void loop() {
  while (true) {

    in = Serial.read();
    if (in == -1) {
      continue;
    } else if (in == 0 || in == 1) {
      line[it] = in;
      it++;
    }
    else if (in == 's') {
      in = 'a';

      if (movement == 0) {
        motorTurnRight();
      } else {
        motorTurnLeft();
      }
      canstop = 0;
      while (1) {
        if (encoder0Pos > 2500 && encoder0Pos < (2500 + it) && movement == 1) {
          digitalWrite(laser, !line[encoder0Pos - 2500]);
        } else {
          digitalWrite(laser, LOW);
        }
        if (encoder0Pos == 2400) {
          canstop = 1;
        }
        if (encoder0Pos < 1500 && canstop) {          
          motorStop();
          delay(100);
          stepone();
          delay(100);
          shortstep++;
          longstep++;
          if (shortstep == 3 && longstep < 15) {
            shortstep = 0;
            Serial.print('s');
            it = 0;
            break;
          } else if (longstep == 16) {
            shortstep = 0;
            longstep = 0;
            Serial.print('s');
            it = 0;
            break;
          }
          canstop = 0;
          delay(10);
          motorTurnRight();
        }
        if (encoder0Pos > 3000 + it) {
          delay(10);
          motorTurnLeft();
        }
      }

    }
  }
}
/*

          if (longstep == 4) {
            step(4);
            longstep = 0;
          } else {
            step(3);
            longstep++;
          }
          Serial.print('s');
          it = 0;

*/


/*void step (int steps) {
  for (int i = 0; i < steps; i++) {
    delay(10);
    digitalWrite(stepper, HIGH);
    delay(10);
    digitalWrite(stepper, LOW);
    delay(10);
  }
}*/

void stepone(){
  digitalWrite(stepper, HIGH);
  delay(10);
  digitalWrite(stepper, LOW);
  delay(10);
}

void motorTurnLeft() {
  digitalWrite(motorLeft, LOW);
  digitalWrite(motorRight, LOW);
  delay(5);
  digitalWrite(motorRight, HIGH);
  movement = 0;
}

void motorTurnRight() {
  digitalWrite(motorLeft, LOW);
  digitalWrite(motorRight, LOW);
  delay(5);
  digitalWrite(motorLeft, HIGH);
  movement = 1;
}
void motorStop() {
  digitalWrite(motorLeft, LOW);
  digitalWrite(motorRight, LOW);
  delay(5);
  //movement = 1;
}
void doEncoderA() {
  if (digitalRead(encoder0PinA) == HIGH) {
    if (digitalRead(encoder0PinB) == LOW) {
      encoder0Pos--;
    }
    else {
      encoder0Pos++;
    }
  }
  else
  {
    if (digitalRead(encoder0PinB) == HIGH) {
      encoder0Pos--;
    }
    else {
      encoder0Pos++;
    }
  }
}

void doEncoderB() {
  if (digitalRead(encoder0PinB) == HIGH) {
    if (digitalRead(encoder0PinA) == HIGH) {
      encoder0Pos--;
    }
    else {
      encoder0Pos++;
    }
  }
  else {
    if (digitalRead(encoder0PinA) == LOW) {
      encoder0Pos--;
    }
    else {
      encoder0Pos++;
    }
  }
}

