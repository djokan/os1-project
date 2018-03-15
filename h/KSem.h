#ifndef _SPCBLIST_H_
#define _SPCBLIST_H_

#include "SPCBElem.h"


class KernelSem {
public:
	SemaphorePCBElem* glava;
	SemaphorePCBElem* rep;
	int v;

	static SemaphorePCBElem* tglava;
	static unsigned int vreme;
	static unsigned int contextvreme;
	int wait (unsigned int maxTimeToWait);
	void put(PCB* ,unsigned int );
	void signal();
	void ispis();
	static void ispis1();
	static void tick();
	static void ticknocontext();
	KernelSem(int);
	~KernelSem();
};

#endif
