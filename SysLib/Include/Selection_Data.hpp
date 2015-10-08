//*********************************************************
//	Selection_Data.hpp - household selection classes
//*********************************************************

#ifndef SELECTION_DATA_HPP
#define SELECTION_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"
#include "Trip_Index.hpp"
#include "Data_Buffer.hpp"

#include <map>
using namespace std;

//---------------------------------------------------------
//	Select_Data class definition
//---------------------------------------------------------

class SYSLIB_API Select_Data
{
public:
	Select_Data (void)             { Clear (); }

	int  Type (void)               { return (type); }
	int  Partition (void)          { return (partition); }

	void Type (int value)          { type = (unsigned short) value; }
	void Partition (int value)     { partition = (short) value; }

	void Clear (void)              { type = partition = 0; }

private:
	unsigned short type;
	short partition;
};

//---- select map ----

class SYSLIB_API Select_Map : public map <Trip_Index, Select_Data> 
{
public:
	Select_Map (void)                 { max_part = max_hhold = num_hhold = 0; }

	Select_Map::iterator Best (int hhold, int person = 0, int tour = 0, int trip = 0);
	Select_Map::iterator Best (Trip_Index index)
	{
		return (Best (index.Household (), index.Person (), index.Tour (), index.Trip ())); 
	}

	int  Max_Partition (void)         { return (max_part); }
	void Max_Partition (int value)    { max_part = value; }

	int  Max_Household (void)         { return (max_hhold); }
	void Max_Household (int value)    { max_hhold = value; }

	int  Num_Household (void)         { return (num_hhold); }
	void Add_Household (void)         { num_hhold++; }

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data);
	bool UnPack (Data_Buffer &data);
#endif

private:
	int  max_part, max_hhold, num_hhold;
};

typedef pair <Trip_Index, Select_Data>    Select_Map_Data;
typedef Select_Map::iterator              Select_Map_Itr;
typedef pair <Select_Map_Itr, bool>       Select_Map_Stat;

//---------------------------------------------------------
//	Selection_Data class definition
//---------------------------------------------------------

class SYSLIB_API Selection_Data
{
public:
	Selection_Data (void)            { Clear (); }

	int  Household (void)            { return (hhold); }
	int  Person (void)               { return (person); }
	int  Tour (void)                 { return (tour); }
	int  Trip (void)                 { return (trip); }
	int  Type (void)                 { return (type); }
	int  Partition (void)            { return (partition); }

	void Household (int value)       { hhold = value; }
	void Person (int value)          { person = (short) value; }
	void Tour (int value)            { tour = (char) value; }
	void Trip (int value)            { trip = (char) value; }
	void Type (int value)            { type = (short) value; }
	void Partition (int value)       { partition = (short) value; }

	void Get_Trip_Index (Trip_Index &index) { index.Set (hhold, person, tour, trip); }

	void Clear (void)
	{
		hhold = 0; person = type = partition = 0; tour = trip = 0;
	}
private:
	int   hhold;
	short person;
	char  tour;
	char  trip;
	short type;
	short partition;
};

#endif
