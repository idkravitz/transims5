//*********************************************************
//	Path_Data.hpp - path data classes
//*********************************************************

#ifndef PATH_DATA_HPP
#define PATH_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Path_Data class definition
//---------------------------------------------------------

class SYSLIB_API Path_Data
{
public:
	Path_Data (void)                    { Clear (); }

	unsigned Imped (void)               { return (imped); }
	Dtime    Time (void)                { return (time); }
	int      Length (void)              { return (length); }
	int      Cost (void)                { return (cost); }
	int      Walk (void)                { return (walk); }
	int      From (void)                { return (from); }
	int      Type (void)                { return (type); }
	int      Path (void)                { return (path); }
	int      Run (void)                 { return (run); }
	int      Mode (void)                { return (mode); }
	int      Xfer (void)                { return (xfer); }
	int      Dir (void)                 { return (dir); }
	int      Status (void)              { return (status); }
		
	void     Imped (unsigned value)     { imped = value; }
	void     Time (Dtime value)         { time = value; }
	void     Length (int value)         { length = value; }
	void     Cost (int value)           { cost = value; }
	void     Walk (int value)           { walk = value; }
	void     From (int value)           { from = value; }
	void     Type (int value)           { type = (char) value; }
	void     Path (int value)           { path = (char) value; }
	void     Run (int value)            { run = (short) value; }
	void     Mode (int value)           { mode = (char) value; }
	void     Xfer (int value)           { xfer = (char) value; }
	void     Dir (int value)            { dir = (char) value; }
	void     Status (int value)         { status = (char) value; }
	
	void     Add_Imped (unsigned value) { imped += value; }
	void     Add_Time (Dtime value)     { time += value; }
	void     Add_Length (int value)     { length += value; }
	void     Add_Cost (int value)       { cost += value; }
	void     Add_Walk (int value)       { walk += value; }

	void     Clear (void) 
	{
		imped = MAX_IMPEDANCE; time = 0; length = cost = walk = 0; from = -1;
		run = 0; type = path = mode = xfer = dir = status = 0;
	}
	void     Add (Path_Data *ptr, bool forward_flag = true) 
	{
		imped += ptr->imped; length += ptr->length; cost += ptr->cost; walk += ptr->walk;
		if (forward_flag) time += ptr->time; else time -= ptr->time;
	}
	void     Subtract (Path_Data *ptr, bool forward_flag = true)
	{
		imped -= ptr->imped; length -= ptr->length; cost -= ptr->cost; walk -= ptr->walk;
		if (forward_flag) time -= ptr->time; else time += ptr->time;
	}

private:
	unsigned imped;
	Dtime    time;
	int      length;
	int      cost;
	int      walk;
	int      from;
	char     type;
	char     path;
	short    run;
	char     mode;
	char     xfer;
	char     dir;
	char     status;
};

//---------------------------------------------------------
//	Path_Array class definition
//---------------------------------------------------------

class SYSLIB_API Path_Array : public vector <Path_Data>
{
public:
	Path_Array (void)         { Clear (); }

	void Clear (void)
	{
		vector <Path_Data>::iterator itr;

		for (itr = begin (); itr != end (); itr++) {
			itr->Clear ();
		}
	}
};
typedef Path_Array::iterator          Path_Itr;
typedef Path_Array::reverse_iterator  Path_RItr;
typedef Path_Data *                   Path_Ptr;

//---- impedance sort index ----

typedef multimap <unsigned, int>      Imp_Sort;
typedef pair <unsigned, int>          Imp_Sort_Data;
typedef Imp_Sort::iterator            Imp_Sort_Itr;

#endif
