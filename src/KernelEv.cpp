
#include "KernelEv.h"
#include "Kernel.h"
#include "IVTEntry.h"
#include "Sch1.h"
#include <iostream.h>
#include "operator.h"

KernelEv::KernelEv(unsigned char i)
{
	creator=Kernel::running;
	ulaz=i;
	v=0;
	if (IVTEntry::events[i]==NULL) IVTEntry::events[i]=this;
	else {
		Kernel::error("ERROR: pokusaj dodavanja popunjenog ulaza u ivt tabelu");

	}
};


void KernelEv::signal()
{
	Kernel::lock();
	if (v==1)
	{
		v=0;
		Scheduler1::put(creator);
	}
	Kernel::unlock();
};
extern PCB* event_queue[256];
extern int event_queue_not_empty;
extern int event_queue_not_empty_2[2];
extern int event_queue_not_empty_4[4];
extern int event_queue_not_empty_8[8];
extern int event_queue_not_empty_16[16];
extern int event_queue_not_empty_32[32];
extern int event_queue_not_empty_64[64];
extern int event_queue_not_empty_128[128];
extern int event_queue_not_empty_256[256];
void KernelEv::nocontextsignal()
{
	if (v==1)
	{
		v=0;
		event_queue[ulaz]=creator;
		event_queue_not_empty_256[ulaz]=1;
		event_queue_not_empty_128[ulaz/2]=1;
		event_queue_not_empty_64[ulaz/4]=1;
		event_queue_not_empty_32[ulaz/8]=1;
		event_queue_not_empty_16[ulaz/16]=1;
		event_queue_not_empty_8[ulaz/32]=1;
		event_queue_not_empty_4[ulaz/64]=1;
		event_queue_not_empty_2[ulaz/128]=1;
		event_queue_not_empty=1;
		/*
		ideja je da prvo postavi vrednost PCB-a svog cvora pa onda da pravi "put" od svog cvora ka glavi postavljajuci
		cvorove na 1 (1 oznacava da se ispod njega nalazi neubaceni element u scheduler, a nula suprotno), tako da
		kada se bude pozivala metoda timer u kernelu prvo proverava da li je glava postavljena na 1 pa tek onda proverava njenu decu,
		na kraju dolazi do pcb-a koji treba da se ubaci u scheduler. Kada u metodi timer "putuje" ka cvorovima deci metoda timer 
		postavlja vrednost glave na 0 ukoliko je jednaka 1 i sve ostale cvorove koji su na putu do PCB-a u obrnutom redosledu 
		tako je obezbedjena konkurentnost ovog sistema za ubacivanje u stablo i efikasnost jer se svakih 55ms proverava da li je 
		nesto ubaceno ili ne.
		
		*/
	}
};

void KernelEv::wait()
{
	Kernel::lock();
	if (Kernel::running==creator)
	{
		v=1;
		Kernel::block();

	}
	else
	{
		Kernel::unlock();

	}
};
KernelEv::~KernelEv()
{
	IVTEntry::events[ulaz]=NULL;
}
