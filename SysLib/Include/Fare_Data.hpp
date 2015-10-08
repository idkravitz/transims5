//*********************************************************
//	Fare_Data.hpp - network transit fare data
//*********************************************************

#ifndef FARE_DATA_HPP
#define FARE_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"
#include "Time_Periods.hpp"

#include <map>
using namespace std;

//---------------------------------------------------------
//	Fare_Index class definition
//---------------------------------------------------------

class SYSLIB_API Fare_Index
{
public:
	Fare_Index (void)               { Clear (); }

	int   From_Zone (void)          { return (from_zone); }
	int   To_Zone (void)            { return (to_zone); }
	int   From_Mode (void)          { return (from_mode); }
	int   To_Mode (void)            { return (to_mode); }
	int   Period (void)             { return (period); }
	int   Class (void)              { return (type); }

	void  From_Zone (int value)     { from_zone = (short) value; }
	void  To_Zone (int value)       { to_zone = (short) value; }
	void  From_Mode (int value)     { from_mode = (char) value; }
	void  To_Mode (int value)       { to_mode = (char) value; }
	void  Period (int value)        { period = (char) value; }
	void  Class (int value)         { type = (char) value; }
	
	void Clear (void) 
	{
		from_zone = to_zone = 0; from_mode = to_mode = period = type = 0;
	}
private:
	short from_zone;	
	short to_zone;
	char  from_mode;
	char  to_mode;
	char  period;
	char  type;
};

//---------------------------------------------------------
//	Fare_Data class definition
//---------------------------------------------------------

class SYSLIB_API Fare_Data : public Notes_Data
{
public:
	Fare_Data (void)                      { Clear (); }

	String From_Zone (void)               { return (from_zone); }
	String To_Zone (void)                 { return (to_zone); }
	String From_Mode (void)               { return (from_mode); }
	String To_Mode (void)                 { return (to_mode); }
	String Period (void)                  { return (period); }
	String Class (void)                   { return (type); }
	int    Fare (void)                    { return (fare); }

	void  From_Zone (const char * value)  { from_zone = value; }
	void  From_Zone (string value)        { from_zone = value; }
	void  To_Zone (const char * value)    { to_zone = value; }
	void  To_Zone (string value)          { to_zone = value; }
	void  From_Mode (const char * value)  { from_mode = value; }
	void  From_Mode (string value)        { from_mode = value; }
	void  To_Mode (const char * value)    { to_mode = value; }
	void  To_Mode (string value)          { to_mode = value; }
	void  Period (const char * value)     { period = value; }
	void  Period (string value)           { period = value; }
	void  Class (const char * value)      { type = value; }
	void  Class (string value)            { type = value; }
	void  Fare (int value)                { fare = value; }
	
	void  Clear (void)
	{
		from_zone.clear (); to_zone.clear (); from_mode.clear (); to_mode.clear (); period.clear (); type.clear (); 
		fare = 0; Notes_Data::Clear (); 
	}
private:
	String from_zone;
	String to_zone;
	String from_mode;
	String to_mode;
	String period;
	String type;
	int    fare;
};

//---------------------------------------------------------
//	Fare_Map class definition
//---------------------------------------------------------

class SYSLIB_API Fare_Map : public map <Fare_Index, int>
{
public:
	Fare_Map (void) {}

	int    Period (Dtime time)            { return (fare_periods.Period (time)); }
	bool   Periods (string &breaks)       { return (fare_periods.Add_Breaks (breaks)); }
	string Period_String (void)           { return (fare_periods.Break_String ()); }

	//int  Num_Periods (void)              { return (fare_periods.Num_Records ()); }
	//int  Time_Step (char *time)          { return (fare_periods.Step (time)); }
	//char *Period_Format (int num)        { return (fare_periods.Range_Format (num)); }

private:
	Time_Periods fare_periods;
};

//---- fare acess map ----

inline bool operator < (const Fare_Index &left, const Fare_Index &right) 
{
	return (memcmp (&left, &right, sizeof (left)) < 0); 
}
typedef pair <Fare_Index, int>       Fare_Map_Data;
typedef Fare_Map::iterator           Fare_Map_Itr;
typedef pair <Fare_Map_Itr, bool>    Fare_Map_Stat;

#endif
