//*********************************************************
//	Pocket_Warrant.hpp - pocket lane warrant data
//*********************************************************

#ifndef POCKET_WARRANT_HPP
#define POCKET_WARRANT_HPP

#include <map>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	Pocket_Warrant class definition
//---------------------------------------------------------

class Pocket_Warrant
{
public:
	Pocket_Warrant (void)           { Clear (); }

	int   Left_Length (void)        { return (left_len); }
	int   Left_Lanes (void)         { return (left_lanes); }
	int   Right_Length (void)       { return (right_len); }
	int   Right_Lanes (void)        { return (right_lanes); }
	int   Merge_Length (void)       { return (merge_len); }
	int   Merge_Lanes (void)        { return (merge_lanes); }

	void  Left_Length (int value)   { left_len = value; }	
	void  Left_Lanes (int value)    { left_lanes = value; }
	void  Right_Length (int value)  { right_len = value; }	
	void  Right_Lanes (int value)   { right_lanes = value; }
	void  Merge_Length (int value)  { merge_len = value; }	
	void  Merge_Lanes (int value)   { merge_lanes = value; }

	void  Clear (void)
	{
		left_len = right_len = merge_len = left_lanes = right_lanes = merge_lanes = 0;
	}
private:
	int  left_len;
	int  right_len;
	int  merge_len;
	int  left_lanes;
	int  right_lanes;
	int  merge_lanes;
};

typedef vector <Pocket_Warrant>         Pocket_Warrant_Array;
typedef Pocket_Warrant_Array::iterator  Pocket_Warrant_Itr;

//---------------------------------------------------------
//	Pocket_Index class definition
//---------------------------------------------------------

class Pocket_Index
{
public:
	Pocket_Index (void)             { Clear (); }

	int   From_Type (void)          { return (from_type); }
	int   To_Type (void)            { return (to_type); }
	int   Area_Type (void)          { return (area_type); }

	void  From_Type (int value)     { from_type = (char) value; }
	void  To_Type (int value)       { to_type = (char) value; }
	void  Area_Type (int value)     { area_type = (short) value; }

	void  Clear (void)              { from_type = to_type = 0; area_type = 0; }
private:
	char  from_type;
	char  to_type;
	short area_type;
};

//---------------------------------------------------------
//	Pocket_Warrant_Map class definition
//---------------------------------------------------------

bool operator < (Pocket_Index left, Pocket_Index right);

typedef map <Pocket_Index, int>               Pocket_Warrant_Map;
typedef pair <Pocket_Index, int>              Pocket_Warrant_Map_Data;
typedef Pocket_Warrant_Map::iterator          Pocket_Warrant_Map_Itr;
typedef pair <Pocket_Warrant_Map_Itr, bool>   Pocket_Warrant_Map_Stat;

#endif
