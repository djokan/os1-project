#include <iostream.h>
#include <stdlib.h>
#include <dos.h>
#include "Thread.h"
#include "Kernel.h"
#include "Event.h"
#include "Semaphor.h"
#include "Sch1.h"
#include "operator.h"
#include "PCB.h"





//void tick(){}
Semaphore mutex(1);
Semaphore sleep1(0);
class A: public Thread
{
protected:
	void run();
public:
	A():Thread(60,1){};
	int d;
};

void A::run()
{
	Kernel::lock();
	cout << "User " << d << "running" << endl;
	Kernel::unlock();
	int i=0,p,p1;
	while(i<100)
	{
		i++;
		mutex.wait(0);
		Kernel::lock();
		cout << "t(" << d << ")," <<i << " ";
		Kernel::unlock();
		mutex.signal();
		sleep1.wait(2);
	}
}

class Keyboard : public Thread
{
protected:
	void run();
};

extern char keymap[128];

void Keyboard::run()
{
	Kernel::lock();
	Event event9(9);
	cout<<"KeyebordListener started!"<<endl;
	Kernel::unlock();
	char scancode, status, znak,theEnd=0;

	while (!theEnd) {

		event9.wait();
		do{
			status = inportb(0x64); // ocitava statusni reg. sa 64h


			if (status & 0x01){           // Can I read?
				scancode = inportb(0x60);

				if (scancode==-127){
					theEnd = 1;
					mutex.wait(0);
					Kernel::lock();
					cout << "ESC ";
					Kernel::unlock();
					mutex.signal();
				}else {
					if (scancode&0x80) {
						mutex.wait(0);
						Kernel::lock();
						cout << keymap[scancode&0x7F] << " ";
						Kernel::unlock();
						mutex.signal();
					}
				}

			};
       //////////////////////
			asm{
				cli
				in      al, 61h         //; Send acknowledgment without
				or      al, 10000000b   //;   modifying the other bits.
				out     61h, al         //;
				and     al, 01111111b   //;
				out     61h, al         //;
				mov     al, 20h         //; Send End-of-Interrupt signal
				out     20h, al         //;
				sti
			}

		 }while (!theEnd && status & 0x01); //dok se ocitava takav status da je pritisnut neki taster

	}// while

	Kernel::lock();
		cout<<endl<<"KeyebordListener stopped!"<<endl;
	Kernel::unlock();

}


int userMain1(int argc, char* argv[])
{


	return 0;
};
