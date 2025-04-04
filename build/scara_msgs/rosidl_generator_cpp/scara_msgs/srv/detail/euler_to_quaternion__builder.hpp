// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from scara_msgs:srv/EulerToQuaternion.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "scara_msgs/srv/euler_to_quaternion.hpp"


#ifndef SCARA_MSGS__SRV__DETAIL__EULER_TO_QUATERNION__BUILDER_HPP_
#define SCARA_MSGS__SRV__DETAIL__EULER_TO_QUATERNION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "scara_msgs/srv/detail/euler_to_quaternion__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace scara_msgs
{

namespace srv
{

namespace builder
{

class Init_EulerToQuaternion_Request_yaw
{
public:
  explicit Init_EulerToQuaternion_Request_yaw(::scara_msgs::srv::EulerToQuaternion_Request & msg)
  : msg_(msg)
  {}
  ::scara_msgs::srv::EulerToQuaternion_Request yaw(::scara_msgs::srv::EulerToQuaternion_Request::_yaw_type arg)
  {
    msg_.yaw = std::move(arg);
    return std::move(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Request msg_;
};

class Init_EulerToQuaternion_Request_pitch
{
public:
  explicit Init_EulerToQuaternion_Request_pitch(::scara_msgs::srv::EulerToQuaternion_Request & msg)
  : msg_(msg)
  {}
  Init_EulerToQuaternion_Request_yaw pitch(::scara_msgs::srv::EulerToQuaternion_Request::_pitch_type arg)
  {
    msg_.pitch = std::move(arg);
    return Init_EulerToQuaternion_Request_yaw(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Request msg_;
};

class Init_EulerToQuaternion_Request_roll
{
public:
  Init_EulerToQuaternion_Request_roll()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_EulerToQuaternion_Request_pitch roll(::scara_msgs::srv::EulerToQuaternion_Request::_roll_type arg)
  {
    msg_.roll = std::move(arg);
    return Init_EulerToQuaternion_Request_pitch(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Request msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::scara_msgs::srv::EulerToQuaternion_Request>()
{
  return scara_msgs::srv::builder::Init_EulerToQuaternion_Request_roll();
}

}  // namespace scara_msgs


namespace scara_msgs
{

namespace srv
{

namespace builder
{

class Init_EulerToQuaternion_Response_w
{
public:
  explicit Init_EulerToQuaternion_Response_w(::scara_msgs::srv::EulerToQuaternion_Response & msg)
  : msg_(msg)
  {}
  ::scara_msgs::srv::EulerToQuaternion_Response w(::scara_msgs::srv::EulerToQuaternion_Response::_w_type arg)
  {
    msg_.w = std::move(arg);
    return std::move(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Response msg_;
};

class Init_EulerToQuaternion_Response_z
{
public:
  explicit Init_EulerToQuaternion_Response_z(::scara_msgs::srv::EulerToQuaternion_Response & msg)
  : msg_(msg)
  {}
  Init_EulerToQuaternion_Response_w z(::scara_msgs::srv::EulerToQuaternion_Response::_z_type arg)
  {
    msg_.z = std::move(arg);
    return Init_EulerToQuaternion_Response_w(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Response msg_;
};

class Init_EulerToQuaternion_Response_y
{
public:
  explicit Init_EulerToQuaternion_Response_y(::scara_msgs::srv::EulerToQuaternion_Response & msg)
  : msg_(msg)
  {}
  Init_EulerToQuaternion_Response_z y(::scara_msgs::srv::EulerToQuaternion_Response::_y_type arg)
  {
    msg_.y = std::move(arg);
    return Init_EulerToQuaternion_Response_z(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Response msg_;
};

class Init_EulerToQuaternion_Response_x
{
public:
  Init_EulerToQuaternion_Response_x()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_EulerToQuaternion_Response_y x(::scara_msgs::srv::EulerToQuaternion_Response::_x_type arg)
  {
    msg_.x = std::move(arg);
    return Init_EulerToQuaternion_Response_y(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Response msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::scara_msgs::srv::EulerToQuaternion_Response>()
{
  return scara_msgs::srv::builder::Init_EulerToQuaternion_Response_x();
}

}  // namespace scara_msgs


namespace scara_msgs
{

namespace srv
{

namespace builder
{

class Init_EulerToQuaternion_Event_response
{
public:
  explicit Init_EulerToQuaternion_Event_response(::scara_msgs::srv::EulerToQuaternion_Event & msg)
  : msg_(msg)
  {}
  ::scara_msgs::srv::EulerToQuaternion_Event response(::scara_msgs::srv::EulerToQuaternion_Event::_response_type arg)
  {
    msg_.response = std::move(arg);
    return std::move(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Event msg_;
};

class Init_EulerToQuaternion_Event_request
{
public:
  explicit Init_EulerToQuaternion_Event_request(::scara_msgs::srv::EulerToQuaternion_Event & msg)
  : msg_(msg)
  {}
  Init_EulerToQuaternion_Event_response request(::scara_msgs::srv::EulerToQuaternion_Event::_request_type arg)
  {
    msg_.request = std::move(arg);
    return Init_EulerToQuaternion_Event_response(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Event msg_;
};

class Init_EulerToQuaternion_Event_info
{
public:
  Init_EulerToQuaternion_Event_info()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_EulerToQuaternion_Event_request info(::scara_msgs::srv::EulerToQuaternion_Event::_info_type arg)
  {
    msg_.info = std::move(arg);
    return Init_EulerToQuaternion_Event_request(msg_);
  }

private:
  ::scara_msgs::srv::EulerToQuaternion_Event msg_;
};

}  // namespace builder

}  // namespace srv

template<typename MessageType>
auto build();

template<>
inline
auto build<::scara_msgs::srv::EulerToQuaternion_Event>()
{
  return scara_msgs::srv::builder::Init_EulerToQuaternion_Event_info();
}

}  // namespace scara_msgs

#endif  // SCARA_MSGS__SRV__DETAIL__EULER_TO_QUATERNION__BUILDER_HPP_
