#include "SPCBElem.h"
#include "operator.h"

SemaphorePCBElem::SemaphorePCBElem() {
	tsl= NULL;
	sl=NULL;
	mojsem=NULL;
}

SemaphorePCBElem::SemaphorePCBElem(PCB* a,unsigned int b,KernelSem* c)
{
	e=a;
	vreme=b;
	mojsem= c;
	tsl= NULL;
	sl=NULL;
}

SemaphorePCBElem::~SemaphorePCBElem(){};
