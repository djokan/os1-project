
#include "Sch1.h"
#include "PCB.h"
#include "Schedule.h"
#include "Kernel.h"
#include <dos.h>
#include <iostream.h>
#include <stdlib.h>
#include "operator.h"
/*
	Svrha kotiscenja ovog schedulera umesto obicnog je to sto ovaj scheduler vraca idle thread kada nema nicega u scheduleru
	a pozvana je metoda get i ne stavlja idle thread u scheduler kada je pozvana metoda put
*/

//#define DEBUG
int Scheduler1::i=0;
PCB* Scheduler1::put(PCB * a)
{
	Kernel::lock();

	

	if (a!=Kernel::idle && a!=NULL)
	{
#ifdef DEBUG
		cout << "Scheduler put PCB:" << a << ", Thread:" << a->thread << ", ss=" << a->ss << ",sp=" << a->sp << ",locked=" << Kernel::locked << endl;
#endif
		i++;
		Scheduler::put(a);
	}
	Kernel::unlock();
	return a;
};

PCB* Scheduler1::get()
{
	Kernel::lock();

	if (i==0)
	{
#ifdef DEBUG
		cout << "Scheduler get idle " << Kernel::idle->thread << ", ss=" << Kernel::idle->ss << ",sp=" << Kernel::idle->sp << ",locked=" << Kernel::locked << endl;
#endif

		Kernel::unlock();
		return Kernel::idle;
	}
	PCB* a=Scheduler::get();
#ifdef DEBUG
	cout << "Scheduler get " << a->thread  << ", ss=" << a->ss << ",sp=" << a->sp << ",locked=" << Kernel::locked << endl;
#endif
	i--;

	Kernel::unlock();
	return a;
};
