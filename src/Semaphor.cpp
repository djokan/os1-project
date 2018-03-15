
#include "Semaphor.h"
#include "operator.h"

#include "Ksem.h"
#include "Kernel.h"

Semaphore::Semaphore(int i) {

	Kernel::lock();
	myImpl= new KernelSem(i);
	Kernel::unlock();
}

void Semaphore::signal()
{
	myImpl->signal();
}

int Semaphore::wait(unsigned int maxTimeToWait)
{
	return myImpl->wait(maxTimeToWait);
}

int Semaphore::val() const
{
	return myImpl->v;
}

Semaphore::~Semaphore()
{
	Kernel::lock();
	if (myImpl!=NULL)
	delete myImpl;
	Kernel::unlock();
}
