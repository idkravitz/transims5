//*********************************************************
//	Connect_Data.hpp - network lane connectivity data
//*********************************************************

#ifndef CONNECT_DATA_HPP
#define CONNECT_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Connect_Data class definition
//---------------------------------------------------------

class SYSLIB_API Connect_Data : public Notes_Data
{
public:
	Connect_Data (void)                   { Clear (); }

	int  Dir_Index (void)                 { return (dir_index); }
	int  To_Index (void)                  { return (to_index); }
	int  Low_Lane (void)                  { return (low_lane); }
	int  High_Lane (void)                 { return (high_lane); }
	int  To_Low_Lane (void)               { return (to_low); }
	int  To_High_Lane (void)              { return (to_high); }
	int  Type (void)                      { return (type); }
	int  Control(void)                    { return (control); }
	int  Penalty (void)                   { return (penalty); }
	int  Speed (void)                     { return (speed); }
	int  Capacity (void)                  { return (capacity); }
	int  Next_To (void)                   { return (next_to); }
	int  Next_From (void)                 { return (next_from); }
	int  Next_Index (bool to_flag = true) { return ((to_flag) ? next_to : next_from); }

	void Dir_Index (int value)            { dir_index = value; }
	void To_Index (int value)             { to_index = value; }
	void Low_Lane (int value)             { low_lane = (char) value; }
	void High_Lane (int value)            { high_lane = (char) value; }
	void To_Low_Lane (int value)          { to_low = (char) value; }
	void To_High_Lane (int value)         { to_high = (char) value; }
	void Type (int value)                 { type = (char) value; }
	void Control (int value)              { control = (char) value; }
	void Penalty (int value)              { penalty = (short) value; }
	void Speed (int value)                { speed = (short) value; }
	void Capacity (int value)             { capacity = (short) value; }
	void Next_To (int value)              { next_to = value; }
	void Next_From (int value)            { next_from = value; }

	void Speed (double value)             { speed = (short) exe->Round (value); }
	
	void Clear (void)
	{
		dir_index = to_index = 0; low_lane = high_lane = to_low = to_high = type = control = 0;
		penalty = speed = capacity = 0; next_to = next_from = -1; Notes_Data::Clear ();
	}
private:
	int        dir_index;
	int        to_index;
	char       low_lane;
	char       high_lane;
	char       to_low;
	char       to_high;
	char       type;
	char       control;
	short      penalty;
	short      speed;
	short      capacity;
	int        next_to;
	int        next_from;
};

typedef vector <Connect_Data>    Connect_Array;
typedef Connect_Array::iterator  Connect_Itr;
#endif
