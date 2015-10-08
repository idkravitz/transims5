//*********************************************************
//	Transfer_Data.hpp - transfers at transit stops
//*********************************************************

#ifndef TRANSFER_DATA_HPP
#define TRANSFER_DATA_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Transfer_Data class definition
//---------------------------------------------------------

class SYSLIB_API Transfer_Data
{
public:
	Transfer_Data (void)          { Clear (); }

	int  Walk_Board (void)        { return (walk_board); }
	int  Bus_Board (void)         { return (bus_board); }
	int  Rail_Board (void)        { return (rail_board); }
	int  Drive_Board (void)       { return (drive_board); }
	int  Walk_Alight (void)       { return (walk_alight); }
	int  Bus_Alight (void)        { return (bus_alight); }
	int  Rail_Alight (void)       { return (rail_alight); }
	int  Drive_Alight (void)      { return (drive_alight); }

	void Walk_Board (int value)   { walk_board = value; }
	void Bus_Board (int value)    { bus_board = value; }
	void Rail_Board (int value)   { rail_board = value; }
	void Drive_Board (int value)  { drive_board = value; }
	void Walk_Alight (int value)  { walk_alight = value; }
	void Bus_Alight (int value)   { bus_alight = value; }
	void Rail_Alight (int value)  { rail_alight = value; }
	void Drive_Alight (int value) { drive_alight = value; }

	void Add_Walk_Board (void)    { walk_board++; }
	void Add_Bus_Board (void)     { bus_board++; }
	void Add_Rail_Board (void)    { rail_board++; }
	void Add_Drive_Board (void)   { drive_board++; }
	void Add_Walk_Alight (void)   { walk_alight++; }
	void Add_Bus_Alight (void)    { bus_alight++; }
	void Add_Rail_Alight (void)   { rail_alight++; }
	void Add_Drive_Alight (void)  { drive_alight++; }

	void Sum_Data (Transfer_Data &data);

	void Clear (void)
	{
		walk_board = bus_board = rail_board = drive_board = 0;
		walk_alight = bus_alight = rail_alight = drive_alight = 0;
	}
private:
	int  walk_board;
	int  bus_board;
	int  rail_board;
	int  drive_board;
	int  walk_alight;
	int  bus_alight;
	int  rail_alight;
	int  drive_alight;
};

class SYSLIB_API Transfer_Array : public vector <Transfer_Data> 
{
public:
	Transfer_Array (void) {}

	void Replicate (Transfer_Array &transfer_array);

	void Merge_Data (Transfer_Array &transfer_array);
};
typedef Transfer_Array::iterator  Transfer_Itr;

#endif
