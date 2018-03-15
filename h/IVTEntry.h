
#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

#include "Kernel.h"
#include "KernelEv.h"
#include "Sch1.h"
#include <iostream.h>
#include "PCB.h"

class KernelEv;
class IVTEntry {


public:
	static KernelEv*  events[256];
	unsigned char ulaz;
	IVTEntry(void (interrupt *)(),unsigned int);
	void (interrupt* old)();
	~IVTEntry();
};





#define PREPAREENTRY(ivt, stara) \
extern IVTEntry Entry##ivt;\
void interrupt int_##ivt()\
{\
	if (Kernel::locked==0)\
	{\
		Kernel::lock();\
		if (IVTEntry::events[ivt]!=NULL) IVTEntry::events[ivt]->signal();\
		if (stara==1) Entry##ivt.old();\
		Kernel::popdispatch();\
	}\
	else\
	{\
		if (IVTEntry::events[ivt]!=NULL) IVTEntry::events[ivt]->nocontextsignal();\
		if (stara==1) Entry##ivt.old();\
	}\
};\
IVTEntry Entry##ivt(int_##ivt,ivt)
/*
Ako kernel nije zakljucan onda normalno poziva signal i staru prekidnu rutinu ukoliko se poziva, i na kraju zove popdispatch
koji se razlikuje od obicnog dispatcha po tome sto ce da skine sa stacka ono sto ce da poziv te metode stavi na stack
jer smo vec u interruptu pa nije potrebno da se vracamo u interrupt vec na nit na kojoj se pozvao interrupt.
Ukoliko je kernel zakljucan onda ne sme da se zove obican signal od eventa koji ce da menja scheduler vec se zove signalnocontext
koji ne menja kontekst vec u posebno staticko stablo ubacuje sboj pcb koji ce kasnije metoda timer da ubaci u scheduler kada ne bude
kernel bio zakljucan. Poziva se stara prekidna rutina ukoliko treba i ovog puta se ne menja kontekst jer ne sme da se koristi Scheduler
koji ne sme da koristi vise niti u isto vreme
*/



#endif
