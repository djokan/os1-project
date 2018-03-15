#ifndef CPP_IDLET_H_
#define CPP_IDLET_H_

#include "Thread.h"

class Thread;
class IdleThread: public Thread {
protected:
	void run() ;
public:
	IdleThread();
	~IdleThread();
};



#endif
