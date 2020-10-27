/*
  This file is part of the Arduino Outdoor Carrier library.
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include <Arduino.h>
#include "EdgeControl_Expander.h"

enum : pin_size_t {
    LATCHING_OUT_1 = 0,
    LATCHING_OUT_2,
    LATCHING_OUT_3,
    LATCHING_OUT_4,
    LATCHING_OUT_5,
    LATCHING_OUT_6,
    LATCHING_OUT_7,
    LATCHING_OUT_8,

    LATCHING_CMD_1 = 8,
    LATCHING_CMD_2,
    LATCHING_CMD_3,
    LATCHING_CMD_4,
    LATCHING_CMD_5,
    LATCHING_CMD_6,
    LATCHING_CMD_7,
    LATCHING_CMD_8,
};

enum PulseDirection {
    POSITIVE,
    NEGATIVE
};

class EdgeControl_LatchingClass {
public:
    EdgeControl_LatchingClass() = default;
    ~EdgeControl_LatchingClass() = default;

    void begin();
    void end();

    void channelDirection(pin_size_t channel, PulseDirection dir);
    void latch();
    void release();
    void strobe(unsigned int duration);

private:
    bool selectSensor(pin_size_t channel);
    void digitalWrite(pin_size_t pin, PinStatus val);

    pin_size_t _channel;
};

extern EdgeControl_LatchingClass Latching;