#include "signalhandler.h"
#include <stdio.h>

void signalhandler::deny_signal(int sig)
{
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(sig, &sa, NULL);
}

void signalhandler::set_signal_handler(int sig, sighandler acthandler)
{
	struct sigaction act;
	act.sa_handler = acthandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(sig, &act, NULL);

}
