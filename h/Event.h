
#ifndef _EVENT_H_
#define _EVENT_H_


#include "IVTEntry.h"

typedef unsigned char IVTNo;
class KernelEv;
class Event {
public:
Event (IVTNo ivtNo);
~Event ();
void wait ();
protected:
friend class KernelEv;
void signal(); // can call KernelEv
private:
KernelEv* myImpl;
};

#endif
