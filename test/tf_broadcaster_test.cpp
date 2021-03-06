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
 * @file tf_broadcaster_test.cpp
 * @author Siddharth (stelang@umd.edu)
 * @brief
 * @version 0.1
 * @date 2021-11-15
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <gtest/gtest.h>
#include <tf/transform_listener.h>
#include <beginner__tutorials/talker.hpp>

std::shared_ptr<ros::NodeHandle> nh;

TEST(TESTSuite, tf_broadcaster) {
  std::string platform_name, parent_frame, child_frame;

  double x_expect{}, y_expect{}, z_expect{};
  double roll_expect{}, pitch_expect{}, yaw_expect{};

  double x_actual{}, y_actual{}, z_actual{};
  double qx_actual{}, qy_actual{}, qz_actual{}, qw_actual{};
  double roll_actual{}, pitch_actual{}, yaw_actual{};

  bool tf_exists {true};

  // get the values from parameter server
  ros::param::get("/talker/parent_frame", parent_frame);
  ros::param::get("/talker/child_frame", child_frame);
  ros::param::get("/talker/x", x_expect);
  ros::param::get("/talker/y", y_expect);
  ros::param::get("/talker/z", z_expect);
  ros::param::get("/talker/roll", roll_expect);
  ros::param::get("/talker/pitch", pitch_expect);
  ros::param::get("/talker/yaw", yaw_expect);

  tf::TransformListener listener;
  tf::StampedTransform transform;

  // Sleep till the TF broadcasts
  ros::Duration(2).sleep();

  try {
      listener.lookupTransform(parent_frame, child_frame,
                                ros::Time(0), transform);
      tf_exists = true;
  } catch (tf::TransformException ex) {
      tf_exists = false;
      ROS_ERROR("%s", ex.what());
      ros::Duration(1.0).sleep();
  }
    // get the actual values
    x_actual = transform.getOrigin().getX();
    y_actual = transform.getOrigin().getY();
    z_actual = transform.getOrigin().getZ();

    qx_actual = transform.getRotation().getX();
    qy_actual = transform.getRotation().getY();
    qz_actual = transform.getRotation().getZ();
    qw_actual = transform.getRotation().getW();

    tf::Quaternion q(qx_actual, qy_actual, qz_actual, qw_actual);
    tf::Matrix3x3 m(q);
    m.getRPY(roll_actual, pitch_actual, yaw_actual);

    EXPECT_TRUE(tf_exists);
    EXPECT_NEAR(x_actual, x_expect, 0.02);
    EXPECT_NEAR(y_actual, y_expect, 0.02);
    EXPECT_NEAR(z_actual, z_expect, 0.02);
    EXPECT_NEAR(roll_actual, roll_expect, 0.02);
    EXPECT_NEAR(pitch_actual, pitch_expect, 0.02);
    EXPECT_NEAR(yaw_actual, yaw_expect, 0.02);
}

int main(int argc,
         char **argv) {
  ros::init(argc, argv, "test_tf_node");
  nh.reset(new ros::NodeHandle);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

