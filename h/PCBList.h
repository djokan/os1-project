#ifndef _PCBLIST_H_
#define _PCBLIST_H_
#include "PCBElem.h"
#include "PCB.h"
class PCBList {
	PCBElem* glava;
	PCBElem* rep;
public:
	PCBList();
	~PCBList();
	void put(PCB* );
	PCB* get();
	void ispis();
};

#endif
