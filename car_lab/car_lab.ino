// 47 - расстояние 56 - скорость / врепед - HIGH  назад - LEFT
#define DIR_RIGHT 4 
#define SPEED_RIGHT 5
#define DIR_LEFT 7 
#define SPEED_LEFT 6

#define EYE_LEFT_ECHO 11
#define EYE_LEFT_TRIGGER 12

#define EYE_FRONT_ECHO 9
#define EYE_FRONT_TRIGGER 8

void move(int rspeed, int lspeed){
  bool lforward = lspeed <= 0;
  bool rforward = rspeed > 0;
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
  RotateRight
};

State state = State::Forward;

int LD = 20;
int CD = 15;
int FD = 30;

int ld = 0;
int fd = 0; 
int rotation_counter = 0;
int prev_ld = 0;
int rotate_delay = 0;

int last_millis = 0;

void set_state(int new_state) {
  if (new_state != state) {
    rotation_counter = 450;
    state = new_state;
  }
}

// int state_forward_run() {
//   move(255, 255);
//   if (fd > CD && CD < ld && ld <FD){
//     return State::Forward;
//   }
//   if (ld > FD){
//     return State::TurnLeft;
//   }
//   if (ld < CD){
//     return State::TurnRight;
//   }
//   if (fd < CD && CD < ld && ld < FD){
//     rotation_counter = 250;
//     return State::RotateRight;
//   }
//   if (fd < FD && ld -prev_ld > LD){
//     rotation_counter = 250;
//     return State::RotateLeft;
//   }

  
//   if (fd < FD) {
//     return State::Stop;
//   }
// }


int state_forward_run() {
  move(255, 255);
  // if (ld > LD && prev_ld > LD) {
  //   delay(200);
  //   return State::RotateLeft;
  // }
  if (fd < FD) {
    return State::RotateRight;
  }
  if (ld > 40 + 2 && prev_ld > 40 + 2) {
    delay(200);
    return State::RotateLeft;
  }
  if (ld < CD + 5 && prev_ld < CD + 5) {
    return State::TurnRight;
  }
  if (ld > CD + 2 && prev_ld > CD + 2) {
    return State::TurnLeft;
  }

  return State::Forward;
}



int state_stop_run() {
  move(0, 0);
  // if (fd > CD && CD < ld < FD) {
  if (ld < FD) {
    return State::Forward;
  }
  return State::Stop;
}

int state_turn_left_run() {
  move(150, 255);
  // if (fd > CD && CD < ld < FD) {
  // if (ld < FD) {
  //   return State::Forward;
  // }
  // return State::TurnLeft;
  return State::Forward;
}

int state_turn_right_run() {
  move(255, 150);
  // if (fd > CD && CD < ld < FD) {
  // if (ld < FD) {
  //   return State::Forward;
  // }
  // return State::TurnRight;
  return State::Forward;
}

int state_rotate_left_run() {
  move(0, 255);
  if (rotation_counter <= 0) {
    return State::Forward;
  }
  return State::RotateLeft;
}

int state_rotate_right_run() {
  move(255, 0);
  if (rotation_counter <= 0) {
    return State::Forward;
  }
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
  pinMode(EYE_LEFT_ECHO, INPUT);
  pinMode(EYE_LEFT_TRIGGER, OUTPUT);
  pinMode(EYE_FRONT_ECHO, INPUT);
  pinMode(EYE_FRONT_TRIGGER, OUTPUT);
}

void loop() {
  digitalWrite(EYE_FRONT_TRIGGER, LOW);
  delayMicroseconds(5);
  digitalWrite(EYE_FRONT_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(EYE_FRONT_TRIGGER, LOW);
  fd = pulseIn(EYE_FRONT_ECHO, HIGH) / 2 / 29.1;

  digitalWrite(EYE_LEFT_TRIGGER, LOW);
  delayMicroseconds(5);
  digitalWrite(EYE_LEFT_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(EYE_LEFT_TRIGGER, LOW);
  ld = pulseIn(EYE_LEFT_ECHO, HIGH) / 2 / 29.1;

  state_run();
  if (rotation_counter > 0) {
    rotation_counter -= millis() - last_millis;
  }
  last_millis = millis();
  prev_ld = ld;
  Serial.print(ld);
  Serial.print(" ");
  Serial.print(fd);
  Serial.print(" ");
  Serial.println(state);
}