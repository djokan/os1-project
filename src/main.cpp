

//TODO proveriti da li su svi pozivi new i delete obezbedjeni od promene konteksta
#include "Kernel.h"
#include "Thread.h"
#include "IVTEntry.h"
#include "operator.h"




class wubcuaksdjfn : public Thread
{
public:
	wubcuaksdjfn(int b,char* a[]);
	int t;
	int argc;
	char** argv;
protected:
	virtual void run();
};

wubcuaksdjfn::wubcuaksdjfn(int a, char* b[]):Thread()
{
	argc=a;
	argv=b;
}
int userMain(int argc, char* argv[]);
void wubcuaksdjfn::run()
{
	t=userMain(argc,argv);
}



int main(int argc, char* argv[])
{
#ifdef DEBUG
	unsigned ss1,sp1;

	asm mov ss1,ss
	asm mov sp1,sp
	cout << "stack: " <<ss1 << " " << sp1 << endl;
	asm push ax
	asm mov ss1,ss
	asm mov sp1,sp
	cout << "stack: " <<ss1 << " " << sp1 << endl;
	asm pop ax
#endif

	Kernel::inic();

	int t=0;

	wubcuaksdjfn *user= new wubcuaksdjfn(argc,argv);

	user->start();
	user->waitToComplete();
	t=user->t;
	delete user;


	Kernel::restore();
	return t;
}
