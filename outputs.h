/*
  Copyright (C) 2017 Christof Efkemann.
  This file is part of gw-monitor.

  gw-monitor is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  gw-monitor is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with gw-monitor.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _OUTPUTS_H_
#define _OUTPUTS_H_

#include <stdbool.h>

int init_outputs();
void play_sequence(const int *seq);
void set_led(bool on);

#endif
