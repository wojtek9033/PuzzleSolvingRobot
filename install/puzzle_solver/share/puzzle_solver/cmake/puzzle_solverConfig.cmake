# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_puzzle_solver_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED puzzle_solver_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(puzzle_solver_FOUND FALSE)
  elseif(NOT puzzle_solver_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(puzzle_solver_FOUND FALSE)
  endif()
  return()
endif()
set(_puzzle_solver_CONFIG_INCLUDED TRUE)

# output package information
if(NOT puzzle_solver_FIND_QUIETLY)
  message(STATUS "Found puzzle_solver: 0.0.0 (${puzzle_solver_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'puzzle_solver' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT puzzle_solver_DEPRECATED_QUIET)
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(puzzle_solver_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${puzzle_solver_DIR}/${_extra}")
endforeach()
