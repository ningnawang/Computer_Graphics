# Install script for directory: /afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/build/CMU462/src/libCMU462.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/CMU462" TYPE FILE FILES
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/CMU462.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/vector2D.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/vector3D.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/vector4D.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/matrix3x3.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/matrix4x4.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/quaternion.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/complex.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/color.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/osdtext.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/viewer.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/base64.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/tinyxml2.h"
    "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/CMU462/src/renderer.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

