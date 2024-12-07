#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include "std_msgs/Float32.h"

float x=0;
float y=0;
float th=0;

double distX=0;
double distY=0;
double distTH=0;

void dist_callback(const geometry_msgs::Twist& dist)
{
	distX = dist.linear.x;
	distY = dist.linear.y;
	distTH = dist.angular.z;
}
//..................................................................................
int main(int argc, char** argv){
  ros::init(argc, argv, "odometry_publisher");

  ros::NodeHandle n;
  ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 50);

  ros::Subscriber subdist = n.subscribe("dist", 1000, dist_callback);
  tf::TransformBroadcaster odom_broadcaster;


  ros::Time current_time, last_time;
  current_time = ros::Time::now();
  last_time = ros::Time::now();

  ros::Rate r(20.0);
  while(n.ok()){

    ros::spinOnce();               // check for incoming messages
   	current_time = ros::Time::now();

	double dt = (current_time - last_time).toSec();
	double delta_x = distX;
	double delta_y = distY;
	double delta_th = distTH;

    //compute odometry in a typical way given the velocities of the robot
   	x = delta_x;
    	y = delta_y;
    	th = delta_th;

    //since all odometry is 6DOF we'll need a quaternion created from yaw
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

    //first, we'll publish the transform over tf
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = ros::Time::now();
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    odom_trans.transform.translation.x = x;
    odom_trans.transform.translation.y = y;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = odom_quat;

    //send the transform
    odom_broadcaster.sendTransform(odom_trans);

    //next, we'll publish the odometry message over ROS
    nav_msgs::Odometry odom;
    odom.header.stamp = ros::Time::now();
    odom.header.frame_id = "odom";

    //set the position
    odom.pose.pose.position.x = x;
    odom.pose.pose.position.y = y;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation = odom_quat;

    //set the velocity
    odom.child_frame_id = "base_link";
    odom.twist.twist.linear.x = delta_x/dt;
    odom.twist.twist.linear.y = delta_y/dt;
    odom.twist.twist.angular.z = delta_th/dt;

    //publish the message
    odom_pub.publish(odom);

    	last_time = current_time;

    r.sleep();
  }
}

