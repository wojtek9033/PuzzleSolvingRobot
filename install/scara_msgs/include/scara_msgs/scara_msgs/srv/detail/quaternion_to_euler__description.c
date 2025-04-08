// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from scara_msgs:srv/QuaternionToEuler.idl
// generated code does not contain a copyright notice

#include "scara_msgs/srv/detail/quaternion_to_euler__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_scara_msgs
const rosidl_type_hash_t *
scara_msgs__srv__QuaternionToEuler__get_type_hash(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x53, 0xfd, 0xb4, 0x54, 0xd5, 0x4a, 0x9b, 0xa9,
      0xd3, 0x51, 0x1c, 0xb3, 0x48, 0xa9, 0xf7, 0x5c,
      0x7a, 0x27, 0x3d, 0xe6, 0x3b, 0xce, 0xe3, 0x2f,
      0xde, 0x40, 0x9c, 0x70, 0x30, 0x77, 0x82, 0xfd,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_scara_msgs
const rosidl_type_hash_t *
scara_msgs__srv__QuaternionToEuler_Request__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0x77, 0x1f, 0x98, 0xd5, 0x00, 0x42, 0x8d, 0x8b,
      0x5e, 0x51, 0x7f, 0x9a, 0x34, 0x2a, 0xf7, 0x6f,
      0x39, 0xa5, 0xd5, 0x40, 0x92, 0x47, 0xe3, 0xc5,
      0x7c, 0x8a, 0xce, 0x37, 0xb2, 0x70, 0x7f, 0x61,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_scara_msgs
const rosidl_type_hash_t *
scara_msgs__srv__QuaternionToEuler_Response__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xba, 0xa5, 0xac, 0x75, 0xd5, 0x54, 0xb6, 0x5b,
      0xde, 0xf7, 0x2f, 0xa5, 0x6b, 0x7b, 0x72, 0x19,
      0x08, 0xb3, 0x16, 0xde, 0x27, 0x13, 0xbd, 0x71,
      0x52, 0xf5, 0xd9, 0x8c, 0x68, 0xdf, 0x2c, 0xd5,
    }};
  return &hash;
}

ROSIDL_GENERATOR_C_PUBLIC_scara_msgs
const rosidl_type_hash_t *
scara_msgs__srv__QuaternionToEuler_Event__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xfc, 0x2d, 0xb3, 0x10, 0x93, 0xa0, 0x97, 0xb5,
      0xc8, 0x10, 0xe7, 0x9b, 0xf0, 0x11, 0xd9, 0x3e,
      0xe1, 0xda, 0x79, 0x0e, 0xe1, 0x6b, 0xb0, 0x03,
      0x83, 0x70, 0xf3, 0x16, 0x8d, 0x86, 0xb1, 0xc8,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types
#include "service_msgs/msg/detail/service_event_info__functions.h"
#include "builtin_interfaces/msg/detail/time__functions.h"

// Hashes for external referenced types
#ifndef NDEBUG
static const rosidl_type_hash_t builtin_interfaces__msg__Time__EXPECTED_HASH = {1, {
    0xb1, 0x06, 0x23, 0x5e, 0x25, 0xa4, 0xc5, 0xed,
    0x35, 0x09, 0x8a, 0xa0, 0xa6, 0x1a, 0x3e, 0xe9,
    0xc9, 0xb1, 0x8d, 0x19, 0x7f, 0x39, 0x8b, 0x0e,
    0x42, 0x06, 0xce, 0xa9, 0xac, 0xf9, 0xc1, 0x97,
  }};
static const rosidl_type_hash_t service_msgs__msg__ServiceEventInfo__EXPECTED_HASH = {1, {
    0x41, 0xbc, 0xbb, 0xe0, 0x7a, 0x75, 0xc9, 0xb5,
    0x2b, 0xc9, 0x6b, 0xfd, 0x5c, 0x24, 0xd7, 0xf0,
    0xfc, 0x0a, 0x08, 0xc0, 0xcb, 0x79, 0x21, 0xb3,
    0x37, 0x3c, 0x57, 0x32, 0x34, 0x5a, 0x6f, 0x45,
  }};
#endif

static char scara_msgs__srv__QuaternionToEuler__TYPE_NAME[] = "scara_msgs/srv/QuaternionToEuler";
static char builtin_interfaces__msg__Time__TYPE_NAME[] = "builtin_interfaces/msg/Time";
static char scara_msgs__srv__QuaternionToEuler_Event__TYPE_NAME[] = "scara_msgs/srv/QuaternionToEuler_Event";
static char scara_msgs__srv__QuaternionToEuler_Request__TYPE_NAME[] = "scara_msgs/srv/QuaternionToEuler_Request";
static char scara_msgs__srv__QuaternionToEuler_Response__TYPE_NAME[] = "scara_msgs/srv/QuaternionToEuler_Response";
static char service_msgs__msg__ServiceEventInfo__TYPE_NAME[] = "service_msgs/msg/ServiceEventInfo";

// Define type names, field names, and default values
static char scara_msgs__srv__QuaternionToEuler__FIELD_NAME__request_message[] = "request_message";
static char scara_msgs__srv__QuaternionToEuler__FIELD_NAME__response_message[] = "response_message";
static char scara_msgs__srv__QuaternionToEuler__FIELD_NAME__event_message[] = "event_message";

static rosidl_runtime_c__type_description__Field scara_msgs__srv__QuaternionToEuler__FIELDS[] = {
  {
    {scara_msgs__srv__QuaternionToEuler__FIELD_NAME__request_message, 15, 15},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {scara_msgs__srv__QuaternionToEuler_Request__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler__FIELD_NAME__response_message, 16, 16},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {scara_msgs__srv__QuaternionToEuler_Response__TYPE_NAME, 41, 41},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler__FIELD_NAME__event_message, 13, 13},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {scara_msgs__srv__QuaternionToEuler_Event__TYPE_NAME, 38, 38},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription scara_msgs__srv__QuaternionToEuler__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Event__TYPE_NAME, 38, 38},
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Request__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Response__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
scara_msgs__srv__QuaternionToEuler__get_type_description(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {scara_msgs__srv__QuaternionToEuler__TYPE_NAME, 32, 32},
      {scara_msgs__srv__QuaternionToEuler__FIELDS, 3, 3},
    },
    {scara_msgs__srv__QuaternionToEuler__REFERENCED_TYPE_DESCRIPTIONS, 5, 5},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = scara_msgs__srv__QuaternionToEuler_Event__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = scara_msgs__srv__QuaternionToEuler_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[3].fields = scara_msgs__srv__QuaternionToEuler_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[4].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char scara_msgs__srv__QuaternionToEuler_Request__FIELD_NAME__x[] = "x";
static char scara_msgs__srv__QuaternionToEuler_Request__FIELD_NAME__y[] = "y";
static char scara_msgs__srv__QuaternionToEuler_Request__FIELD_NAME__z[] = "z";
static char scara_msgs__srv__QuaternionToEuler_Request__FIELD_NAME__w[] = "w";

static rosidl_runtime_c__type_description__Field scara_msgs__srv__QuaternionToEuler_Request__FIELDS[] = {
  {
    {scara_msgs__srv__QuaternionToEuler_Request__FIELD_NAME__x, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Request__FIELD_NAME__y, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Request__FIELD_NAME__z, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Request__FIELD_NAME__w, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
scara_msgs__srv__QuaternionToEuler_Request__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {scara_msgs__srv__QuaternionToEuler_Request__TYPE_NAME, 40, 40},
      {scara_msgs__srv__QuaternionToEuler_Request__FIELDS, 4, 4},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char scara_msgs__srv__QuaternionToEuler_Response__FIELD_NAME__roll[] = "roll";
static char scara_msgs__srv__QuaternionToEuler_Response__FIELD_NAME__pitch[] = "pitch";
static char scara_msgs__srv__QuaternionToEuler_Response__FIELD_NAME__yaw[] = "yaw";

static rosidl_runtime_c__type_description__Field scara_msgs__srv__QuaternionToEuler_Response__FIELDS[] = {
  {
    {scara_msgs__srv__QuaternionToEuler_Response__FIELD_NAME__roll, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Response__FIELD_NAME__pitch, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Response__FIELD_NAME__yaw, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
scara_msgs__srv__QuaternionToEuler_Response__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {scara_msgs__srv__QuaternionToEuler_Response__TYPE_NAME, 41, 41},
      {scara_msgs__srv__QuaternionToEuler_Response__FIELDS, 3, 3},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}
// Define type names, field names, and default values
static char scara_msgs__srv__QuaternionToEuler_Event__FIELD_NAME__info[] = "info";
static char scara_msgs__srv__QuaternionToEuler_Event__FIELD_NAME__request[] = "request";
static char scara_msgs__srv__QuaternionToEuler_Event__FIELD_NAME__response[] = "response";

static rosidl_runtime_c__type_description__Field scara_msgs__srv__QuaternionToEuler_Event__FIELDS[] = {
  {
    {scara_msgs__srv__QuaternionToEuler_Event__FIELD_NAME__info, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE,
      0,
      0,
      {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Event__FIELD_NAME__request, 7, 7},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {scara_msgs__srv__QuaternionToEuler_Request__TYPE_NAME, 40, 40},
    },
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Event__FIELD_NAME__response, 8, 8},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_NESTED_TYPE_BOUNDED_SEQUENCE,
      1,
      0,
      {scara_msgs__srv__QuaternionToEuler_Response__TYPE_NAME, 41, 41},
    },
    {NULL, 0, 0},
  },
};

static rosidl_runtime_c__type_description__IndividualTypeDescription scara_msgs__srv__QuaternionToEuler_Event__REFERENCED_TYPE_DESCRIPTIONS[] = {
  {
    {builtin_interfaces__msg__Time__TYPE_NAME, 27, 27},
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Request__TYPE_NAME, 40, 40},
    {NULL, 0, 0},
  },
  {
    {scara_msgs__srv__QuaternionToEuler_Response__TYPE_NAME, 41, 41},
    {NULL, 0, 0},
  },
  {
    {service_msgs__msg__ServiceEventInfo__TYPE_NAME, 33, 33},
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
scara_msgs__srv__QuaternionToEuler_Event__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {scara_msgs__srv__QuaternionToEuler_Event__TYPE_NAME, 38, 38},
      {scara_msgs__srv__QuaternionToEuler_Event__FIELDS, 3, 3},
    },
    {scara_msgs__srv__QuaternionToEuler_Event__REFERENCED_TYPE_DESCRIPTIONS, 4, 4},
  };
  if (!constructed) {
    assert(0 == memcmp(&builtin_interfaces__msg__Time__EXPECTED_HASH, builtin_interfaces__msg__Time__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[0].fields = builtin_interfaces__msg__Time__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[1].fields = scara_msgs__srv__QuaternionToEuler_Request__get_type_description(NULL)->type_description.fields;
    description.referenced_type_descriptions.data[2].fields = scara_msgs__srv__QuaternionToEuler_Response__get_type_description(NULL)->type_description.fields;
    assert(0 == memcmp(&service_msgs__msg__ServiceEventInfo__EXPECTED_HASH, service_msgs__msg__ServiceEventInfo__get_type_hash(NULL), sizeof(rosidl_type_hash_t)));
    description.referenced_type_descriptions.data[3].fields = service_msgs__msg__ServiceEventInfo__get_type_description(NULL)->type_description.fields;
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "# Request\n"
  "float64 x\n"
  "float64 y\n"
  "float64 z\n"
  "float64 w\n"
  "---\n"
  "# Response\n"
  "float64 roll\n"
  "float64 pitch\n"
  "float64 yaw";

static char srv_encoding[] = "srv";
static char implicit_encoding[] = "implicit";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
scara_msgs__srv__QuaternionToEuler__get_individual_type_description_source(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {scara_msgs__srv__QuaternionToEuler__TYPE_NAME, 32, 32},
    {srv_encoding, 3, 3},
    {toplevel_type_raw_source, 103, 103},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
scara_msgs__srv__QuaternionToEuler_Request__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {scara_msgs__srv__QuaternionToEuler_Request__TYPE_NAME, 40, 40},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
scara_msgs__srv__QuaternionToEuler_Response__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {scara_msgs__srv__QuaternionToEuler_Response__TYPE_NAME, 41, 41},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource *
scara_msgs__srv__QuaternionToEuler_Event__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {scara_msgs__srv__QuaternionToEuler_Event__TYPE_NAME, 38, 38},
    {implicit_encoding, 8, 8},
    {NULL, 0, 0},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
scara_msgs__srv__QuaternionToEuler__get_type_description_sources(
  const rosidl_service_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[6];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 6, 6};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *scara_msgs__srv__QuaternionToEuler__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *scara_msgs__srv__QuaternionToEuler_Event__get_individual_type_description_source(NULL);
    sources[3] = *scara_msgs__srv__QuaternionToEuler_Request__get_individual_type_description_source(NULL);
    sources[4] = *scara_msgs__srv__QuaternionToEuler_Response__get_individual_type_description_source(NULL);
    sources[5] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
scara_msgs__srv__QuaternionToEuler_Request__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *scara_msgs__srv__QuaternionToEuler_Request__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
scara_msgs__srv__QuaternionToEuler_Response__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *scara_msgs__srv__QuaternionToEuler_Response__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
scara_msgs__srv__QuaternionToEuler_Event__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[5];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 5, 5};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *scara_msgs__srv__QuaternionToEuler_Event__get_individual_type_description_source(NULL),
    sources[1] = *builtin_interfaces__msg__Time__get_individual_type_description_source(NULL);
    sources[2] = *scara_msgs__srv__QuaternionToEuler_Request__get_individual_type_description_source(NULL);
    sources[3] = *scara_msgs__srv__QuaternionToEuler_Response__get_individual_type_description_source(NULL);
    sources[4] = *service_msgs__msg__ServiceEventInfo__get_individual_type_description_source(NULL);
    constructed = true;
  }
  return &source_sequence;
}
