//*********************************************************
//	Access_Warrant.hpp - trip access warrant data
//*********************************************************

#ifndef ACCESS_WARRANT_HPP
#define ACCESS_WARRANT_HPP

#include <map>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	Access_Warrant class definition
//---------------------------------------------------------

class Access_Warrant
{
public:
	Access_Warrant (void)                  { Clear (); }

	int   Setback (void)                   { return (setback); }
	int   Max_Points (void)                { return (max_pts); }
	int   Min_Length (void)                { return (min_len); }

	void  Setback (int value)              { setback = (short) value; }
	void  Max_Points (int value)           { max_pts = (short) value; }
	void  Min_Length (int value)           { min_len = value; }

	void Clear (void)
	{
		setback = max_pts = 0; min_len = 0;
	}
private:
	short setback;
	short max_pts;
	int   min_len;
};

typedef vector <Access_Warrant>         Access_Warrant_Array;
typedef Access_Warrant_Array::iterator  Access_Warrant_Itr;

//---------------------------------------------------------
//	Access_Index class definition
//---------------------------------------------------------

class Access_Index
{
public:
	Access_Index (void)           { Clear (); }

	int  Facility (void)           { return (facility); }
	int  Area_Type (void)          { return (area_type); }

	void Facility (int value)      { facility = (short) value; }
	void Area_Type (int value)     { area_type = (short) value; }

	void Clear (void)
	{
		facility = area_type = 0;
	}
private:
	short facility;
	short area_type;
};

//---------------------------------------------------------
//	Access_Warrant_Map class definition
//---------------------------------------------------------

bool operator < (Access_Index left, Access_Index right);

typedef map <Access_Index, int>              Access_Warrant_Map;
typedef pair <Access_Index, int>             Access_Warrant_Map_Data;
typedef Access_Warrant_Map::iterator         Access_Warrant_Map_Itr;
typedef pair <Access_Warrant_Map_Itr, bool>  Access_Warrant_Map_Stat;

#endif
