#include "Kernel.h"
#include <dos.h>
#include "Sch1.h"
#include "PCB.h"
#include "PCBList.h"
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include "KSem.h"
#include "TIDTable.h"


#include "operator.h"
#include "TIDTable.h"

volatile unsigned Kernel::oldTimerOFF = 0;
volatile unsigned Kernel::oldTimerSEG = 0;
volatile short Kernel::locked = 0;
volatile short Kernel::lockedistek = 0;
PCB* Kernel::running = NULL;
PCB* Kernel::idle = NULL;
volatile void* Kernel::thread_function = 0;
volatile int Kernel::istek_vremena = 0;
PCB* Kernel::pcbarg=NULL;
KernelSem* Kernel::semarg=NULL;
unsigned Kernel::timearg=0;
unsigned Kernel::bparg=0;
unsigned a4,b4,c4,d4,e4,f4,g4,h4;
unsigned nextevent=0;
unsigned *tmpstack;
void tick();




void Kernel::test() {

}

unsigned volatile a2, b2;


void interrupt Kernel::block()//ova metoda za nit koja se izvrsava cuva sp i ss, a zatim postavlja drugu nit da ze izvrsava
{
	asm{
			mov a2,sp
			mov b2,ss
	}
	Kernel::running->sp=a2;
	Kernel::running->ss=b2;

	Kernel::running = Scheduler1::get();
	a2 = Kernel::running->sp;
	b2 = Kernel::running->ss;
	Kernel::locki();
	Kernel::istek_vremena = Kernel::running->kvant + 1;
	Kernel::unlocki();
	asm{
		mov sp,a2
		mov ss,b2
	}
	Kernel::unlock();
}

void interrupt Kernel::fork()
{
/* 
	posto su stekovi niti roditelja i deteta dobri do poziva ovog interrupta,sada za nit dete mora da se postavi stek
	tako da kada se vraca iz interrupta nastavi regularno sa izvrsavanjem. Takodje stavlja pcb deteta u scheduler 
	i u listu dece svog roditelja.

*/
	asm{
				mov a2,sp
				mov b2,ss
	}
	Kernel::running->sp=a2;
	Kernel::running->ss=b2;
	*(unsigned*)MK_FP(b2,a2+26)=bparg;

	*(unsigned*)MK_FP(b2,a2+28)=Kernel::pcbarg->id;// lokalna promenljiva roditelja "i" metode fork dobija vrednost id-a deteta

	*(unsigned*)MK_FP(Kernel::pcbarg->ss,Kernel::pcbarg->sp+2)= Kernel::pcbarg->sp+bparg-Kernel::running->sp-24;
	// lokalna promenljiva roditelja "b1" metode fork dobija vrednost bp-a na koji treba da se postavi
	Kernel::pcbarg->sp-=24;
	for (int i=0;i<24;i+=2)
	{
		*(unsigned*)MK_FP(Kernel::pcbarg->ss,Kernel::pcbarg->sp+i)=*(unsigned*)MK_FP(Kernel::running->ss,Kernel::running->sp+i);

	}
	Scheduler1::put(Kernel::pcbarg);

	Kernel::running->deca->put(Kernel::pcbarg);

	/*for (a2=36;a2>0;a2-=2)
		{
			cout << *(unsigned*)MK_FP(Kernel::pcbarg->ss,Kernel::pcbarg->sp+a2-2) << " " << Kernel::pcbarg->ss << ":" << Kernel::pcbarg->sp+a2-2 <<"        ";
			cout << *(unsigned*)MK_FP(Kernel::running->ss,Kernel::running->sp+a2-2) << " " << Kernel::running->ss << ":" << Kernel::running->sp+a2-2  << " " << Kernel::running->id << endl;

		}*/


	Kernel::unlock();
}
extern PCB* event_queue[256];
extern int event_queue_not_empty_2[2];
extern int event_queue_not_empty_4[4];
extern int event_queue_not_empty_8[8];
extern int event_queue_not_empty_16[16];
extern int event_queue_not_empty_32[32];
extern int event_queue_not_empty_64[64];
extern int event_queue_not_empty_128[128];
extern int event_queue_not_empty_256[256];
extern int event_queue_not_empty;
void interrupt Kernel::timer() {
	tick();
	asm int 60h;
	if (locked==0) // ako je kernel zakljucan ne sme da se menja kontekst pa se pozivaju metode koje nisu bezbedne za pristup
	{// sa vise niti
		Kernel::lock();
		KernelSem::tick();
		if (nextevent!=0)
		{
			add_interrupted_events_from_nextevent();
			// ubacuje iz stabla pcbove dogadjaja koji nisu ubaceni u scheduler
			// razlog za pravljenje 2 metode (add_interrupted_events_from_nextevent i add_interrupted_events) je to sto
			// se najvise 3 pcb-a ubacuju u scheduler za vreme jednog pozivanja metode
			// timer da ne bi trajanje izvrsavanja ove metode trajalo duze od 55ms sto zna da izazove problem
		}
		else
		{
			add_interrupted_events();
		}
		
		if (Kernel::istek_vremena > 0)
		{

			Kernel::istek_vremena--;
			if (Kernel::istek_vremena == 0)// ako je vreme isteklo menja se kontekst
			{
				//putchar('p');
				asm mov a2,sp
				asm mov b2,ss

				Kernel::running->sp=a2;
				Kernel::running->ss = b2;

				Scheduler1::put(Kernel::running);
				Kernel::running = Scheduler1::get();
				Kernel::istek_vremena = Kernel::running->kvant;
				a2 = Kernel::running->sp;
				b2 = Kernel::running->ss;
				asm mov sp,a2
				asm mov ss,b2
			}
		}

		


		Kernel::unlock();
	}
	else
	{
		KernelSem::ticknocontext();
		if (Kernel::lockedistek==0)
		if (Kernel::istek_vremena > 1) Kernel::istek_vremena--;
	}

};

void Kernel::add_interrupted_events()
{
	short iter=0;
	if (event_queue_not_empty==1 && iter<3)
	{
		event_queue_not_empty=0;
		a4=0;
		while (a4<2 && iter<3)
		{
			if (event_queue_not_empty_2[a4]==1 && iter<3)
			{
				event_queue_not_empty_2[a4]=0;
				b4=a4*2;
				while (b4<a4*2+2 && iter<3)
				{
					if (event_queue_not_empty_4[b4]==1 && iter<3)
					{
						event_queue_not_empty_4[b4]=0;
						c4=b4*2;
						while (c4<b4*2+2 && iter<3)
						{
							if (event_queue_not_empty_8[c4]==1 && iter<3)
							{
								event_queue_not_empty_8[c4]=0;
								d4=c4*2;
								while (d4<c4*2+2 && iter<3)
								{
									if (event_queue_not_empty_16[d4]==1 && iter<3)
									{
										event_queue_not_empty_16[d4]=0;
										e4=d4*2;
										while ((e4<d4*2+2) && iter<3)
										{
											if (event_queue_not_empty_32[e4]==1 && iter<3)
											{
												event_queue_not_empty_32[e4]=0;
												f4=e4*2;
												while ((f4<e4*2+2) && iter<3)
												{
													if (event_queue_not_empty_64[f4]==1 && iter<3)
													{
														event_queue_not_empty_64[f4]=0;
														g4=f4*2;
														while ((g4<f4*2+2) && iter<3)
														{
															if (event_queue_not_empty_128[g4]==1 && iter<3)
															{
																event_queue_not_empty_128[g4]=0;
																h4=g4*2;
																if (event_queue_not_empty_256[h4]==1 && iter<3)
																{
																	event_queue_not_empty_256[h4]=0;
																	Scheduler1::put(event_queue[h4]);
																	iter++;
																	if (iter==3)
																	{
																		nextevent=h4+1;
																	}
																}
																if (event_queue_not_empty_256[h4+1]==1 && iter<3)
																{
																	event_queue_not_empty_256[h4+1]=0;
																	Scheduler1::put(event_queue[h4+1]);
																	iter++;
																	if (iter==3)
																	{
																		nextevent=h4+2;
																		if (nextevent==256)
																			nextevent=0;
																	}
																}

															}
															g4++;
														}
													}
													f4++;
												}
											}
											e4++;
										}
									}
									d4++;
								}
							}
							c4++;
						}
					}
					b4++;
				}
			}
			a4++;
		}

	}
	if (iter<3)
	{
		nextevent=0;
	}
}

void Kernel::add_interrupted_events_from_nextevent()
{
	event_queue_not_empty_128[nextevent/2]=1;
	event_queue_not_empty_64[nextevent/4]=1;
	event_queue_not_empty_32[nextevent/8]=1;
	event_queue_not_empty_16[nextevent/16]=1;
	event_queue_not_empty_8[nextevent/32]=1;
	event_queue_not_empty_4[nextevent/64]=1;
	event_queue_not_empty_2[nextevent/128]=1;
	event_queue_not_empty=1;

	add_interrupted_events();

}




void interrupt Kernel::exit_thread() { // izlazak iz niti- oslobadja se zauzeta memorija za stack, stanje pcb-a postaje 2 (zavrseno) 
	// i menja se kontekst
	tmpstack = Kernel::running->stack;
	Kernel::running->stanje=2;
	Kernel::running = Scheduler1::get();
	a2 = Kernel::running->sp;
	b2 = Kernel::running->ss;
	Kernel::locki();
	Kernel::istek_vremena = Kernel::running->kvant + 1;
	Kernel::unlocki();
	asm{
		mov sp,a2
		mov ss,b2
	}
	delete[] tmpstack;
	Kernel::unlock();
};


void interrupt Kernel::create_thread() { //postablja se nit koja se do sada izvrsavala u schedueler i pocinje se sa izvrsavanjem
	// kreirane niti
	asm mov a2,sp
	asm mov b2,ss

	Kernel::running->sp = a2;
	Kernel::running->ss = b2;
	Scheduler1::put(Kernel::running);
	a2 = pcbarg->sp;
	b2 = pcbarg->ss;
	Kernel::locki();
	Kernel::istek_vremena = Kernel::running->kvant + 1;
	Kernel::unlocki();
	Kernel::running = pcbarg;
	asm mov sp,a2
	asm mov ss,b2
	Kernel::unlock();
}

void Kernel::inic() { // inicijalizuje kernel
	volatile unsigned oldTimerSEG1, oldTimerOFF1, a, b;
	volatile unsigned newTimerSEG, newTimerOFF;
	Kernel::locked=0;
	newTimerSEG = FP_SEG(Kernel::timer);
	newTimerOFF = FP_OFF(Kernel::timer);
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h//; pamti staru rutinu
		mov word ptr oldTimerSEG1, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF1, ax
		mov ax, newTimerSEG
		mov word ptr es:0022h, ax//postavlja

		mov ax, newTimerOFF
		mov word ptr es:0020h, ax//novu rutinu

		mov ax, oldTimerSEG1//	postavlja staru rutinu
		mov word ptr es:0182h, ax//; na int 60h
		mov ax, oldTimerOFF1
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}

	Kernel::running=PCB::make_main_thread();// pravi se nit za pcb glavnog programa, ta nit ima prazan stek jer je stek vec zauzeo 
		//kompajler
	PCB::make_idle_thread();// kreira idle nit- nit koja ce da se izvrsava ukoliko ne postoji nit koja moze da se izvrsava

	Kernel::oldTimerOFF= oldTimerOFF1;
	Kernel::oldTimerSEG = oldTimerSEG1;

};


void Kernel::restore() { // vraca staru prekidnu rutinu i oslobadja memoriju koja nije oslobodjena
	volatile unsigned oldTimerSEG1, oldTimerOFF1;
	oldTimerOFF1 = Kernel::oldTimerOFF;
	oldTimerSEG1 = Kernel::oldTimerSEG;
	asm{
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG1
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF1
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}

	ThreadIdTable::clear();
	PCB::close_main_thread();
	PCB::close_idle_thread();


	SemaphorePCBElem* x;

	while (KernelSem::tglava!=NULL)
	{
		x=KernelSem::tglava;
		KernelSem::tglava=KernelSem::tglava->tsl;
		delete x;
	}
}
void Kernel::error(char* a) // ova metoda je napravljena u svrhu testiranja projekta, ukoliko dodje do greske ispise sta je greska i
// prekida sa radom programa
{
	Kernel::locked=1;
	cout << a << endl;
	volatile unsigned oldTimerSEG, oldTimerOFF;
		oldTimerOFF = Kernel::oldTimerOFF;
		oldTimerSEG = Kernel::oldTimerSEG;
		asm{
			cli
			push es
			push ax

			mov ax,0
			mov es,ax

			mov ax, word ptr oldTimerSEG
			mov word ptr es:0022h, ax
			mov ax, word ptr oldTimerOFF
			mov word ptr es:0020h, ax

			pop ax
			pop es
			sti
		}
		exit(1);
}
void Kernel::locki() { //posto se istek vremena poziva u delovima u kojima nije zakljucano jezgro, mora da se obezbedi konkurentnost
// radi isto kao obican lock()
	Kernel::lockedistek++;
}
void Kernel::unlocki() {
	Kernel::lockedistek--;

}


void Kernel::lock() {// kada je pozvana ova metoda, kernel se zakljucava i ne sme da se zove nijedna metoda u prekidima koja 
	// nije napravljena za vise niti u isto vreme, ni da se menja kontekst
	Kernel::locked++;
}
void Kernel::unlock() {
	/*if (locked<=0)
	{
		error("FATAL ERROR: locked<0");
	}*/
	Kernel::locked--;

}
void interrupt Kernel::popdispatch() { // ova metoda se poziva samo u prekidu na koji ceka dogadjaj
	asm add sp,24
	asm mov a2,sp
	asm mov b2,ss

	Kernel::running->sp=a2;
	Kernel::running->ss = b2;

	Scheduler1::put(Kernel::running);
	Kernel::running = Scheduler1::get();
	Kernel::locki();
	Kernel::istek_vremena = Kernel::running->kvant + 1;
	Kernel::unlocki();
	a2 = Kernel::running->sp;
	b2 = Kernel::running->ss;
	asm mov sp,a2
	asm mov ss,b2
	Kernel::unlock();
}
void interrupt Kernel::dispatch() {

	asm{
		mov a2,sp
		mov b2,ss
	}
	Kernel::running->sp = a2;
	Kernel::running->ss = b2;
	Scheduler1::put(Kernel::running);
	Kernel::running = Scheduler1::get();
	Kernel::locki();
	Kernel::istek_vremena = Kernel::running->kvant + 1;
	Kernel::unlocki();

	a2 = Kernel::running->sp;
	b2 = Kernel::running->ss;
	asm mov sp,a2
	asm mov ss,b2
	Kernel::unlock();
}
void interrupt Kernel::dispatch_i(){

	asm mov a2,sp
	asm mov b2,ss

	Kernel::running->sp=a2;
	Kernel::running->ss = b2;
	Kernel::idle=Kernel::running;
	Kernel::running = Scheduler1::get();
	Kernel::locki();
	Kernel::istek_vremena = Kernel::running->kvant + 1;
	Kernel::unlocki();

	a2 = Kernel::running->sp;
	b2 = Kernel::running->ss;
	asm mov sp,a2
	asm mov ss,b2
	Kernel::unlock();
}
