#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H
#include <signal.h> 

typedef void(*sighandler)(int);

class signalhandler
{
public:
	~signalhandler(){};
	static void deny_signal(int sig);
	static void set_signal_handler(int sig, sighandler acthandler);
private:
	signalhandler(){}
};

#endif