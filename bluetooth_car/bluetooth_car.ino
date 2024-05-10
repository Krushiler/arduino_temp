#include <SoftwareSerial.h>
#include <EEPROM.h>

SoftwareSerial mySerial(10, 11);

// 47 - расстояние 56 - скорость / врепед - HIGH  назад - LEFT
#define DIR_RIGHT 4 
#define SPEED_RIGHT 5
#define DIR_LEFT 7 
#define SPEED_LEFT 6

bool raceMode = 0;

const int spin_calibration_states[4][2] = { {0, 0}, {1, 1}, {0, 1}, {1, 0} };
int calibration_state = 0;

void move(int rspeed, int lspeed){
  bool lforward = lspeed > 0 ^ spin_calibration_states[calibration_state][0];
  bool rforward = rspeed > 0 ^ spin_calibration_states[calibration_state][1];
  digitalWrite(DIR_RIGHT, rforward);
  digitalWrite(DIR_LEFT, lforward);
  analogWrite(SPEED_RIGHT, abs(rspeed));
  analogWrite(SPEED_LEFT, abs(lspeed));
}

enum State {
  Stop,
  Forward,
  TurnLeft,
  TurnRight,
  RotateLeft,
  RotateRight,
  Backward
};

State state = State::Stop;
State prevState = State::Stop;

int rotation_counter = 0;
int rotate_delay = 0;

int last_millis = 0;

void set_state(int new_state) {
  if (new_state != state) {
    rotation_counter = 450;
    state = new_state;
  }
}

void set_state_force(int new_state) {
  rotation_counter = 450;
  state = new_state;
}

int state_forward_run() {
  move(255, 255);
  return State::Forward;
}

int state_backward_run() {
  move(-255, -255);
  return State::Backward;
}

int state_stop_run() {
  move(0, 0);
  return State::Stop;
}

int state_turn_left_run() {
  move(80, 255);
  return State::TurnLeft;
}

int state_turn_right_run() {
  move(255, 80);
  return State::TurnRight;
}

int state_rotate_left_run() {
  move(-255, 255);
  // if (rotation_counter <= 0) {
  //   return State::Forward;
  // }
  return State::RotateLeft;
}

int state_rotate_right_run() {
  move(255, -255);
  // if (rotation_counter <= 0) {
  //   return State::Forward;
  // }
  return State::RotateRight;
}

int process_state() {
  switch (state) {
    case State::Stop:
      return state_stop_run();
    case State::Forward:
      return state_forward_run();
    case State::TurnRight:
      return state_turn_right_run();
    case State::TurnLeft:
      return state_turn_left_run();
    case State::RotateLeft:
      return state_rotate_left_run();
    case State::RotateRight:
      return state_rotate_right_run();
    case State::Backward:
      return state_backward_run();
  }
}

void state_run() {
  int new_state = process_state();
  set_state(new_state);
}

void setup() {
  Serial.begin(9600);
  last_millis = millis();
  for(int i = 4; i <= 7; i++){
    pinMode(i, OUTPUT);
  }
  Serial.println("Goodnight PC!");

  mySerial.begin(9600);
  mySerial.println("Hello, Phone?");
  
  calibration_state = EEPROM.read(0);
}

int action_counter = 0;

void loop() {
  prevState = state;

  if (mySerial.available()) {
    char c = mySerial.read();
    Serial.write(c);
    if (c == 'C') {
      calibration_state = (calibration_state + 1) % 4;
      EEPROM.put(0, calibration_state);
    }
    if (c == 'T') {
      raceMode = !raceMode;
      if (!raceMode) {
        action_counter = 0;
      }
    }
    if (c == '0') {
      action_counter -= 1;
      if (!raceMode) {
        set_state_force(State::Stop);
      }
    } else {
      action_counter += 1;
    }
    if (action_counter <= 0) {
      if (raceMode) {
        set_state_force(State::Stop);
      }
    }
    if (c == 'F') {
      set_state_force(State::Forward);
    }
    if (c == 'R') {
      
      if (!raceMode) {
        set_state_force(State::RotateRight);
      } else {
        set_state_force(State::TurnRight);
      }
    }
    if (c == 'L') {
      if (!raceMode) {
        set_state_force(State::RotateLeft);
      } else {
        set_state_force(State::TurnLeft);
      }
    }
    if (c == 'B') {
      set_state_force(State::Backward);
    }
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }

  state_run();  
  if (rotation_counter > 0) {
    rotation_counter -= millis() - last_millis;
  }
  
  last_millis = millis();
}