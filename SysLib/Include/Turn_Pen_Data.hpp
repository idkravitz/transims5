//*********************************************************
//	Turn_Pen_Data.hpp - network turn penalty data
//*********************************************************

#ifndef TURN_PEN_DATA_HPP
#define TURN_PEN_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"
#include "Dtime.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Turn_Pen_Data class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Pen_Data : public Notes_Data
{
public:
	Turn_Pen_Data (void)                   { Clear (); }

	int   Dir_Index (void)                 { return (dir_index); }
	int   To_Index (void)                  { return (to_index); }
	Dtime Start (void)                     { return (start); }
	Dtime End (void)                       { return (end); }
	int   Use (void)                       { return (use); }
	int   Min_Veh_Type (void)              { return (min_type); }
	int   Max_Veh_Type (void)              { return (max_type); }
	int   Penalty (void)                   { return (penalty); }
	int   Next_To (void)                   { return (next_to); }
	int   Next_From (void)                 { return (next_from); }
	int   Next_Index (bool to_flag = true) { return ((to_flag) ? next_to : next_from); }
	
	void  Dir_Index (int value)            { dir_index = value; }
	void  To_Index (int value)             { to_index = value; }
	void  Start (Dtime value)              { start = value; }
	void  End (Dtime value)                { end = value; }
	void  Use (int value)                  { use = (short) value; }
	void  Min_Veh_Type (int value)         { min_type = (char) value; }
	void  Max_Veh_Type (int value)         { max_type = (char) value; }
	void  Penalty (int value)              { penalty = value; }
	void  Next_To (int value)              { next_to = value; }
	void  Next_From (int value)            { next_from = value; }

	void  Clear (void)
	{
		dir_index = to_index = next_to = next_from = -1; start = end = 0; use = 0; penalty = 0; 
		min_type = max_type = -1; Notes_Data::Clear ();
	}	
private:
	int     dir_index;
	int     to_index;
	Dtime   start;
	Dtime   end;
	short   use;
	char    min_type;
	char    max_type;
	int     penalty;
	int     next_to;
	int     next_from;
};

typedef vector <Turn_Pen_Data>    Turn_Pen_Array;
typedef Turn_Pen_Array::iterator  Turn_Pen_Itr;

#endif
