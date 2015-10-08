//*********************************************************
//	Vehicle_Data.hpp - vehicle data classes
//*********************************************************

#ifndef VEHICLE_DATA_HPP
#define VEHICLE_DATA_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
//#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Vehicle_Data class definition
//---------------------------------------------------------

class SYSLIB_API Vehicle_Data //: public Notes_Data
{
public:
	Vehicle_Data (void)            { Clear (); }

	int  Household (void)          { return (hhold); }
	int  Vehicle (void)            { return (vehicle); }
	int  Type (void)               { return (type); }
	int  Parking (void)            { return (parking); }
	int  Partition (void)          { return (partition); }

	void Household (int value)     { hhold = value; }
	void Vehicle (int value)       { vehicle = (unsigned char) value; }
	void Type (int value)          { type = (unsigned char) value; }
	void Parking (int value)       { parking = value; }
	void Partition (int value)     { partition = (short) value; }

	void Clear (void)
	{
		hhold = 0; parking = -1; vehicle = type = 0; partition = 0;
		//Notes_Data::Clear ();
	}
private:
	int           hhold;
	int           parking;
	unsigned char vehicle;
	unsigned char type;
	short         partition;
};

typedef vector <Vehicle_Data>    Vehicle_Array;
typedef Vehicle_Array::iterator  Vehicle_Itr;

//---------------------------------------------------------
//	Vehicle_Index class definition
//---------------------------------------------------------

class SYSLIB_API Vehicle_Index : public Int2_Key
{
public:
	Vehicle_Index (void)           { Clear (); }

	int  Household (void)          { return (first); }
	int  Vehicle (void)            { return (second); }

	void Household (int value)     { first = value; }
	void Vehicle (int value)       { second = value; }
	
	void Clear (void)              { first = second = 0; }
};

//---- vehicle map ----

typedef map <Vehicle_Index, int>       Vehicle_Map;
typedef pair <Vehicle_Index, int>      Vehicle_Map_Data;
typedef Vehicle_Map::iterator          Vehicle_Map_Itr;
typedef pair <Vehicle_Map_Itr, bool>   Vehicle_Map_Stat;

typedef HASH <Vehicle_Index, int>      Vehicle_Hash;
typedef pair <Vehicle_Index, int>      Vehicle_Hash_Data;
typedef Vehicle_Hash::iterator         Vehicle_Hash_Itr;
typedef pair <Vehicle_Hash_Itr, bool>  Vehicle_Hash_Stat;

#endif
