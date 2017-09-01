#ifndef _OUTPUTS_H_
#define _OUTPUTS_H_

#include <stdbool.h>

int init_outputs();
void play_sequence(const int *seq);
void set_led(bool on);

#endif
