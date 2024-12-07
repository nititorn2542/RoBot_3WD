#!/usr/bin/env python

import rospy
from std_msgs.msg import Int32
from geometry_msgs.msg import Twist
from opencv_apps.msg import CircleArrayStamped

class Driver:
	def __init__(self):
		rospy.init_node('listener', anonymous=True)
		self.pub = rospy.Publisher('/cmd_vel', Twist, queue_size=5)
	        self.cir = rospy.Subscriber('/hough_circles/circles', CircleArrayStamped, self.callback) 

		self.vel = None
	def callback(self,data):
    		x = int(data.circles[0].center.x)
    		y = data.circles[0].center.y
    		r = data.circles[0].radius
    		rospy.loginfo('X :%d Y:%d R:%d',x,y,r)
    		
		self.vel = Twist()

    		#rate = rospy.Rate(1)

		if y > 400:
			self.vel.linear.x = -0.4
		elif y < 190:
			self.vel.linear.x = 0.4
		else: 
			self.vel.linear.x = 0
        	if x < 200:
                	self.vel.angular.z = 0.6
        	elif x > 440:
                	self.vel.angular.z = -0.6
        	else:
                	self.vel.angular.z = 0
        	self.pub.publish(self.vel)


if __name__ == '__main__':
	Driver()
	rospy.spin()
