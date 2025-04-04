// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from scara_msgs:action/ScaraTask.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "scara_msgs/action/scara_task.h"


#ifndef SCARA_MSGS__ACTION__DETAIL__SCARA_TASK__STRUCT_H_
#define SCARA_MSGS__ACTION__DETAIL__SCARA_TASK__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_Goal
{
  int32_t task_number;
} scara_msgs__action__ScaraTask_Goal;

// Struct for a sequence of scara_msgs__action__ScaraTask_Goal.
typedef struct scara_msgs__action__ScaraTask_Goal__Sequence
{
  scara_msgs__action__ScaraTask_Goal * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_Goal__Sequence;

// Constants defined in the message

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_Result
{
  bool success;
} scara_msgs__action__ScaraTask_Result;

// Struct for a sequence of scara_msgs__action__ScaraTask_Result.
typedef struct scara_msgs__action__ScaraTask_Result__Sequence
{
  scara_msgs__action__ScaraTask_Result * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_Result__Sequence;

// Constants defined in the message

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_Feedback
{
  int32_t percentage;
} scara_msgs__action__ScaraTask_Feedback;

// Struct for a sequence of scara_msgs__action__ScaraTask_Feedback.
typedef struct scara_msgs__action__ScaraTask_Feedback__Sequence
{
  scara_msgs__action__ScaraTask_Feedback * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_Feedback__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
#include "unique_identifier_msgs/msg/detail/uuid__struct.h"
// Member 'goal'
#include "scara_msgs/action/detail/scara_task__struct.h"

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_SendGoal_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
  scara_msgs__action__ScaraTask_Goal goal;
} scara_msgs__action__ScaraTask_SendGoal_Request;

// Struct for a sequence of scara_msgs__action__ScaraTask_SendGoal_Request.
typedef struct scara_msgs__action__ScaraTask_SendGoal_Request__Sequence
{
  scara_msgs__action__ScaraTask_SendGoal_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_SendGoal_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_SendGoal_Response
{
  bool accepted;
  builtin_interfaces__msg__Time stamp;
} scara_msgs__action__ScaraTask_SendGoal_Response;

// Struct for a sequence of scara_msgs__action__ScaraTask_SendGoal_Response.
typedef struct scara_msgs__action__ScaraTask_SendGoal_Response__Sequence
{
  scara_msgs__action__ScaraTask_SendGoal_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_SendGoal_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  scara_msgs__action__ScaraTask_SendGoal_Event__request__MAX_SIZE = 1
};
// response
enum
{
  scara_msgs__action__ScaraTask_SendGoal_Event__response__MAX_SIZE = 1
};

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_SendGoal_Event
{
  service_msgs__msg__ServiceEventInfo info;
  scara_msgs__action__ScaraTask_SendGoal_Request__Sequence request;
  scara_msgs__action__ScaraTask_SendGoal_Response__Sequence response;
} scara_msgs__action__ScaraTask_SendGoal_Event;

// Struct for a sequence of scara_msgs__action__ScaraTask_SendGoal_Event.
typedef struct scara_msgs__action__ScaraTask_SendGoal_Event__Sequence
{
  scara_msgs__action__ScaraTask_SendGoal_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_SendGoal_Event__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.h"

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_GetResult_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
} scara_msgs__action__ScaraTask_GetResult_Request;

// Struct for a sequence of scara_msgs__action__ScaraTask_GetResult_Request.
typedef struct scara_msgs__action__ScaraTask_GetResult_Request__Sequence
{
  scara_msgs__action__ScaraTask_GetResult_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_GetResult_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'result'
// already included above
// #include "scara_msgs/action/detail/scara_task__struct.h"

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_GetResult_Response
{
  int8_t status;
  scara_msgs__action__ScaraTask_Result result;
} scara_msgs__action__ScaraTask_GetResult_Response;

// Struct for a sequence of scara_msgs__action__ScaraTask_GetResult_Response.
typedef struct scara_msgs__action__ScaraTask_GetResult_Response__Sequence
{
  scara_msgs__action__ScaraTask_GetResult_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_GetResult_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
// already included above
// #include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  scara_msgs__action__ScaraTask_GetResult_Event__request__MAX_SIZE = 1
};
// response
enum
{
  scara_msgs__action__ScaraTask_GetResult_Event__response__MAX_SIZE = 1
};

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_GetResult_Event
{
  service_msgs__msg__ServiceEventInfo info;
  scara_msgs__action__ScaraTask_GetResult_Request__Sequence request;
  scara_msgs__action__ScaraTask_GetResult_Response__Sequence response;
} scara_msgs__action__ScaraTask_GetResult_Event;

// Struct for a sequence of scara_msgs__action__ScaraTask_GetResult_Event.
typedef struct scara_msgs__action__ScaraTask_GetResult_Event__Sequence
{
  scara_msgs__action__ScaraTask_GetResult_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_GetResult_Event__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.h"
// Member 'feedback'
// already included above
// #include "scara_msgs/action/detail/scara_task__struct.h"

/// Struct defined in action/ScaraTask in the package scara_msgs.
typedef struct scara_msgs__action__ScaraTask_FeedbackMessage
{
  unique_identifier_msgs__msg__UUID goal_id;
  scara_msgs__action__ScaraTask_Feedback feedback;
} scara_msgs__action__ScaraTask_FeedbackMessage;

// Struct for a sequence of scara_msgs__action__ScaraTask_FeedbackMessage.
typedef struct scara_msgs__action__ScaraTask_FeedbackMessage__Sequence
{
  scara_msgs__action__ScaraTask_FeedbackMessage * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} scara_msgs__action__ScaraTask_FeedbackMessage__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SCARA_MSGS__ACTION__DETAIL__SCARA_TASK__STRUCT_H_
