//*********************************************************
//	Lane_Use_Data.hpp - network lane use data
//*********************************************************

#ifndef LANE_USE_DATA_HPP
#define LANE_USE_DATA_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Link_Dir_Data.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Lane_Use_Data class definition
//---------------------------------------------------------

class SYSLIB_API Lane_Use_Data : public Notes_Data
{
public:
	Lane_Use_Data (void)               { Clear (); }

	int   Dir_Index (void)             { return (dir_index); }
	int   Low_Lane (void)              { return (low_lane); }
	int   High_Lane (void)             { return (high_lane); }
	int   Type (void)                  { return (type); }
	int   Use (void)                   { return (use); }
	int   Min_Veh_Type (void)          { return (min_type); }
	int   Max_Veh_Type (void)          { return (max_type); }
	int   Min_Traveler (void)          { return (min_trav); }
	int   Max_Traveler (void)          { return (max_trav); }
	Dtime Start (void)                 { return (start); }
	Dtime End (void)                   { return (end); }
	int   Offset (void)                { return (offset); }
	int   Length (void)                { return (length); }
	int   Toll (void)                  { return (toll); }
	int   Toll_Rate (void)             { return (rate); }
	int   Min_Delay (void)             { return (min_delay); }
	int   Max_Delay (void)             { return (max_delay); }
	int   Next_Index (void)            { return (next_index); }
	
	void  Dir_Index (int value)        { dir_index = value; }
	void  Low_Lane (int value)         { low_lane = (short) value; }
	void  High_Lane (int value)        { high_lane = (short) value; }
	void  Type (int value)             { type = (short) value; }
	void  Use (int value)              { use = (short) value; }
	void  Min_Veh_Type (int value)     { min_type = (short) value; }
	void  Max_Veh_Type (int value)     { max_type = (short) value; }
	void  Min_Traveler (int value)     { min_trav = (short) value; }
	void  Max_Traveler (int value)     { max_trav = (short) value; }
	void  Start (Dtime value)          { start = value; }
	void  End (Dtime value)            { end = value; }
	void  Offset (int value)           { offset = value; }
	void  Length (int value)           { length = value; }
	void  Toll (int value)             { toll = (short) value; }
	void  Toll_Rate (int value)        { rate = (short) value; }
	void  Min_Delay (int value)        { min_delay = (short) value; }
	void  Max_Delay (int value)        { max_delay = (short) value; }
	void  Next_Index (int value)       { next_index = value; }
	
	void  Offset (double value)        { offset = exe->Round (value); }
	void  Length (double value)        { length = exe->Round (value); }
	void  Toll_Rate (double value)     { rate = (short) exe->Round (value); }
	void  Min_Delay (double value)     { min_delay = (short) exe->Round (value); }
	void  Max_Delay (double value)     { max_delay = (short) exe->Round (value); }

	void  Clear (void)
	{
		dir_index = next_index = -1, low_lane = high_lane = type = toll = rate = min_delay = max_delay = 0;
		min_type = max_type = min_trav = max_trav = -1; start = end = 0; offset = length = 0; Notes_Data::Clear (); 
	}
private:
	int        dir_index;
	short      low_lane;
	short      high_lane;
	short      type;
	short      use;
	short      min_type;
	short      max_type;
	short      min_trav;
	short      max_trav;
	short      toll;
	short      rate;
	short      min_delay;
	short      max_delay;
	Dtime      start;
	Dtime      end;
	int        offset;
	int        length;
	int        next_index;
};

typedef vector <Lane_Use_Data>    Lane_Use_Array;
typedef Lane_Use_Array::iterator  Lane_Use_Itr;

//---------------------------------------------------------
//	Lane_Use_Period class definition
//---------------------------------------------------------

class SYSLIB_API Lane_Use_Period
{
public:
	Lane_Use_Period (void)             { Clear (); }

	Dtime Start (void)                 { return (start); }
	Dtime End (void)                   { return (end); }
	int   Index (void)                 { return (index); }
	int   Records (void)               { return (records); }
	int   Periods (void)               { return (periods); }
	int   Lanes0 (void)                { return (lanes [0]); }
	int   Lanes1 (void)                { return (lanes [1]); }
	int   Lanes (int num)              { return ((num >= 0 && num < 2) ? lanes [num] : 0); }

	void  Start (Dtime value)          { start = value; }
	void  End (Dtime value)            { end = value; }
	void  Index (int value)            { index = value; }
	void  Records (int value)          { records = (char) value; }
	void  Periods (int value)          { periods = (char) value; }
	void  Lanes0 (int value)           { lanes [0] = (char) value; }
	void  Lanes1 (int value)           { lanes [1] = (char) value; }
	void  Lanes (int num, int value)   { if (num >= 0 && num < 2) lanes [num] = (char) value; }

	void  Add_Record (void)            { records++; };

	void  Clear (void)
	{
		start = end = 0; index = -1, records = periods = lanes [0] = lanes [1] = 0; 
	}
private:
	Dtime start;
	Dtime end;
	int   index;
	char  records;
	char  periods;
	char  lanes [2];
};

typedef vector <Lane_Use_Period>    Use_Period_Array;
typedef Use_Period_Array::iterator  Use_Period_Itr;

#endif
