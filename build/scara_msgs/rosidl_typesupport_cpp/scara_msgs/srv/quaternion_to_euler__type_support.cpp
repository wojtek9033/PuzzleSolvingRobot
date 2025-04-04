// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from scara_msgs:srv/QuaternionToEuler.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "scara_msgs/srv/detail/quaternion_to_euler__functions.h"
#include "scara_msgs/srv/detail/quaternion_to_euler__struct.hpp"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
#include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace scara_msgs
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _QuaternionToEuler_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _QuaternionToEuler_Request_type_support_ids_t;

static const _QuaternionToEuler_Request_type_support_ids_t _QuaternionToEuler_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _QuaternionToEuler_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _QuaternionToEuler_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _QuaternionToEuler_Request_type_support_symbol_names_t _QuaternionToEuler_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, scara_msgs, srv, QuaternionToEuler_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, scara_msgs, srv, QuaternionToEuler_Request)),
  }
};

typedef struct _QuaternionToEuler_Request_type_support_data_t
{
  void * data[2];
} _QuaternionToEuler_Request_type_support_data_t;

static _QuaternionToEuler_Request_type_support_data_t _QuaternionToEuler_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _QuaternionToEuler_Request_message_typesupport_map = {
  2,
  "scara_msgs",
  &_QuaternionToEuler_Request_message_typesupport_ids.typesupport_identifier[0],
  &_QuaternionToEuler_Request_message_typesupport_symbol_names.symbol_name[0],
  &_QuaternionToEuler_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t QuaternionToEuler_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_QuaternionToEuler_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &scara_msgs__srv__QuaternionToEuler_Request__get_type_hash,
  &scara_msgs__srv__QuaternionToEuler_Request__get_type_description,
  &scara_msgs__srv__QuaternionToEuler_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace scara_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Request>()
{
  return &::scara_msgs::srv::rosidl_typesupport_cpp::QuaternionToEuler_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, scara_msgs, srv, QuaternionToEuler_Request)() {
  return get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "scara_msgs/srv/detail/quaternion_to_euler__functions.h"
// already included above
// #include "scara_msgs/srv/detail/quaternion_to_euler__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace scara_msgs
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _QuaternionToEuler_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _QuaternionToEuler_Response_type_support_ids_t;

static const _QuaternionToEuler_Response_type_support_ids_t _QuaternionToEuler_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _QuaternionToEuler_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _QuaternionToEuler_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _QuaternionToEuler_Response_type_support_symbol_names_t _QuaternionToEuler_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, scara_msgs, srv, QuaternionToEuler_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, scara_msgs, srv, QuaternionToEuler_Response)),
  }
};

typedef struct _QuaternionToEuler_Response_type_support_data_t
{
  void * data[2];
} _QuaternionToEuler_Response_type_support_data_t;

static _QuaternionToEuler_Response_type_support_data_t _QuaternionToEuler_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _QuaternionToEuler_Response_message_typesupport_map = {
  2,
  "scara_msgs",
  &_QuaternionToEuler_Response_message_typesupport_ids.typesupport_identifier[0],
  &_QuaternionToEuler_Response_message_typesupport_symbol_names.symbol_name[0],
  &_QuaternionToEuler_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t QuaternionToEuler_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_QuaternionToEuler_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &scara_msgs__srv__QuaternionToEuler_Response__get_type_hash,
  &scara_msgs__srv__QuaternionToEuler_Response__get_type_description,
  &scara_msgs__srv__QuaternionToEuler_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace scara_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Response>()
{
  return &::scara_msgs::srv::rosidl_typesupport_cpp::QuaternionToEuler_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, scara_msgs, srv, QuaternionToEuler_Response)() {
  return get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "scara_msgs/srv/detail/quaternion_to_euler__functions.h"
// already included above
// #include "scara_msgs/srv/detail/quaternion_to_euler__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace scara_msgs
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _QuaternionToEuler_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _QuaternionToEuler_Event_type_support_ids_t;

static const _QuaternionToEuler_Event_type_support_ids_t _QuaternionToEuler_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _QuaternionToEuler_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _QuaternionToEuler_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _QuaternionToEuler_Event_type_support_symbol_names_t _QuaternionToEuler_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, scara_msgs, srv, QuaternionToEuler_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, scara_msgs, srv, QuaternionToEuler_Event)),
  }
};

typedef struct _QuaternionToEuler_Event_type_support_data_t
{
  void * data[2];
} _QuaternionToEuler_Event_type_support_data_t;

static _QuaternionToEuler_Event_type_support_data_t _QuaternionToEuler_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _QuaternionToEuler_Event_message_typesupport_map = {
  2,
  "scara_msgs",
  &_QuaternionToEuler_Event_message_typesupport_ids.typesupport_identifier[0],
  &_QuaternionToEuler_Event_message_typesupport_symbol_names.symbol_name[0],
  &_QuaternionToEuler_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t QuaternionToEuler_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_QuaternionToEuler_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &scara_msgs__srv__QuaternionToEuler_Event__get_type_hash,
  &scara_msgs__srv__QuaternionToEuler_Event__get_type_description,
  &scara_msgs__srv__QuaternionToEuler_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace scara_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Event>()
{
  return &::scara_msgs::srv::rosidl_typesupport_cpp::QuaternionToEuler_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, scara_msgs, srv, QuaternionToEuler_Event)() {
  return get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "scara_msgs/srv/detail/quaternion_to_euler__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace scara_msgs
{

namespace srv
{

namespace rosidl_typesupport_cpp
{

typedef struct _QuaternionToEuler_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _QuaternionToEuler_type_support_ids_t;

static const _QuaternionToEuler_type_support_ids_t _QuaternionToEuler_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _QuaternionToEuler_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _QuaternionToEuler_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _QuaternionToEuler_type_support_symbol_names_t _QuaternionToEuler_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, scara_msgs, srv, QuaternionToEuler)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, scara_msgs, srv, QuaternionToEuler)),
  }
};

typedef struct _QuaternionToEuler_type_support_data_t
{
  void * data[2];
} _QuaternionToEuler_type_support_data_t;

static _QuaternionToEuler_type_support_data_t _QuaternionToEuler_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _QuaternionToEuler_service_typesupport_map = {
  2,
  "scara_msgs",
  &_QuaternionToEuler_service_typesupport_ids.typesupport_identifier[0],
  &_QuaternionToEuler_service_typesupport_symbol_names.symbol_name[0],
  &_QuaternionToEuler_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t QuaternionToEuler_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_QuaternionToEuler_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<scara_msgs::srv::QuaternionToEuler_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<scara_msgs::srv::QuaternionToEuler>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<scara_msgs::srv::QuaternionToEuler>,
  &scara_msgs__srv__QuaternionToEuler__get_type_hash,
  &scara_msgs__srv__QuaternionToEuler__get_type_description,
  &scara_msgs__srv__QuaternionToEuler__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace srv

}  // namespace scara_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<scara_msgs::srv::QuaternionToEuler>()
{
  return &::scara_msgs::srv::rosidl_typesupport_cpp::QuaternionToEuler_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, scara_msgs, srv, QuaternionToEuler)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<scara_msgs::srv::QuaternionToEuler>();
}

#ifdef __cplusplus
}
#endif
