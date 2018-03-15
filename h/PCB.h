#ifndef _PCB_h_
#define _PCB_h_


class Thread;
class PCBList;
class PCB{
public:
	unsigned volatile ss;
	unsigned volatile sp;
	unsigned short semretval;
	unsigned long stackSize;
	unsigned *stack;
	int id;
	friend class Kernel;
	unsigned short stanje; //0- nije poceo, 1- poceo, 2-zavrsio
	static int ids;
	Thread* thread;
	static  Thread* main;
	static  Thread* idle;
	PCBList* listanacekanju;
	PCBList* deca;
	int kvant;
	static void run();
	void clone(PCB*) ;
	PCB();
	~PCB();
	static PCB* make_main_thread();
	static void close_main_thread();
	static void make_idle_thread();
	static void close_idle_thread();

};



#endif
