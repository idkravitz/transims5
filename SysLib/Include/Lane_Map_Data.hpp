//*********************************************************
//	Lane_Map_Data.hpp - network lane connectivity data
//*********************************************************

#ifndef LANE_MAP_DATA_HPP
#define LANE_MAP_DATA_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Lane_Map_Data class definition
//---------------------------------------------------------

class SYSLIB_API Lane_Map_Data
{
public:
	Lane_Map_Data (void)           { Clear (); }

	int  In_Lane (void)            { return (in_lane); }
	int  In_Thru (void)            { return (in_thru); }
	int  Out_Lane (void)           { return (out_lane); }
	int  Out_Thru (void)           { return (out_thru); }

	void In_Lane (int value)       { in_lane = (char) value; }
	void In_Thru (int value)       { in_thru = (char) value; }
	void Out_Lane (int value)      { out_lane = (char) value; }
	void Out_Thru (int value)      { out_thru = (char) value; }

	void Clear (void) {	in_lane = in_thru = out_lane = out_thru = 0; }

private:
	char in_lane;
	char in_thru;
	char out_lane;
	char out_thru;
};

typedef vector <Lane_Map_Data>    Lane_Map_Array;
typedef Lane_Map_Array::iterator  Lane_Map_Itr;

#endif
