robotexecdir = $(RTB_DIR)/Robots
INCLUDES = -I$(top_srcdir)/include
if linuxjoystickavailable
robotexec_PROGRAMS = joypad_robot.robot joypad_robot_analog.robot
endif
joypad_robot_robot_SOURCES = joypad_robot.c
joypad_robot_analog_robot_SOURCES = joypad_robot_analog.c
joypaddir = $(RTB_DIR)/Robots/joypad_robot
joypad_DATA = $(joypad_robot_robot_SOURCES) $(joypad_robot_analog_robot_SOURCES) Makefile.am README

