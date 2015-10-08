//*********************************************************
//	Line_Data.hpp - network transit route data
//*********************************************************

#ifndef LINE_DATA_HPP
#define LINE_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Buffer.hpp"
#include "Driver_Data.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Line_Run class definition
//---------------------------------------------------------

class SYSLIB_API Line_Run
{
public:
	Line_Run (void)                   { Clear (); }

	Dtime  Schedule (void)            { return (schedule); }
	Dtime  Time (void)                { return (time); }
	int    Board (void)               { return (board); }
	int    Alight (void)              { return (alight); }
	int    Load (void)                { return (load); }
	int    Factor (void)              { return (factor); }
	int    Off_Rider (void)           { return (off_rider); }
	int    Penalty (void)             { return (penalty); }

	void   Schedule (Dtime value)     { schedule = value; }
	void   Time (Dtime value)         { time = value; }
	void   Board (int value)          { board = (short) value; }
	void   Alight (int value)         { alight = (short) value; }
	void   Load (int value)           { load = (short) value; }
	void   Factor (int value)         { factor = (short) value; }
	void   Off_Rider (int value)      { off_rider = value; }
	void   Penalty (int value)        { penalty = value; }

	void   Add_Board (void)           { board++; }
	void   Add_Alight (void)          { alight++; }
	void   Add_Load (void)            { load++; }
	
	void   Clear (void)
	{
		schedule = time = 0; board = alight = load = factor = 0; off_rider = -1;
	}
private:
	Dtime  schedule;
	Dtime  time;
	short  board;
	short  alight;
	short  load;
	short  factor;
	union {
		int off_rider;
		int penalty;
	};
};

typedef vector <Line_Run>                 Line_Run_Array;
typedef Line_Run_Array::iterator          Line_Run_Itr;
typedef Line_Run_Array::reverse_iterator  Line_Run_RItr;

//---------------------------------------------------------
//	Line_Stop class definition
//---------------------------------------------------------

class SYSLIB_API Line_Stop : public Line_Run_Array
{
public:
	Line_Stop (void)                 { Clear (); }

	int   Stop (void)                { return (stop); }
	int   Zone (void)                { return (zone); }
	bool  Time_Flag (void)           { return (flag > 0); }
	int   Length (void)              { return (length); }
	int   On_Rider (void)            { return (on_rider); }
	
	void  Stop (int value)           { stop = value; }
	void  Zone (int value)           { zone = (unsigned short) value; }
	void  Time_Flag (bool value)     { flag = (unsigned short) value; }
	void  Length (int value)         { length = value; }
	void  On_Rider (int value)       { on_rider = value; }
	
	void  Clear (void)
	{
		stop = length = 0, zone = flag = 0; on_rider = -1; clear ();
	}
private:
	int            stop;
	unsigned short zone;
	unsigned short flag;
	int            length;
	int            on_rider;
};

typedef vector <Line_Stop>                 Line_Stop_Array;
typedef Line_Stop_Array::iterator          Line_Stop_Itr;
typedef Line_Stop_Array::reverse_iterator  Line_Stop_RItr;

//---------------------------------------------------------
//	Line_Data class definition
//---------------------------------------------------------

class SYSLIB_API Line_Data : public Notes_Data, public Line_Stop_Array
{
public:
	Line_Data (void)             { Clear (); }

	int   Route (void)           { return (route); }
	int   Mode (void)            { return (mode); }
	int   Type (void)            { return (type); }
	char *Name (void)            { return (name.Data ()); }

	void  Route (int value)      { route = value; }
	void  Mode (int value)       { mode = (short) value; }
	void  Type (int value)       { type = (short) value; }
	void  Name (char *value)     { name.Data (value); }
	void  Name (string value)    { if (!value.empty ()) name.Data (value); else name.Clear (); }

	Driver_Array driver_array;
 
	void Clear (void)
	{
		route = 0; mode = type = 0; name.Size (0); driver_array.clear (); clear ();
		Notes_Data::Clear (); 
	}
private:
	int    route;
	short  mode;
	short  type;
	Buffer name;
};

//---------------------------------------------------------
//	Line_Array class definition
//---------------------------------------------------------

class SYSLIB_API Line_Array : public vector <Line_Data> 
{
public:
	Line_Array (void)                    { Clear (); }

	int  Num_Runs (void)                 { return (num_runs); }
	int  Max_Runs (void)                 { return (max_runs); }
	int  Vehicle_ID (void)               { return (veh_id); }
	int  Route_Records (void)            { return (route_records); }
	int  Schedule_Records (void)         { return (schedule_records); }
	int  Driver_Records (void)           { return (driver_records); }
	int  Route_Stops (void)              { return (route_stops); }

	int  Vehicle_ID (int route, int run) { return (veh_id + route * offset + run + 1); }
	int  Route (int vehicle_id)          { return ((vehicle_id - veh_id) / offset); }
	int  Run (int vehicle_id)            { return ((vehicle_id % offset) - 1); }

	void Num_Runs (int value)            { num_runs = value; }
	void Max_Runs (int value)            { max_runs = value; }
	int  Vehicle_ID (int vehicles);
	void Route_Records (int value)       { route_records = value; }
	void Schedule_Records (int value)    { schedule_records = value; }
	void Driver_Records (int value)      { driver_records = value; }
	void Route_Stops (int value)         { route_stops = value; }

	void Add_Route_Stops (int value)     { route_stops += value; }
	
	void Clear (void) 
	{
		num_runs = max_runs = veh_id = offset = route_records = schedule_records = driver_records = route_stops = 0;
		clear ();
	}
private:
	int num_runs;
	int max_runs;
	int veh_id;
	int offset;
	int route_records;
	int schedule_records;
	int driver_records;
	int route_stops;
};

typedef Line_Array::iterator          Line_Itr;
typedef Line_Array::reverse_iterator  Line_RItr;

#endif
