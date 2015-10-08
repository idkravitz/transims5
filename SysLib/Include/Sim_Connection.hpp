//*********************************************************
//	Sim_Connection.hpp - link connection data
//*********************************************************

#ifndef SIM_CONNECTION_HPP
#define SIM_CONNECTION_HPP

#include "TypeDefs.hpp"
#include "Lane_Map_Data.hpp"

#include <vector>
using namespace std;

#define NUM_SIM_CONFLICTS	2

//---------------------------------------------------------
//	Sim_Connection class definition
//---------------------------------------------------------
 
class Sim_Connection : public Lane_Map_Array
{
public:
	Sim_Connection (void)                { Clear (); }

	int   Conflict (int num)             { return (Check (num) ? conflict [num] : -1); }

	void  Conflict (int num, int value)  { if (Check (num)) conflict [num] = value; }

	int   Max_Conflicts (void)           { return (NUM_SIM_CONFLICTS); }

	void Clear (void)
	{
		memset (conflict, -1, sizeof (conflict)); clear ();
	}
private:
	bool Check (int num)                 { return (num >= 0 && num < NUM_SIM_CONFLICTS); }
	int  conflict [NUM_SIM_CONFLICTS];
};

typedef vector <Sim_Connection>      Sim_Connect_Array;
typedef Sim_Connect_Array::iterator  Sim_Connect_Itr;

#endif
