
#include "IdleT.h"
#include "Kernel.h"
#include <stdio.h>
#include "TIDTable.h"
#include "operator.h"

IdleThread::IdleThread():Thread(64,1) {

}
void IdleThread::run()
{
	Kernel::lock();
	Kernel::dispatch_i();
	int i,p;
	while (1==1)
	{
		i=i+1;
		p=i+1;
	}
}
IdleThread::~IdleThread() {

	Kernel::running=Kernel::idle;


}
