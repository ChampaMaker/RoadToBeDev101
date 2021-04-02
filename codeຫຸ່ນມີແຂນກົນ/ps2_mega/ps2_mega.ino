#include <PS2X_lib.h>  //for v1.6
#include <Servo.h>

/*
  right now, the library does NOT support hot pluggable controllers, meaning 
  you must always either restart your Arduino after you conect the controller, 
  or call config_gamepad(pins) again after connecting the controller.
*/
int pos;

#define PWM1 3
#define PWM2 5
#define DIR1 4
#define DIR2 2

#define SERVO 6

Servo myservo; 

// create PS2 Controller Class
PS2X ps2x;
int error = 0; 
byte type = 0;
byte vibrate = 0;

void setup()
{
  myservo.attach(SERVO, 500, 2500);
  myservo.write(90);
  
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  
  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);
  
  Serial.begin(57600);
  // CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  // setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  
  error = ps2x.config_gamepad(13,11,10,12, true, true);
//  delay(3000);
  if(error == 0)
  {
    Serial.println("Found Controller, configured successful");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
  // Serial.print(ps2x.Analog(1), HEX);
  type = ps2x.readType();
  switch(type)
  {
    case 0:
      Serial.println("Unknown Controller type");
      break;
    case 1:
      Serial.println("DualShock Controller Found");
      break;
    case 2:
      Serial.println("GuitarHero Controller Found");
      break;
  }
}

void loop()
{
  /*
    You must Read Gamepad to get new values
    Read GamePad and set vibration values
    ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
    if you don't enable the rumble, use ps2x.read_gamepad(); with no values
    you should call this at least once a second
  */
  // skip loop if no controller found
  if(error == 1)
    return; 
  
  if(type == 2)
  {
    // Guitar Hero Controller
    // read controller
    ps2x.read_gamepad();
    
    if(ps2x.ButtonPressed(GREEN_FRET))
      Serial.println("Green Fret Pressed");
    if(ps2x.ButtonPressed(RED_FRET))
      Serial.println("Red Fret Pressed");
    if(ps2x.ButtonPressed(YELLOW_FRET))
      Serial.println("Yellow Fret Pressed");
    if(ps2x.ButtonPressed(BLUE_FRET))
      Serial.println("Blue Fret Pressed");
    if(ps2x.ButtonPressed(ORANGE_FRET))
      Serial.println("Orange Fret Pressed");
    
    if(ps2x.ButtonPressed(STAR_POWER))
      Serial.println("Star Power Command");
    
    // will be TRUE as long as button is pressed
    if(ps2x.Button(UP_STRUM))
      Serial.println("Up Strum");
    if(ps2x.Button(DOWN_STRUM))
      Serial.println("DOWN Strum");
    
    // will be TRUE as long as button is pressed
    if(ps2x.Button(PSB_START))
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");
    
    // print stick value IF TRUE
    if(ps2x.Button(ORANGE_FRET))
    {
      Serial.print("Wammy Bar Position:");
      Serial.println(ps2x.Analog(WHAMMY_BAR), DEC); 
    } 
  }
  else
  {
    // DualShock Controller
    // read controller and set large motor to spin at 'vibrate' speed
    ps2x.read_gamepad(false, vibrate);
    
    // will be TRUE as long as button is pressed
    if(ps2x.Button(PSB_START))
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");
    
    // will be TRUE as long as button is pressed
    if(ps2x.Button(PSB_PAD_UP)){
      
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
      MoveForward();
      
    }else if(ps2x.Button(PSB_PAD_RIGHT)){
      
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
      TurnRight();
      
    }else if(ps2x.Button(PSB_PAD_LEFT)){
      
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
      TurnLeft();
      
    }else if(ps2x.Button(PSB_PAD_DOWN)){
      
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
      MoveBackward();

     // will be TRUE if button was JUST pressed
    }else if(ps2x.Button(PSB_RED)){
      Serial.println("Circle just pressed");
      MoveRight();
    
    // will be TRUE if button was JUST released
    }else if(ps2x.Button(PSB_PINK)){
      Serial.println("Square just released");
      MoveLeft();     
      
    }else if(ps2x.Button(PSB_L2)){
      Serial.println("L2 pressed");
      servoCatch();
    }else if(ps2x.Button(PSB_R2)){
      Serial.println("R2 pressed");
      servoRelease();
        
    }else if(ps2x.Button(PSB_GREEN)){
      Serial.println("Triangle just press");
        ArmUp();
    }else if(ps2x.Button(PSB_BLUE)){
      Serial.println("X just press");
       ArmDown();
    }else{      
      Serial.println("Stop");
      Stop();
      servoStop();
      ArmStop();
    }
    
//    // this will set the large motor vibrate speed based on how hard you press the blue (X) button
//    vibrate = ps2x.Analog(PSAB_BLUE);
//    
//    // will be TRUE if any button changes state (on to off, or off to on)
//    if (ps2x.NewButtonState())
//    {
//      if(ps2x.Button(PSB_L3))
//        Serial.println("L3 pressed");
//      if(ps2x.Button(PSB_R3))
//        Serial.println("R3 pressed");
//      if(ps2x.Button(PSB_GREEN))
//        Serial.println("Triangle pressed");
//    }
//    
//    // will be TRUE if button was JUST pressed OR released
//    if(ps2x.NewButtonState(PSB_BLUE))
//      Serial.println("X just changed");    
//    
//    // print stick values if either is TRUE
//    if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1))
//    {
//      Serial.print("Stick Values:");
//      // Left stick, Y axis. Other options: LX, RY, RX  
//      Serial.print(ps2x.Analog(PSS_LY), DEC);
//      Serial.print(",");
//      Serial.print(ps2x.Analog(PSS_LX), DEC); 
//      Serial.print(",");
//      Serial.print(ps2x.Analog(PSS_RY), DEC); 
//      Serial.print(",");
//      Serial.println(ps2x.Analog(PSS_RX), DEC); 
//    }
  }
  delay(50);
}

void MoveForward(){
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);
  
  analogWrite(PWM1, 255);
  analogWrite(PWM2, 255);
}

void MoveBackward(){
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  
  analogWrite(PWM1, 255);
  analogWrite(PWM2, 255);

  
}

void TurnRight(){
  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, LOW);
  
  analogWrite(PWM1, 255);
  analogWrite(PWM2, 255);
  
}

void TurnLeft(){
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
  
  analogWrite(PWM1, 255);
  analogWrite(PWM2, 255);
 
}

void Stop(){
  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);
}

void MoveRight(){
  
}

void MoveLeft(){
  
}

void servoStop(){
  pos = 1500;
  myservo.writeMicroseconds(pos);
  delay(15);
}

void servoCatch() {
    pos = 2500;
    myservo.writeMicroseconds(pos);              // if pos > 1500 is forward
    delay(15);                                   // waits 15ms for the servo to reach the position
}

void servoRelease() {
    pos = 0;
    myservo.writeMicroseconds(pos);              // if pos < 1300 is backward 
    delay(15);                                   // waits 15ms for the servo to reach the position
}

void ArmDown(){
 
}

void ArmUp(){
  
}

void ArmStop(){
  
}
