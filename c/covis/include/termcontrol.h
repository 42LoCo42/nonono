#ifndef _TERMCONTROL_H
#define _TERMCONTROL_H

extern struct termios settings;

int tctl_load();
int tctl_apply();

void tctl_reset();
void tctl_enable_raw_term();
void tctl_set_vtime(int vtime);

#endif // _TERMCONTROL_H
