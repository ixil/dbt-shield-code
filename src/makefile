ARDUINO_DIR = /usr/share/arduino
ARDMK_DIR = /usr/share/arduino
AVR_TOOLS_DIR = /usr
AVRDUDE_CONF = /etc/avrdude.conf
ARDUINO_CORE_PATH = /usr/share/arduino/hardware/archlinux-arduino/avr/cores/arduino
ARDUINO_PLATFORM_LIB_PATH = /usr/share/arduino/hardware/archlinux-arduino/avr/libraries
BOARDS_TXT = /usr/share/arduino/hardware/archlinux-arduino/avr/boards.txt
ARDUINO_VAR_PATH = /usr/share/arduino/hardware/archlinux-arduino/avr/variants
BOOTLOADER_PARENT = /usr/share/arduino/hardware/archlinux-arduino/avr/bootloaders

MONITOR_CMD = picocom

PROJECT_DIR       = $(shell dirname $(shell pwd))

ARCHITECTURE  = avr
BOARD_TAG    = mega
BOARD_SUB    = atmega2560
MONITOR_PORT  = /dev/ttyUSB*
# MONITOR_PORT  = /dev/ttyACM*
MONITOR_BAUDRATE  = 230400

ARDUINO_LIBS = AutoPID TMCStepper  SPI SoftwareSerial TimerInterrupt

CFLAGS_STD = -std=gnu11
CXXFLAGS_STD += -std=gnu++11
CURRENT_DIR       = $(shell basename $(CURDIR))
OBJDIR = $(PROJECT_DIR)/build/$(CURRENT_DIR)/$(BOARD_TAG)
CXXFLAGS +=  -Wextra -Wall -pedantic
LDFLAGS +=  -fdiagnostics-color

include $(ARDMK_DIR)/Arduino.mk
