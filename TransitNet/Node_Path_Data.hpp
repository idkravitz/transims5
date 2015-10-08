//*********************************************************
//	Node_Path_Data.hpp - path building data between nodes
//*********************************************************

#ifndef NODE_PATH_DATA_HPP
#define NODE_PATH_DATA_HPP

#include "TypeDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Node_Path_Data class definition
//---------------------------------------------------------

class Node_Path_Data
{
public:
	Node_Path_Data (void)           { Clear (); }

	int   Distance (void)           { return (distance); }
	int   Next_List (void)          { return (next_list); }
	int   From_Node (void)          { return (from_node); }
	int   Dir_Index (void)          { return (dir_index); }

	void  Distance (int value)      { distance = value; }
	void  Next_List (int value)     { next_list = value; }
	void  From_Node (int value)     { from_node = value; }
	void  Dir_Index (int value)     { dir_index = value; }

	void  Clear (void)
	{
		distance = 0; next_list = from_node = dir_index = -1;
	}
private:
	int   distance;
	int   next_list;
	int   from_node;
	int   dir_index;
};

typedef vector <Node_Path_Data>    Node_Path_Array;
typedef Node_Path_Array::iterator  Node_Path_Itr;

//---------------------------------------------------------
//	Path_Leg_Data class definition
//---------------------------------------------------------

class Path_Leg_Data
{
public:
	Path_Leg_Data (void)           { Clear (); }

	int   Node (void)              { return (node); }
	int   Dir_Index (void)         { return (dir_index); }

	void  Node (int value)         { node = value; }
	void  Dir_Index (int value)    { dir_index = value; }

	void  Clear (void)             { node = dir_index = -1; }

private:
	int   node;
	int   dir_index;
};

typedef vector <Path_Leg_Data>            Path_Leg_Array;
typedef Path_Leg_Array::iterator          Path_Leg_Itr;
typedef Path_Leg_Array::reverse_iterator  Path_Leg_RItr;

//---------------------------------------------------------
//	Route_Path_Data class definition
//---------------------------------------------------------

class Route_Path_Data
{
public:
	Route_Path_Data (void)         { Clear (); }

	int   Node (void)              { return (node); }
	int   Dir_Index (void)         { return (dir_index); }
	Dtime Dwell (void)             { return (dwell); }
	Dtime TTime (void)             { return (ttime); }

	void  Node (int value)         { node = value; }
	void  Dir_Index (int value)    { dir_index = value; }
	void  Dwell (Dtime value)      { dwell = value; }
	void  TTime (Dtime value)      { ttime = value; }

	void  Clear (void)             { node = dir_index = -1; dwell = ttime = 0; }

private:
	int   node;
	int   dir_index;
	Dtime dwell;
	Dtime ttime;
};

typedef vector <Route_Path_Data>    Route_Path_Array;
typedef Route_Path_Array::iterator  Route_Path_Itr;

#endif
