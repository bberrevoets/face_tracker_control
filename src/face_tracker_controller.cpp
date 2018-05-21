#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <geometry_msgs/Twist.h>
#include <iostream>

#include <face_tracker_pkg/centroid.h>

int screenmaxx, center_offset, center_left, center_right;
float current_pos_x, tracking_speed;
ros::Publisher pub;
geometry_msgs::Twist cmd;

void track_face(int x, int y)
{
    cmd.angular.z = 0.0;
    if (x < center_left)
    {
        ROS_INFO("Face is at left");
        cmd.angular.z = tracking_speed;
    }
    else if (x > center_right)
    {
        ROS_INFO("Face is at Right");
        cmd.angular.z = -tracking_speed;
    }
    else if (x > center_left and x < center_right)
    {
        ROS_INFO("Face is at center");
        cmd.angular.z = 0.0;
    }
    pub.publish(cmd);
}

void face_callback(const face_tracker_pkg::centroid::ConstPtr& msg)
{
    ROS_INFO("Received X = [%d], Y = [%d]", msg->x, msg->y);

    // call robot function
    track_face(msg->x, msg->y);
}

int main(int argc, char* argv[])
{
    screenmaxx = 410;
    center_offset = 50;
    tracking_speed = 0.5;

    current_pos_x = 0;

    ros::init(argc, argv, "face_tracker_controller");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/face_centroid", 10, face_callback);
    pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    try
    {
        nh.getParam("screenmaxx", screenmaxx);
        nh.getParam("center_offset", center_offset);
        nh.getParam("tracking_speed", tracking_speed);
        ROS_INFO("%.2f", tracking_speed);

        ROS_INFO("Successfully Loaded robot_tracking parameters");
    }
    catch (int e)
    {
        ROS_ERROR("Parameters are not properly loaded from file, loading defaults");
    }

    center_left = (screenmaxx / 2) - center_offset;
    center_right = (screenmaxx / 2) + center_offset;

    ros::spin();
    return 0;
}