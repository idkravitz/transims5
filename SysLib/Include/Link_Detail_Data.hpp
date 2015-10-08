//*********************************************************
//	Link_Detail_Data.hpp - network link direction details
//*********************************************************

#ifndef LINK_DETAIL_DATA_HPP
#define LINK_DETAIL_DATA_HPP

#include "APIDefs.hpp"
#include "Link_Dir_Data.hpp"
#include "Time_Periods.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Link_Detail_Data class definition
//---------------------------------------------------------

class SYSLIB_API Link_Detail_Data
{
public:
	Link_Detail_Data (void)                   { Clear (); }

	int   Link_Dir (void)                     { return (link_dir.Link_Dir ()); }
	int   Link (void)                         { return (link_dir.Link ()); }
	int   Dir (void)                          { return (link_dir.Dir ()); }
	int   Control (void)                      { return (control); }
	int   Group (void)                        { return (group); }
	int   LM_Length (void)                    { return (lm_length); }
	int   Left_Merge (void)                   { return (left_merge); }
	int   LT_Length (void)                    { return (lt_length); }
	int   Left (void)                         { return (left); }
	int   Left_Thru (void)                    { return (left_thru); }
	int   Thru (void)                         { return (thru); }
	int   Right_Thru (void)                   { return (right_thru); }
	int   Right (void)                        { return (right); }
	int   RT_Length (void)                    { return (rt_length); }
	int   Right_Merge (void)                  { return (right_merge); }
	int   RM_Length (void)                    { return (rm_length); }
	int   Use (void)                          { return (use); }
	int   Low_Lane (void)                     { return (low_lane); }
	int   High_Lane (void)                    { return (high_lane); }
	Time_Periods *Period (void)               { return (&period); }

	void  Link_Dir (int value)                { link_dir.Link_Dir (value); }
	void  Link (int value)                    { link_dir.Link (value); }
	void  Dir (int value)                     { link_dir.Dir (value); }
	void  Control (int value)                 { control = (char) value; }
	void  Group (int value)                   { group = (char) value; }
	void  LM_Length (int value)               { lm_length = (short) value; }
	void  Left_Merge (int value)              { left_merge = (char) value; }
	void  LT_Length (int value)               { lt_length = (short) value; }
	void  Left (int value)                    { left = (char) value; }
	void  Left_Thru (int value)               { left_thru = (char) value; }
	void  Thru (int value)                    { thru = (char) value; }
	void  Right_Thru (int value)              { right_thru = (char) value; }
	void  Right (int value)                   { right = (char) value; }
	void  RT_Length (int value)               { rt_length = (short) value; }
	void  Right_Merge (int value)             { right_merge = (char) value; }
	void  RM_Length (int value)               { rm_length = (short) value; }
	void  Use (int value)                     { use = (short) value; }
	void  Low_Lane (int value)                { low_lane = (char) value; }
	void  High_Lane (int value)               { high_lane = (char) value; }
	void  Period (string value)               { period.Add_Ranges (value); }
	
	void  LM_Length (double value)            { lm_length = (short) exe->Round (value); }
	void  LT_Length (double value)            { lt_length = (short) exe->Round (value); }
	void  RT_Length (double value)            { rt_length = (short) exe->Round (value); }
	void  RM_Length (double value)            { rm_length = (short) exe->Round (value); }

	void Clear (void)
	{
		Link_Dir (0); group = 0;
		control = left_merge = left = left_thru = thru = right_thru = right = right_merge = low_lane = high_lane = 0;
		lm_length = lt_length = rt_length = rm_length = use = 0;
	}
private:

	Link_Dir_Data link_dir;
	int           group;
	char          control;
	char          left_merge;
	char          left;
	char          left_thru;
	char          thru;
	char          right_thru;
	char          right;
	char          right_merge;
	short         lm_length;
	short         lt_length;
	short         rt_length;
	short         rm_length;
	short         use;
	char          low_lane;
	char          high_lane;
	Time_Periods  period;
};

typedef vector <Link_Detail_Data>     Link_Detail_Array;
typedef Link_Detail_Array::iterator   Link_Detail_Itr;

typedef map <int, Link_Detail_Data>       Link_Detail_Map;
typedef pair <int, Link_Detail_Data>      Link_Detail_Map_Data;
typedef Link_Detail_Map::iterator         Link_Detail_Map_Itr;
typedef pair <Link_Detail_Map_Itr, bool>  Link_Detail_Map_Stat;

#endif
