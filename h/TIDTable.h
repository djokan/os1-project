#ifndef _TIDTable_h_
#define _TIDTable_h_

class Thread;


struct intElem
{
	int e;
	intElem* sl;
};

class ThreadIdTable{
public:
	static Thread** map;
	static unsigned size;
	static intElem *slobodni;
	static intElem *rep;
	static int ids;
	static void clear();
	ThreadIdTable();
	static void put(int ,Thread*);
	static Thread* get(int id);
	static int getnewId();


};


#endif
