//*********************************************************
//	Household_Data.hpp - household data classes
//*********************************************************

#ifndef HOUSEHOLD_DATA_HPP
#define HOUSEHOLD_DATA_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
//#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Person_Data class definition
//---------------------------------------------------------

class SYSLIB_API Person_Data
{
public:
	Person_Data (void)             { Clear (); }

	int  Person (void)             { return (person); }
	int  Age (void)                { return (age); }
	int  Relate (void)             { return (relate); }
	int  Gender (void)             { return (gender); }
	int  Work (void)               { return (work); }
	int  Drive (void)              { return (drive); }

	void Person (int value)        { person = (short) value; }
	void Age (int value)           { age = (short) value; }
	void Relate (int value)        { relate = (char) value; }
	void Gender (int value)        { gender = (char) value; }
	void Work (int value)          { work = (char) value; }
	void Drive (int value)         { drive = (char) value; }
	
	void Clear (void)
	{
		person = age = 0; relate = gender = work = drive = 0;
	}
private:
	short person;
	short age;
	char  relate;
	char  gender;
	char  work;
	char  drive;
};

typedef vector <Person_Data>    Person_Array;
typedef Person_Array::iterator  Person_Itr;

//---------------------------------------------------------
//	Household_Data class definition
//---------------------------------------------------------

class SYSLIB_API Household_Data : public Person_Array  //, public Notes_Data 
{
public:
	Household_Data (void)           { Clear (); }

	int  Household (void)           { return (hhold); }
	int  Location (void)            { return (location); }
	int  Persons (void)             { return (persons); }
	int  Workers (void)             { return (workers); }
	int  Vehicles (void)            { return (vehicles); }
	int  Type (void)                { return (type); }
	int  Partition (void)           { return (partition); }

	void Household (int value)      { hhold = value; }
	void Location (int value)       { location = value; }
	void Persons (int value)        { persons = (short) value; }
	void Workers (int value)        { workers = (char) value; }
	void Vehicles (int value)       { vehicles = (char) value; }
	void Type (int value)           { type = (short) value; }
	void Partition (int value)      { partition = (short) value; }
	
	void Clear (void)
	{
		hhold = location = 0; persons = type = partition = 0; workers = vehicles = 0;;
		clear (); //Notes_Data::Clear (); 
	}
private:
	int   hhold;
	int   location;
	short persons;
	char  workers;
	char  vehicles;
	short type;
	short partition;
};

typedef vector <Household_Data>    Household_Array;
typedef Household_Array::iterator  Household_Itr;

//---------------------------------------------------------
//	Person_Index class definition
//---------------------------------------------------------

class SYSLIB_API Person_Index : public Int2_Key
{
public:
	Person_Index (void)                { Clear (); }

	int   Household (void)             { return (first); }
	int   Person (void)                { return (second); }

	void  Household (int value)        { first = value; }
	void  Person (int value)           { second = value; }
	
	void  Clear (void)                 { first = second = 0; }
};

//---- person map ----

typedef map <Person_Index, int>       Person_Map;
typedef pair <Person_Index, int>      Person_Map_Data;
typedef Person_Map::iterator          Person_Map_Itr;
typedef pair <Person_Map_Itr, bool>   Person_Map_Stat;

#endif
