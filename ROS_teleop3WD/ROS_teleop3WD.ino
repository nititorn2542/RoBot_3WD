#include <ArduinoHardware.h>
#include <ros.h>
#include <math.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Float32.h>
#include <Encoder.h>

Encoder Enc_A(18, 27);
Encoder Enc_B(2, 29);
Encoder Enc_C(19, 26);

long encPos_A;
long encPos_B;
long encPos_C;

double x;
double y;
double z;

double f1, f2, f3;
float dWheel = 0.058; //ความกว้างล่อ
float pi = 3.14;
float wheelD = 0;
float baseR = 0.25; //ความยาวถึงล่อ
int reso = 960;//3500

ros::NodeHandle nh;

geometry_msgs::Twist msg;
geometry_msgs::Twist enc;

ros::Publisher Enc("dist", &enc);

void roverCallBack(const geometry_msgs::Twist& cmd_vel)
{
  x = cmd_vel.linear.x;
  y = cmd_vel.linear.y;
  z = cmd_vel.angular.z;
}
ros::Subscriber <geometry_msgs::Twist> Motor("/cmd_vel", roverCallBack);

void setup()
{
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT); pinMode(11, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(22, OUTPUT); pinMode(23, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(24, OUTPUT); pinMode(25, OUTPUT);
  pinMode(44, OUTPUT);

  digitalWrite(44, HIGH); delay(100);
  digitalWrite(44, LOW); delay(100);
  digitalWrite(44, HIGH); delay(100);
  digitalWrite(44, LOW); delay(100);

  nh.initNode();
  nh.subscribe(Motor);
  nh.advertise(Enc);
}

void loop()
{
  motor();

  encPos_A = Enc_A.read();
  encPos_B = Enc_B.read();
  encPos_C = Enc_C.read();

  if ((encPos_A > 1000000) || (encPos_B > 1000000) || (encPos_C > 1000000) || (encPos_A < -1000000) || (encPos_B < -1000000) || (encPos_C < -1000000))
  {
    Enc_A.write(0);    Enc_C.write(0);
    Enc_B.write(0);
  }
  wheelD = dWheel * pi;
  f1 = (wheelD * encPos_A) / reso;
  f2 = (wheelD * encPos_B) / reso;
  f3 = (wheelD * encPos_C) / reso;

  enc.linear.x = ((sqrt(3) * f1) - (sqrt(3) * f2)) / 3;
  enc.linear.y = -(f1 + f2 - (2 * f3)) / 3;
  enc.angular.z = -((f1 + f2 + f3) / 3) / baseR;

  Enc.publish( &enc);
  nh.spinOnce();
  delay(10);
}

void motor()
{
  float v0 = ((x / 0.57735) + (-y * 3 / 2) + (-z * 0.6)) * 10;
  float v1 =  ((x / 0.57735) + (y * 3 / 2) + (z * 0.6)) * 10;
  float v2 =  (((y * 3)) + (-z * 0.6)) * 10;
  if (v0 >= 0)
  {
    analogWrite(6, max(min(abs(v0 * 20), 150), 0));
    digitalWrite(24, LOW); digitalWrite(25, HIGH);
  }
  else
  {
    analogWrite(6, max(min(abs(v0 * 20), 150), 0));
    digitalWrite(24, HIGH); digitalWrite(25, LOW);
  }
  if (v1 >= 0)
  {
    analogWrite(4, max(min(abs(v1 * 20), 150), 0));
    digitalWrite(10, HIGH); digitalWrite(11, LOW);
  }
  else
  {
    analogWrite(4, max(min(abs(v1 * 20), 150), 0));
    digitalWrite(10, LOW); digitalWrite(11, HIGH);
  }
  if (v2 >= 0)
  {
    analogWrite(7, max(min(abs(v2 * 20), 150), 0));
    digitalWrite(22, LOW); digitalWrite(23, HIGH);
  }
  else
  {
    analogWrite(7, max(min(abs(v2 * 20), 150), 0));
    digitalWrite(22, HIGH); digitalWrite(23, LOW);
  }
}
