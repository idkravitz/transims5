//*********************************************************
//	Trip_Gap_Data.hpp - trip gap index and data
//*********************************************************

#ifndef TRIP_GAP_DATA_HPP
#define TRIP_GAP_DATA_HPP

#include "Trip_Index.hpp"
#include "Partition_Files.hpp"

#include <map>
using namespace std;

typedef struct {
	Dtime time;
	int   current;
	int   previous;
} Trip_Gap_Data;

//---- trip gap_map ----

typedef map <Trip_Index, Trip_Gap_Data>    Trip_Gap_Map;
typedef pair <Trip_Index, Trip_Gap_Data>   Trip_Gap_Map_Data;
typedef Trip_Gap_Map::iterator             Trip_Gap_Map_Itr;
typedef pair <Trip_Gap_Map_Itr, bool>      Trip_Gap_Map_Stat;
	
typedef Partition_Data <Trip_Gap_Map>      Trip_Gap_Map_Array;

#endif

