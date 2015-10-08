//*********************************************************
//	Route_Nodes_Data.hpp - transit route nodes data
//*********************************************************

#ifndef ROUTE_NODES_DATA_HPP
#define ROUTE_NODES_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"
#include "Dtime.hpp"
#include "Buffer.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Route_Node class definition
//---------------------------------------------------------

class SYSLIB_API Route_Node
{
public:
	Route_Node (void)               { Clear (); }

	int    Node (void)              { return (node); }
	Dtime  Dwell (void)             { return (dwell); }
	Dtime  Time (void)              { return (time); }
	int    Speed (void)             { return (speed); }
	int    Type (void)              { return (type); }

	void   Node (int value)         { node = value; }
	void   Dwell (Dtime value)      { dwell = value; }
	void   Time (Dtime value)       { time = value; }
	void   Speed (int value)        { speed = (short) value; }
	void   Type (int value)         { type = (short) value; }

	void  Clear (void)              { node = 0; speed = type = 0; dwell = time = 0; }
	
	void  Speed (double value)      { speed = (short) exe->Round (value); }

private:
	int    node;
	Dtime  dwell;
	Dtime  time;
	short  speed;
	short  type;
};

typedef vector <Route_Node>         Route_Node_Array;
typedef Route_Node_Array::iterator  Route_Node_Itr;

//---------------------------------------------------------
//	Route_Period class definition
//---------------------------------------------------------

class SYSLIB_API Route_Period
{
public:
	Route_Period (void)            { Clear (); }

	Dtime Headway (void)           { return (headway); }
	Dtime Offset (void)            { return (offset); }
	Dtime TTime (void)             { return (ttime); }

	void  Headway (Dtime value)    { headway = value; }
	void  Offset (Dtime value)     { offset = value; }
	void  TTime (Dtime value)      { ttime = value; }
	
	void  Clear (void)
	{
		headway = offset = ttime = 0;
	}
private:
	Dtime headway;
	Dtime offset;
	Dtime ttime;
};

typedef vector <Route_Period>         Route_Period_Array;
typedef Route_Period_Array::iterator  Route_Period_Itr;

//---------------------------------------------------------
//	Route_Header class definition
//---------------------------------------------------------

class SYSLIB_API Route_Header : public Notes_Data
{
public:
	Route_Header (void)           { Clear (); }

	int   Route (void)            { return (route); }
	int   Mode (void)             { return (mode); }
	int   Veh_Type (void)         { return (veh_type); }
	char *Name (void)             { return (name.Data ()); }

	void  Route (int value)       { route = value; }
	void  Mode (int value)        { mode = (short) value; }
	void  Veh_Type (int value)    { veh_type = (short) value; }
	void  Name (char *value)      { name.Data (value); }
	void  Name (string value)     { if (!value.empty ()) name.Data (value); else name.Clear (); }

	Route_Node_Array nodes;
	Route_Period_Array periods;

	void  Clear (void)
	{
		route = 0; mode = veh_type = 0; name.Size (0); nodes.clear (); periods.clear (); Notes_Data::Clear ();
	}
private:
	int    route;
	short  mode;
	short  veh_type;
	Buffer name;
};

typedef vector <Route_Header>        Route_Nodes_Array;
typedef Route_Nodes_Array::iterator  Route_Nodes_Itr;
#endif
