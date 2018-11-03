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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/input.h>
#include <ev.h>

#include "outputs.h"


static int spkr_fd;
static ev_timer beep_timeout_watcher;


void set_led(bool on)
{
    const char *led_dev_name = "/sys/class/leds/apu2:green:2/brightness";
    int led_fd = open(led_dev_name, O_WRONLY);

    if (led_fd != -1) {
        if (on) {
            write(led_fd, "255", 3);
        }
        else {
            write(led_fd, "0", 1);
        }

        close(led_fd);
    }
    else {
        perror(led_dev_name);
    }
}


static void open_pcspkr()
{
    const char *spkr_dev_name = "/dev/input/by-path/platform-pcspkr-event-spkr";
    spkr_fd = open(spkr_dev_name, O_WRONLY);

    if (spkr_fd == -1) {
        perror(spkr_dev_name);
    }
}


static void close_pcspkr()
{
    close(spkr_fd);
    spkr_fd = -1;
}


static void beep(int freq)
{
    struct input_event ev;

    memset(&ev, 0, sizeof ev);
    ev.type = EV_SND;
    ev.code = SND_TONE;
    ev.value = freq;

    write(spkr_fd, &ev, sizeof ev);
}


void play_sequence(const int *seq)
{
    if (spkr_fd == -1) {
        open_pcspkr();
    }

    beep(*seq);

    if (*seq == 0) {
        ev_timer_stop(EV_DEFAULT_ &beep_timeout_watcher);
        close_pcspkr();
    }
    else {
        beep_timeout_watcher.data = (void *)(seq + 1);
        if (!ev_is_active(&beep_timeout_watcher)) {
            ev_timer_again(EV_DEFAULT_ &beep_timeout_watcher);
        }
    }
}


static void beep_cb(EV_P_ ev_timer *w, int revents)
{
    const int *seq = w->data;
    play_sequence(seq);
}


int init_outputs()
{
    spkr_fd = -1;
    ev_timer_init(&beep_timeout_watcher, beep_cb, 0.0, 0.3);
    set_led(false);

    return 1;
}
