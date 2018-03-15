#include "Thread.h"
#include "Kernel.h"
#include "TIDTable.h"
#include "PCB.h"
#include <iostream.h>
#include <dos.h>
#include <stdio.h>
#include "PCBList.h"
#include "Sch1.h"

#include "operator.h"
//#define DEBUG

unsigned a3,b3;
extern volatile Thread *tmpthr;
Thread::Thread(StackSize stackSize , Time timeSlice )

{

	Kernel::lock();


	myPCB= new PCB();
	ThreadIdTable::put(myPCB->id,this);
#ifdef DEBUG
	cout << "thread create " << this << ", PCB=" << myPCB << endl;
#endif

	myPCB->thread= this;
	myPCB->kvant= timeSlice;
	myPCB->stackSize=(stackSize+120)/2;
	Kernel::unlock();

}

void Thread::waitToComplete()
{
	Kernel::lock();

#ifdef DEBUG
	cout << Kernel::running->thread << " waiting to " << this << endl;
#endif
	if (myPCB!=Kernel::running && myPCB->stanje!=2)
	{
			myPCB->listanacekanju->put(Kernel::running);
			Kernel::block();
	}
	else
	{
		Kernel::unlock();
	}
}

Thread* Thread::getThreadById(ID id)  //TODO test
{
	return ThreadIdTable::get(id);
}

int Thread::getId()
{
	Kernel::lock();
	int t=myPCB->id;
	Kernel::unlock();
	return t;
}

int Thread::getRunningId()
{
	Kernel::lock();
	int t=Kernel::running->id;
	Kernel::unlock();
	return t;
}

void dispatch()
{
	Kernel::lock();
	Kernel::dispatch(); // TODO test

}
Thread::~Thread() //
{
#ifdef DEBUG
	Kernel::lock();
	cout << "thread delete " << this << endl;
	Kernel::unlock();
#endif
	waitToComplete();
	Kernel::lock();
	ThreadIdTable::put(myPCB->id,NULL); // TODO moze da se cuva ono sto je brisano
	if (myPCB!=NULL)
	delete myPCB;
	Kernel::unlock();
}
void Thread::start()
{
	// TODO optimizacija steka- ne zauzimati sve nego deo po deo
	Kernel::lock();
#ifdef DEBUG
	cout << "thread start " << this << endl;
#endif
	if (myPCB->stanje==0)
	{
		myPCB->stanje=1;
		myPCB->stack=new unsigned[myPCB->stackSize];
		myPCB->stack[myPCB->stackSize-1]=0x200;
		myPCB->stack[myPCB->stackSize-2]=FP_SEG(&PCB::run);
		myPCB->stack[myPCB->stackSize-3]=FP_OFF(&PCB::run);
		myPCB->sp= FP_OFF(myPCB->stack+myPCB->stackSize-12);
		myPCB->ss= FP_SEG(myPCB->stack+myPCB->stackSize-12);
		Kernel::pcbarg=myPCB;
		Kernel::create_thread();
	}
	else
	{
		Kernel::unlock();
	}


}

Thread* tempthread;

ID Thread::fork()
{
	Kernel::lock();
	int i=0;
	unsigned b1=1234;
	asm mov b3,bp
	Kernel::bparg=b3;
	
	asm mov a3,sp
	asm mov b3,ss
	Kernel::running->sp=a3;
	Kernel::running->ss=b3;
	tempthread=Kernel::running->thread->clone();
	if (tempthread==NULL) return -1;
	Kernel::pcbarg=tempthread->myPCB;
	if (Kernel::pcbarg==NULL || Kernel::pcbarg->deca==NULL|| Kernel::pcbarg->listanacekanju==NULL)
	{
		delete tempthread;
		return -1;
	}

	Kernel::fork();


	asm mov bp,b1
	return i;
};

void Thread::waitForForkChildren()
{

	PCB* t;
	Kernel::lock();
	t= Kernel::running->deca->get();
	while (t!=NULL)
	{
		Kernel::unlock();
		t->thread->waitToComplete();
		Kernel::lock();
		t= Kernel::running->deca->get();
	}
	Kernel::unlock();

};
Thread* Thread::clone() const
{

	Thread *t= new Thread((myPCB->stackSize)*2-120,myPCB->kvant);


	t->myPCB->clone(myPCB);

	return t;
}

void Thread::exit()
{
	Kernel::lock();
#ifdef DEBUG
	cout << "thread exit " << Kernel::running->thread <<  endl;
#endif	



	PCB *t = Kernel::running->listanacekanju->get();
	while (t!=NULL)
	{
		Scheduler1::put(t);
		t = Kernel::running->listanacekanju->get();

	}
	Kernel::exit_thread();
}

