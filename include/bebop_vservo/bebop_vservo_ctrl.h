#ifndef BEBOP_VSERVO_CTRL_H
#define BEBOP_VSERVO_CTRL_H

// boost
#include <boost/shared_ptr.hpp>

// ros
#include <ros/node_handle.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/RegionOfInterest.h>
#include <geometry_msgs/Twist.h>
#include <image_geometry/pinhole_camera_model.h>
#include <std_msgs/Bool.h>

#include "bebop_msgs/Ardrone3CameraStateOrientation.h"

// visp && visp_bridge
#include <visp/vpServo.h>
#include <visp/vpFeaturePoint.h>
#include <visp/vpCameraParameters.h>


// bebop_vservo
#include "bebop_vservo/debug.h"

#ifndef CLAMP
#define CLAMP(x, l, h) (((x) > (h)) ? (h) : (((x) < (l)) ? (l) : (x)))
#endif

namespace bebop_vservo
{
namespace util {

inline void ResetCmdVel(geometry_msgs::Twist& v)
{
  v.linear.x = 0.0;
  v.linear.y = 0.0;
  v.linear.z = 0.0;
  v.angular.x = 0.0;
  v.angular.y = 0.0;
  v.angular.z = 0.0;
}

template<typename T>
bool GetParam(const ros::NodeHandle& nh, const::std::string& key, T& val)
{
  if (nh.getParam(key, val))
  {
    ROS_INFO_STREAM("[VSER] Param " << key << " : " << val);
    return true;
  }
  ROS_WARN_STREAM("[VSER] Param " << key << " not found/set.");
  return false;
}

template<typename T>
bool GetParam(const ros::NodeHandle& nh, const::std::string& key, T& val, const T& default_val)
{
  nh.param(key, val, default_val);
  ROS_INFO_STREAM("[VSER] Param " << key << " : " << val);
}

}  // namespace util

class BebopVServoCtrl
{
public:
  explicit BebopVServoCtrl(ros::NodeHandle &nh);
  virtual void Spin();

private:
  std_msgs::BoolConstPtr enabled_ptr_;
  bool enabled_;
  bool servo_inited;

  ros::NodeHandle nh_;
  ros::NodeHandle nh_priv_;

  ros::Subscriber caminfo_sub;
  ros::Subscriber enable_sub;
  ros::Subscriber roi_sub;
  ros::Subscriber cam_orientation_sub;

  ros::Publisher debug_pub;
  ros::Publisher cmd_vel_pub;

  geometry_msgs::Twist cmd_vel;
  bebop_vservo::debug debug_msg;
  sensor_msgs::RegionOfInterestConstPtr roi_cptr_;

  // Internal
  ros::Time roi_recv_time;
  double fov_x;
  double fov_y;
  image_geometry::PinholeCameraModel cam_model;
  double cam_tilt_rad;

  // Visp
  vpServo task;
  vpFeaturePoint pd[4];
  vpFeaturePoint p[4];
  vpCameraParameters cam;
  double Z, Zd;
  double lambda;
  double t_start_loop;
  double tinit;
  vpColVector v;
  vpColVector v_beb;
  vpColVector vi;
  double mu;
  vpAdaptiveGain lambda_adapt;

  // params
  double depth;
  double height_target_m;
  double distground_target_m;
  double update_freq;

  void Reset();
  void Update();

  void CameraOrientationCallback(const bebop_msgs::Ardrone3CameraStateOrientationConstPtr& cam_ori_ptr);
  void CameraCallback(const sensor_msgs::CameraInfoConstPtr& cinfo_msg_ptr);
  void RoiCallback(const sensor_msgs::RegionOfInterestConstPtr& roi_msg_ptr);
  void EnableCallback(const std_msgs::BoolConstPtr& enable_msg_ptr);

  void UpdateParams();
};

}  // namespace bebop_vservo
#endif