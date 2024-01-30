#ifndef SIGNAL_COMPATIBILITY_H
#define SIGNAL_COMPATIBILITY_H

int sigaction_compat(int sig, const struct sigaction_android *act, struct sigaction_android *oact);

#endif


