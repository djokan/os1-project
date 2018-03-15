#ifndef _SPCBELEM_H_
#define _SPCBELEM_H_

#include <iostream.h>
class PCB;
class KernelSem;
class SemaphorePCBElem {
public:
	PCB* e;
	unsigned int vreme;
	SemaphorePCBElem* tsl;
	SemaphorePCBElem* sl;
	KernelSem* mojsem;
	SemaphorePCBElem();
	SemaphorePCBElem(PCB*,unsigned int,KernelSem*);
	~SemaphorePCBElem();
};

#endif
