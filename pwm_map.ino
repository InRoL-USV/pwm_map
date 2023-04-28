#include "Arduino.h"
#include <ros.h>
#include <Servo.h>
#include <std_msgs/String.h>
#include <std_msgs/Char.h>
#include <string.h>
#include "path_following/thrust.h"
#define pin1 1
#define pin2 4
#define pin3 6
#define pin4 5
#define pos 1500
#define inc 30
#define NUMBER_OF_THRUSTER 4
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
  double e=1500;
  if (th > 0){
    a=-0.1028;
    b=2.3557;
    c=-18.5975;
    d=102.7624;
    e=1500;
  }
  else{
    a=0.1297;
    b=2.9214;
    c=22.9818;
    d=114.3421;
    e=1500;
  }
  return a*pow(th,4)+b*pow(th,3)+c*pow(th,2)+d*th+e;
}

int stop_flag = 0;
void callback(const path_following::thrust& thrust_input){
  if (stop_flag==0){
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
void stop_callback(const std_msgs::String& stop_signal){
  for (int i=0; i<NUMBER_OF_THRUSTER; i++){
    thrust[i]=0;
  }
  stop_flag = 1;
}
ros::Subscriber<path_following::thrust> sub("/thrust_data", callback);

ros::Subscriber<std_msgs::String> stopsub("/stop_sig", stop_callback);

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
  nh.subscribe(stopsub);

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
