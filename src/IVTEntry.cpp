
#include "IVTEntry.h"
#include "Kernel.h"
#include "KernelEv.h"
#include <iostream.h>
#include <dos.h>
#include "operator.h"

KernelEv* IVTEntry::events[256] = {NULL};
PCB* event_queue[256]= {NULL};
int event_queue_not_empty=0;
int event_queue_not_empty_2[2]={0};
int event_queue_not_empty_4[4]={0};
int event_queue_not_empty_8[8]={0};
int event_queue_not_empty_16[16]={0};
int event_queue_not_empty_32[32]={0};
int event_queue_not_empty_64[64]={0};
int event_queue_not_empty_128[128]={0};
int event_queue_not_empty_256[256]={0};
IVTEntry::IVTEntry(void (interrupt * f)(),unsigned int i)
{
	ulaz=i;
	i*=4;
	Kernel::lock();
	old=(void(interrupt *)()) MK_FP(*((unsigned*)MK_FP(0,i+2)),*((unsigned*)MK_FP(0,i)));

	*((unsigned*)MK_FP(0,i+2))=FP_SEG(f);
	*((unsigned*)MK_FP(0,i))=FP_OFF(f);
	Kernel::unlock();

}
IVTEntry::~IVTEntry()
{
	int i=4*ulaz;
	Kernel::lock();
	*((unsigned*)MK_FP(0,i+2))=FP_SEG(old);
	*((unsigned*)MK_FP(0,i))=FP_OFF(old);
	Kernel::unlock();
}


