//*********************************************************
//	Control_Warrant.hpp - traffic control warrant data
//*********************************************************

#ifndef CONTROL_WARRANT_HPP
#define CONTROL_WARRANT_HPP
#include <map>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	Control_Warrant class definition
//---------------------------------------------------------

class Control_Warrant
{
public:
	Control_Warrant (void)                 { Clear (); }

	int   Control_Type (void)              { return (type); }
	int   Setback (void)                   { return (length); }
	int   Group (void)                     { return (group); }

	void  Control_Type (int value)         { type = value; }
	void  Setback (int value)              { length = value; }
	void  Group (int value)                { group = value; }

	void Clear (void)
	{
		type = length = group = 0;
	}
private:
	int  type;
	int  length;
	int  group;
};

typedef vector <Control_Warrant>         Control_Warrant_Array;
typedef Control_Warrant_Array::iterator  Control_Warrant_Itr;

//---------------------------------------------------------
//	Control_Index class definition
//---------------------------------------------------------

class Control_Index
{
public:
	Control_Index (void)           { Clear (); }

	int  Primary (void)            { return (primary); }
	int  Secondary (void)          { return (secondary); }
	int  Area_Type (void)          { return (area_type); }

	void Primary (int value)       { primary = (char) value; }
	void Secondary (int value)     { secondary = (char) value; }
	void Area_Type (int value)     { area_type = (short) value; }

	void Clear (void)
	{
		primary = secondary = 0; area_type = 0;
	}
private:
	char  primary;
	char  secondary;
	short area_type;
};

//---------------------------------------------------------
//	Control_Warrant_Map class definition
//---------------------------------------------------------

bool operator < (Control_Index left, Control_Index right);

typedef map <Control_Index, int>               Control_Warrant_Map;
typedef pair <Control_Index, int>              Control_Warrant_Map_Data;
typedef Control_Warrant_Map::iterator          Control_Warrant_Map_Itr;
typedef pair <Control_Warrant_Map_Itr, bool>   Control_Warrant_Map_Stat;

#endif
