#include "Event.h"
#include "Kernel.h"
#include "KernelEv.h"
#include "operator.h"

Event::Event(IVTNo ivtNo)
{
	Kernel::lock();
	myImpl= new KernelEv(ivtNo);
	Kernel::unlock();
};
Event::~Event ()
{
	Kernel::lock();
	delete myImpl;
	Kernel::unlock();
};
void Event::wait ()
{
	myImpl->wait();
};

void Event::signal()
{
	myImpl->signal();
};
