
#include "operator.h"
#include <iostream.h>
#include "Kernel.h"
#include <malloc.h>
#include <stdio.h>
//#define DEBUG
void* operator new(unsigned t) {
	Kernel::lock();
#ifdef DEBUG
	putchar('n');
#endif
	void* m= malloc(t);
	if (m==NULL && t>0)
	{

		//Kernel::error("ERROR: Not enough memory!");
	}
	Kernel::unlock();
	return m;

}

void operator delete(void* t) {

	Kernel::lock();
#ifdef DEBUG
	putchar('d');
#endif
	if (t!=NULL)
	{
		free(t);

	}
	Kernel::unlock();

}
