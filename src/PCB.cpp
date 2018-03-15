#include "PCB.h"
#include "Kernel.h"
#include "PCBList.h"
#include "IdleT.h"
#include "Sch1.h"
#include <iostream.h>


#include <dos.h>
#include <stdio.h>
#include "TIDTable.h"
#include "operator.h"

//#define DEBUG
int k;

Thread* PCB::main=NULL;
Thread* PCB::idle=NULL;
int PCB::ids=0;


PCB::PCB()
{
	Kernel::lock();
	id=ThreadIdTable::getnewId();
	stanje=0;// 0- nije pokrenuta, 1-radi, 2- zavrsena
	listanacekanju = new PCBList();
	deca= new PCBList();
#ifdef DEBUG
	cout << "createPCB " << this << ",List=" << listanacekanju << endl;
#endif

	Kernel::unlock();
};

PCB* PCB::make_main_thread()
{
	PCB::main=new Thread(0,2);
	//PCB::main->myPCB->me=ThrTree::glava->addchild();

	return PCB::main->myPCB;
};
void PCB::close_main_thread()
{
	delete PCB::main;
};
void PCB::make_idle_thread()
{
	PCB::idle= new IdleThread();
	PCB::idle->start();
};
void PCB::close_idle_thread()
{
	delete PCB::idle;
};


unsigned a5;
void PCB::run(){

#ifdef DEBUG
	Kernel::lock();
	cout << "runthread " << Kernel::running->thread << endl;
	Kernel::unlock();
#endif

	Kernel::running->thread->run();
	Thread::exit();

}

PCB::~PCB(){
	Kernel::lock();
#ifdef DEBUG
	cout << Kernel::running->thread <<  "deleting PCB of thread" << this->thread << endl;
#endif
	if (listanacekanju!=NULL)
	delete listanacekanju;
	if (deca!=NULL)
	delete deca;
	Kernel::unlock();
}

void PCB::clone(PCB* a)
{

	unsigned int p,i= (unsigned int)(((unsigned long)(FP_SEG(a->stack+a->stackSize)-a->ss))*16+(unsigned)FP_OFF(a->stack+a->stackSize)-(unsigned)a->sp);

	stack= new unsigned[stackSize];
	for (unsigned p1=0;p1<i/2;p1++)
	{
		stack[stackSize-1-p1]=a->stack[a->stackSize-1-p1];

	}
	unsigned int i1=Kernel::bparg;
	while ((FP_OFF(a->stack+a->stackSize-1)-i1)/2!=0)
	{
		p=(FP_OFF(a->stack+a->stackSize-1)-i1)/2;
		stack[stackSize-1-p]= FP_OFF(stack+stackSize-1-((FP_OFF(a->stack+a->stackSize-1))-a->stack[a->stackSize-1-p])/2);

		i1=a->stack[a->stackSize-1-p];

	}




	sp=FP_OFF(stack+stackSize-i/2);
	ss=FP_SEG(stack+stackSize-i/2);

	/*for (unsigned p2=0;p2<i/2;p2++)
		{
			cout <<"p" << stack[stackSize-1-p2] << " " << FP_SEG(stack+stackSize-1-p2)  << ":" << FP_OFF(stack+stackSize-1-p2) << "       " << a->stack[a->stackSize-1-p2] << " " << FP_SEG(a->stack+a->stackSize-1-p2) << ":" << FP_OFF(a->stack+a->stackSize-1-p2) << endl;

		}

	cout << ss << ":" << sp << endl;*/

};
