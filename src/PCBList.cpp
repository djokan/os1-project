/*
 * ThrList.cpp
 *
 *  Created on: Apr 25, 2016
 *      Author: OS1
 */

#include "PCBList.h"
#include "Kernel.h"
#include <iostream.h>
#include "operator.h"

//#define DEBUG

PCBList::PCBList() {

	glava=NULL;
	rep=NULL;
}

void PCBList::ispis()
{
	Kernel::lock();
	cout << "PCBList ispis: list= " << this << endl;
	PCBElem* x=glava;
	while (x!=NULL)
	{
		cout << x->e->thread << "->";
		x=x->sl;
	}
	cout << endl;
	Kernel::unlock();
}

void PCBList::put(PCB* a)
{
	Kernel::lock();
	if (a==NULL)
		Kernel::error("Error: putting NULL element to waittocomplete list");
#ifdef DEBUG
	cout << "put PCBList " << this << ", put " << a->thread << endl;
#endif
	if (glava==NULL)
	{
		glava= new PCBElem();
		rep=glava;
		glava->e=a;
	}
	else
	{
		rep->sl= new PCBElem();
		rep=rep->sl;
		rep->e=a;
	}

#ifdef DEBUG
	ispis();
#endif
	Kernel::unlock();
}
PCBList::~PCBList()
{
	glava=NULL;
	rep=NULL;
	
}


PCB* PCBList::get()
{
	Kernel::lock();
#ifdef DEBUG
	cout << "getPCBList " << this  << ", return " << glava->e->thread << endl;
	ispis();
#endif
	if (glava==NULL)
	{
		Kernel::unlock();
		return NULL;
	}
	if (glava->e==NULL)
	{
		Kernel::error("Error: getting NULL element from waittocomplete list");
	}
	PCB* t=glava->e;
	PCBElem* a=glava;
	if (glava==rep)
	{
		rep=NULL;
		glava=NULL;
	}
	else
	{
		glava=glava->sl;
	}
	delete a;
	Kernel::unlock();
	return t;
}
