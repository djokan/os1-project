/*
 * ThrElem.h
 *
 *  Created on: Apr 25, 2016
 *      Author: OS1
 */

#ifndef _PCBELEM_H_
#define _PCBELEM_H_
#include <iostream.h>

class PCB;
class PCBElem {
public:

	PCB* e;
	PCBElem* sl;
	PCBElem();
	~PCBElem();
};

#endif /* CPP_THRELEM_H_ */
