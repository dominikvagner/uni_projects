# Author: Tomáš Polívka

CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

IF(UNIX)
    SET(CMAKE_C_COMPILER gcc)
    SET(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -g")
ENDIF()

PROJECT(IFJ)

FILE(GLOB SOURCES "*.c")
FILE(GLOB HEADERS "*.h")

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${HEADERS})