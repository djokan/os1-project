
#ifndef _KERNELEV_H_
#define _KERNELEV_H_

class PCB;
class KernelEv {
public:
	KernelEv(unsigned char);
	PCB* creator;
	int v;
	unsigned char ulaz;
	void signal();
	void nocontextsignal();
	void wait();
	~KernelEv();
};

#endif
