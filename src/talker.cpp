/**
 * MIT License
 *
 * Copyright (c) 2021 Siddharth Telang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file talker.cpp
 * @author Siddharth Telang (stelang@umd.edu)
 * @brief 
 * @version 0.1
 * @date 2021-11-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "../include/beginner__tutorials/talker.hpp"

Talker::Talker(ros::NodeHandle *nh_) {
  nh = nh_;
  init();
  init_params();
  init_service();
}

Talker::~Talker() {
  delete nh;
}

void Talker::init() {
  simple_publisher = nh->advertise<std_msgs::String>("talker", 1000);
  rate = 10;
}

void Talker::init_service() {
  service_name = "modify_Message";
  server = nh->advertiseService(service_name, &Talker::modify, this);
}

void Talker::init_params() {
  nh->param<int>("/talker/publisher_rate", rate, 25);
  nh->param<double>("/talker/x", x, 2.0);
  nh->param<double>("/talker/y", y, 3.0);
  nh->param<double>("/talker/z", z, 1.0);
  nh->param<double>("/talker/roll", roll, 3.14);
  nh->param<double>("/talker/pitch", pitch, -3.14);
  nh->param<double>("/talker/yaw", yaw, 3.14);
  nh->param<std::string>("/talker/parent_frame", parent_frame, "world");
  nh->param<std::string>("/talker/child_frame", child_frame, "talk");
}

bool Talker::modify(beginner__tutorials::modify_Message::Request &req,
                   beginner__tutorials::modify_Message::Response &res) {
  ROS_WARN_STREAM("Service called - modifying the request string");
  res.str2 = req.str1 + " - Service Called Service Successfully";
  return true;
}

void Talker::broadcastTransform() {
  ROS_INFO_STREAM("Broadcasting transform");
  static tf::TransformBroadcaster br;
  tf::Transform transform;
  transform.setOrigin(tf::Vector3(x, y, z));
  tf::Quaternion q;
  q.setRPY(roll, pitch, yaw);
  transform.setRotation(q);
  br.sendTransform(tf::StampedTransform(transform,
                   ros::Time::now(), parent_frame, child_frame));
}


void Talker::startNode() {
  ROS_DEBUG("Starting Talker node");
  int count {};
  ros::Rate loop_rate(rate);
  if (rate != 10) {
    ROS_WARN_STREAM("Loop rate has been modified from 10 Hz to"
                    << rate <<" Hz");
  }
  while (ros::ok()) {
    std_msgs::String msg;
    std::stringstream ss;
    broadcastTransform();
    ss << " Sending HELLO FROM PUBLISHER " << count++;
    msg.data = ss.str();
    ROS_INFO("%s", msg.data.c_str());
    simple_publisher.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
  }
}
