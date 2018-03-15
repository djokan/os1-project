/*
ova klasa funkcionise tako sto svaki put kada se zove kernel::timer ovde se pozivaju metode timer i timernocontext u zavisnosti od
toga da li je kernel zakljucan. timer ce da odblokira najvise 3 niti da ne bi metoda kernel::timer predugo trajala.

postoji posebna staticka lista cija je glava promenljiva tglava i tu se nalaze niti koje mogu da se odblokiraju posle nekog vremena
u slucaju da nisu odblokirane metodom signal()
za sve objekte ove klase postoje posebne liste u kojima se nalaze niti koje cekaju na signal(). ukoliko se nit odblokira posle nekog
vremena, brise se iz liste elemenata koji cekanju signal() i obrnuto

SemaphorePCBElem je element liste i uradjena je optimizacija tako da se jedan element kreira za svaku nit nezavisno od toga da li ce da
bude ubacena u listu niti koje cekaju ili ne, samo se povezuju drugacije. ako se element izbacuje iz jedne liste a postoji u drugoj
vrednost elementa liste se postavlja na NULL, sto ce da bude indikator drugoj listi da je element nevalidan i da treba breci na sledeci

*/


#include "KSem.h"
#include "PCB.h"
#include "Sch1.h"
#include "Kernel.h"
#include <iostream.h>
#include <stdio.h>
#include "operator.h"
//#define DEBUG

SemaphorePCBElem* KernelSem::tglava=NULL;
unsigned int KernelSem::vreme=1;
unsigned int KernelSem::contextvreme=1;
extern unsigned timearg;

KernelSem::KernelSem(int i) {
	glava=NULL;
	rep=NULL;
	v=i;
}

void KernelSem::ticknocontext()
{
	vreme++;
	if (vreme==0) vreme=1;
	if (vreme==contextvreme) vreme--;
	if (vreme==0) vreme--;
}



void KernelSem::ispis()
{
	SemaphorePCBElem* x=glava;
	cout << this->v << endl;
	while (x!=NULL)
	{
		if (x->e==NULL) cout <<  "NULL ";
		else cout << x->e->id << " ";
		x=x->sl;
	};
	cout << endl;
	ispis1();


}

void KernelSem::ispis1()
{
	SemaphorePCBElem* x=tglava;
	cout << "vreme:" << vreme << endl;
	cout << "contextvreme:" << contextvreme << endl;
		while (x!=NULL)
		{
			if (x->e==NULL) cout << "NULL:" << x->vreme << " ";
			else cout << x->e->id << ":" << x->vreme << " ";
			x=x->tsl;
		};
		cout << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}

void KernelSem::tick()
{
	int iter=0;
	vreme++;
	if (vreme==0) vreme=1;
	if (vreme==contextvreme) vreme--;
	if (vreme==0) vreme--;
	SemaphorePCBElem* x;
	while ((tglava!=NULL && (iter<3)) && (tglava->vreme-contextvreme<=vreme-contextvreme || tglava->e==NULL))
	{
		iter++;
		if (tglava->e==NULL)
		{
			x=tglava;
			tglava=tglava->tsl;
			delete x;
		}
		else
		{
			tglava->mojsem->v++;
			tglava->e->semretval=0;
#ifdef DEBUG
			Kernel::lock();
			//ispis1();
			//cout << "semaphore tick() on thread " << tglava->e->thread << endl;
			Kernel::unlock();
#endif
			Scheduler1::put(tglava->e);
			tglava->e=NULL;
			tglava=tglava->tsl;
		}
	}
	if (iter<3 || tglava==NULL)
	{
		contextvreme=vreme;
	}
	else
	{
		contextvreme=tglava->vreme;
	}


}

void KernelSem::put(PCB* a,unsigned int t)
{
	unsigned int vreme=KernelSem::vreme;
	SemaphorePCBElem* x;
	if (t!=0)
	{
		t=vreme+t;
		if (t<vreme) t++;
	}
	while (glava->e==NULL)
	{
		x=glava;
		glava=glava->sl;
		delete x;
	}

	x=new SemaphorePCBElem(a,t,this);
	if (glava==NULL)
	{
		glava=x;
		rep=x;
	}
	else
	{

		rep->sl=x;
		rep=rep->sl;
	}

	if (t!=0)
	if (tglava==NULL)
	{
		tglava=x;
	}
	else
	{
		if (tglava->vreme>t)
		{
			x->tsl=tglava;
			tglava=x;
		}
		else
		{
			SemaphorePCBElem* temp=tglava;
			while (temp->tsl!=NULL && temp->tsl->vreme<=t) // TODO optimizacija preko stabla
			{
				temp=temp->tsl;
			}
			x->tsl=temp->tsl;
			temp->tsl=x;

		}
	}
}
int KernelSem::wait(unsigned int maxTimeToWait)
{
	unsigned short t;
	Kernel::lock();
#ifdef DEBUG
	cout << "semaphore " << this << " wait() on thread " << Kernel::running->thread << endl;
#endif
	v--;
	if (v<0)
	{

		put(Kernel::running,maxTimeToWait);
		Kernel::block();
		Kernel::lock();
	}

#ifdef DEBUG
	cout << "semaphore " << this << " finished waiting on thread " << Kernel::running->thread << endl;
#endif
	t=Kernel::running->semretval;
	Kernel::unlock();
	return t;
};

void KernelSem::signal()
{
	Kernel::lock();

	v++;
	if (v<=0)
	{

		SemaphorePCBElem* x;
		while (glava->e==NULL)
		{
			x=glava;
			glava=glava->sl;
			delete x;
		}
		if (glava!=NULL)
		{
			glava->e->semretval=1;
#ifdef DEBUG
			cout << "semaphore " << this << " signal() on thread " << glava->e->thread << endl;
#endif
			Scheduler1::put(glava->e);
			if (glava->vreme!=0)
			{
				glava->e=NULL;
				glava=glava->sl;
			}
			else
			{
				x=glava;
				glava=glava->sl;
				delete x;
			}
		}
	}
	Kernel::unlock();
}

KernelSem::~KernelSem()
{
	Kernel::lock();
	SemaphorePCBElem* x;
	while (glava!=NULL)
	{
		if (glava->e==NULL)
		{
			x=glava;
			glava=glava->sl;
			delete x;
		}
		else
		{
			Scheduler1::put(glava->e);
			if (glava->vreme!=0)
			{
				glava->e=NULL;
				glava=glava->sl;
			}
			else
			{
				x=glava;
				glava=glava->sl;
				delete x;
			}
		}
	}
	Kernel::unlock();
}
