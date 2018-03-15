#include "TIDTable.h"
#include "operator.h"
#include <iostream.h>

Thread**	ThreadIdTable::map= new Thread* [32];
unsigned	ThreadIdTable::size=32;
intElem *ThreadIdTable::slobodni=NULL;
intElem *ThreadIdTable::rep=NULL;
int ThreadIdTable::ids=0;
ThreadIdTable::ThreadIdTable()
{

}

void ThreadIdTable::clear()
{
	intElem* t;
	while (slobodni!=NULL)
	{
		t=slobodni;
		slobodni=slobodni->sl;
		delete t;
	}
	delete[] map;
}

int ThreadIdTable::getnewId()
{
	int i;
	intElem *t;
	if (slobodni==NULL) return ids++;
	else
	{
		if (slobodni->sl==NULL)
		{
			rep=NULL;
			i=slobodni->e;
			delete slobodni;
			slobodni=NULL;
			return i;
		}
		i=slobodni->e;
		t= slobodni;
		slobodni=slobodni->sl;
		delete t;
		return i;
	}
}

void ThreadIdTable::put(int id,Thread* a)
{
	int i;
	if (a!=NULL)
	{

		if (id>=size)
		{
			Thread** t= new Thread* [(size*3)/2];
			for ( i=0;i<size;i++)
			{
				t[i]=map[i];
			}
			delete[] map;
			map=t;
			t=NULL;
			size=(size*3)/2;
		}
		map[id]=a;
	}
	else
	{
		map[id]=NULL;
		if (slobodni!=NULL)
		{
			rep->sl=new intElem ;
			rep=rep->sl;
			rep->sl=NULL;
		}
		else
		{
			slobodni=new intElem ;
			rep=slobodni;
			rep->sl=NULL;
		}
		rep->e=id;
	}
};

Thread* ThreadIdTable::get(int id)
{
	if (id<size && id>=0) return map[id];
	return 0;
}
