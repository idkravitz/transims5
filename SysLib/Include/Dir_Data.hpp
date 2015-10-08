//*********************************************************
//	Dir_Data.hpp - network link direction data
//*********************************************************

#ifndef DIR_DATA_HPP
#define DIR_DATA_HPP

#include "APIDefs.hpp"
#include "Link_Dir_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Dir_Data class definition
//---------------------------------------------------------

class SYSLIB_API Dir_Data
{
public:
	Dir_Data (void)                           { Clear (); }

	int   Link_Dir (void)                     { return (link_dir.Link_Dir ()); }
	int   Link (void)                         { return (link_dir.Link ()); }
	int   Dir (void)                          { return (link_dir.Dir ()); }
	int   Lanes (void)                        { return (lanes); }
	int   Left (void)                         { return (left); }
	int   Right (void)                        { return (right); }
	int   Speed (void)                        { return (speed); }
	int   Capacity (void)                     { return (capacity); }
	int   In_Bearing (void)                   { return (in_bear); }
	int   Out_Bearing (void)                  { return (out_bear); }
	Dtime Time0 (void)                        { return (time0); }
	int   Sign (void)                         { return (sign); }

	int   First_Pocket (void)                 { return (first_pocket); }
	int   First_Lane_Use (void)               { return (first_lane_use); }
	int   First_Connect_To (void)             { return (connect_to); }
	int   First_Connect_From (void)           { return (connect_from); }
	int   First_Turn_To (void)                { return (turn_to); }
	int   First_Turn_From (void)              { return (turn_from); }
	int   First_Connect (bool to_flag = true) { return ((to_flag) ? connect_to : connect_from); }
	int   First_Turn (bool to_flag = true)    { return ((to_flag) ? turn_to : turn_from); } 
	int   Flow_Index (void)                   { return (flow_index); }

	void  Link_Dir (int value)                { link_dir.Link_Dir (value); }
	void  Link (int value)                    { link_dir.Link (value); }
	void  Dir (int value)                     { link_dir.Dir (value); }
	void  Lanes (int value)                   { lanes = (char) value; }
	void  Left (int value)                    { left = (char) value; }
	void  Right (int value)                   { right = (char) value; }
	void  Speed (int value)                   { speed = (short) value; }
	void  Capacity (int value)                { capacity = (short) value; }
	void  In_Bearing (int value)              { in_bear = (short) value; }
	void  Out_Bearing (int value)             { out_bear = (short) value; }
	void  Time0 (Dtime value)                 { time0 = value; }
	void  Sign (int value)                    { sign = (char) value; }
	
	void  First_Pocket (int value)            { first_pocket = value; }
	void  First_Lane_Use (int value)          { first_lane_use = value; }
	void  First_Connect_To (int value)        { connect_to = value; }
	void  First_Connect_From (int value)      { connect_from = value; }
	void  First_Turn_To (int value)           { turn_to = value; }
	void  First_Turn_From (int value)         { turn_from = value; }
	void  Flow_Index (int value)              { flow_index = value; }
	
	void  Speed (double value)                { speed = (short) exe->Round (value); }
	void  Time0 (int value)                   { time0 = value; }
	void  Time0 (double value)                { time0 = exe->Round (value); }

	void Clear (void)
	{
		Link_Dir (0); lanes = left = right = sign = 0; speed = capacity = in_bear = out_bear = 0; time0 = 0; 
		first_pocket = first_lane_use = connect_to = connect_from = turn_to = turn_from = flow_index = -1;
	}
private:

	Link_Dir_Data link_dir;
	char          lanes;
	char          left;
	char          right;
	char          sign;
	short         speed;
	short         capacity;
	short         in_bear;
	short         out_bear;
	Dtime         time0;
	int           first_pocket;
	int           first_lane_use;
	int           connect_to;
	int           connect_from;
	int           turn_to;
	int           turn_from;
	int           flow_index;
};

typedef vector <Dir_Data>    Dir_Array;
typedef Dir_Array::iterator  Dir_Itr;

#endif
