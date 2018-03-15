#ifndef _Kernel_h_
#define _Kernel_h_


class PCB;
class KernelEv;
class KernelSem;
class Kernel
{
	static volatile unsigned oldTimerOFF, oldTimerSEG;
	static volatile void* thread_function;
	static volatile int istek_vremena;

	public:
	static void error(char*);


	static volatile short locked;
	static volatile short lockedistek;
	static PCB *running,*idle;
	static void lock();
	static void unlock();
	static void locki();
	static void unlocki();
	static void inic();
	static void add_interrupted_events_from_nextevent();
	static void add_interrupted_events();
	static void restore();
	static void test();
	static PCB* pcbarg;
	static KernelSem* semarg;
	static unsigned timearg;
	static unsigned bparg;
	static void interrupt create_thread();
	static void interrupt timer();
	static void interrupt exit_thread();
	static void interrupt fork();
	static void interrupt block();
	static void interrupt popdispatch();
	static void interrupt dispatch();
	static void interrupt dispatch_i();
};



#endif
