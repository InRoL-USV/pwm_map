#include "Arduino.h"
#include <ros.h>
#include <Servo.h>
#include <std_msgs/String.h>
#include <std_msgs/Char.h>
#include <string.h>
#include "path_following/thrust.h"
#define pin1 10
#define pin2 9
#define pin3 12
#define pin4 11
#define pos 1500
#define inc 30
#define NUMBER_OF_THRUSTER 4
#define th_high 1760
#define th_low 1240
Servo th_servo[NUMBER_OF_THRUSTER];
int pin[4] = {pin1, pin2, pin3, pin4};
int thrust[4] = {1500, 1500, 1500, 1500};

ros::NodeHandle nh;

int pwm_mapping(double th){
  // 2nd-order polynomial mapping
  double a=0;
  double b=0;
  double c=0;
  double d=0;
  double e=0;
  double f=0;
  double g=1500;
  if (th > 0){
    a=-0.0290;
    b=0.8314;
    c=-9.1853;
    d=49.2322;
    e=-132.9114;
    f=190.5353;
    g=1500;
  }
  else{
    a=0.0736;
    b=1.9132;
    c=19.1244;
    d=92.1220;
    e=219.7946;
    f=261.2094;
    g=1500;
  }
  return a*pow(th,6)+b*pow(th,5)+c*pow(th,4)+d*pow(th,3)+e*pow(th,2)+f*th+g;
}

int ctrl_flag = 1;
void callback(const path_following::thrust& thrust_input){
  if (ctrl_flag==0){
  thrust[0] = pwm_mapping(thrust_input.f_1);
  thrust[1] = pwm_mapping(thrust_input.f_2);
  thrust[2] = pwm_mapping(thrust_input.f_3);
  thrust[3] = pwm_mapping(thrust_input.f_4);
  }
  else{
    for(int i=0; i<NUMBER_OF_THRUSTER; i++){
      thrust[i]=0;
    }
  }
  //thrust[0] = 1600;
  //thrust[1] = 1600;
  //thrust[2] = 1600;
  //thrust[3] = 1600;
  //for (int i=0; i<NUMBER_OF_THRUSTER; i++){
    //th_servo[i].write(thrust[i]);
    //th_servo[i].write(1600);
  //}
//  delay(160);
}

void key_callback(const std_msgs::String& key){
  char c = key.data[0];
  if(c == 'm'){
    if (ctrl_flag==1){
      ctrl_flag=0;
    }
    else{
      for (int i=0; i<NUMBER_OF_THRUSTER; i++){
        thrust[i]=1500;
      }
      ctrl_flag=1;
    }
  }
  if(ctrl_flag==1){
    if(c == 'w'){
      //digitalWrite(LED_BUILTIN, HIGH);
      thrust[0] += inc;
      thrust[1] += inc;
      thrust[2] += inc;
      thrust[3] += inc;
    }
    else if(c == 'x'){
      thrust[0] -= inc;
      thrust[1] -= inc;
      thrust[2] -= inc;
      thrust[3] -= inc;
    }
    else if(c == 'a'){
      thrust[0] -= inc;
      thrust[1] += inc;
      thrust[2] += inc;
      thrust[3] -= inc;
    }
    else if(c == 'd'){
      thrust[0] += inc;
      thrust[1] -= inc;
      thrust[2] -= inc;
      thrust[3] += inc;
    }
    else if(c == 's'){
      thrust[0] = 1500;
      thrust[1] = 1500;
      thrust[2] = 1500;
      thrust[3] = 1500;
    }
  }
}
ros::Subscriber<path_following::thrust> sub("/thrust_data", callback);

ros::Subscriber<std_msgs::String> keysub("/key_pub", key_callback);

void setup() {

  //pinMode(LED_BUILTIN, OUTPUT);
  //delay(100);

  //delay(100);
  for (int i=0; i<NUMBER_OF_THRUSTER; i++){
    th_servo[i].attach(pin[i]); // motor1
  }
  for (int i=0; i<NUMBER_OF_THRUSTER; i++){
    th_servo[i].write(pos);
  }
  //delay(10000);

  nh.getHardware()->setBaud(115200);
  nh.initNode();
  nh.subscribe(sub);

}
void loop() {

  //digitalWrite(LED_BUILTIN, LOW);
  //delay(10);
  nh.spinOnce();
  //delay(10);
  for(int i=0; i<NUMBER_OF_THRUSTER; i++){
    th_servo[i].write(thrust[i]);
  }  
  //servoFL.write(th1); //1
  //servoFR.write(th4); //4
  //servoRL.write(th3); //3
  //servoRR.write(th2); //2

}
