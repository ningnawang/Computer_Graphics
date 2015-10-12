# Install script for directory: /afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/src

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
  IF(EXISTS "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/meshedit" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/meshedit")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/meshedit"
         RPATH "")
  ENDIF()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/meshedit")
  IF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
  IF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  ENDIF (CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
FILE(INSTALL DESTINATION "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit" TYPE EXECUTABLE FILES "/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/build/meshedit")
  IF(EXISTS "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/meshedit" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/meshedit")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/afs/andrew.cmu.edu/usr5/ningnaw/Desktop/15662-CG/homeworks/hw2/asst2_meshedit/meshedit")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

