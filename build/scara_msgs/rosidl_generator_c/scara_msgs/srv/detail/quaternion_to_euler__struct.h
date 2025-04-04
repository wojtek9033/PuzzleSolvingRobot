// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from scara_msgs:srv/QuaternionToEuler.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "scara_msgs/srv/quaternion_to_euler.h"


#ifndef SCARA_MSGS__SRV__DETAIL__QUATERNION_TO_EULER__STRUCT_H_
#define SCARA_MSGS__SRV__DETAIL__QUATERNION_TO_EULER__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/QuaternionToEuler in the package scara_msgs.
typedef struct scara_msgs__srv__QuaternionToEuler_Request
{
  double x;
  double y;
  double z;
  double w;
} scara_msgs__srv__QuaternionToEuler_Request;

// Struct for a sequence of scara_msgs__srv__QuaternionToEuler_Request.
typedef struct scara_msgs__srv__QuaternionToEuler_Request__Sequence
{
  scara_msgs__srv__QuaternionToEuler_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__srv__QuaternionToEuler_Request__Sequence;

// Constants defined in the message

/// Struct defined in srv/QuaternionToEuler in the package scara_msgs.
typedef struct scara_msgs__srv__QuaternionToEuler_Response
{
  double roll;
  double pitch;
  double yaw;
} scara_msgs__srv__QuaternionToEuler_Response;

// Struct for a sequence of scara_msgs__srv__QuaternionToEuler_Response.
typedef struct scara_msgs__srv__QuaternionToEuler_Response__Sequence
{
  scara_msgs__srv__QuaternionToEuler_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__srv__QuaternionToEuler_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  scara_msgs__srv__QuaternionToEuler_Event__request__MAX_SIZE = 1
};
// response
enum
{
  scara_msgs__srv__QuaternionToEuler_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/QuaternionToEuler in the package scara_msgs.
typedef struct scara_msgs__srv__QuaternionToEuler_Event
{
  service_msgs__msg__ServiceEventInfo info;
  scara_msgs__srv__QuaternionToEuler_Request__Sequence request;
  scara_msgs__srv__QuaternionToEuler_Response__Sequence response;
} scara_msgs__srv__QuaternionToEuler_Event;

// Struct for a sequence of scara_msgs__srv__QuaternionToEuler_Event.
typedef struct scara_msgs__srv__QuaternionToEuler_Event__Sequence
{
  scara_msgs__srv__QuaternionToEuler_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__srv__QuaternionToEuler_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SCARA_MSGS__SRV__DETAIL__QUATERNION_TO_EULER__STRUCT_H_
