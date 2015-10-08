//*********************************************************
//	Zone_ODT_Data.hpp - zone O-D-T data classes
//*********************************************************

#ifndef ZONE_ODT_DATA_HPP
#define ZONE_ODT_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "TypeDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Zone_ODT_Data class definition
//---------------------------------------------------------

class SYSLIB_API Zone_ODT_Data
{
public:
	Zone_ODT_Data (void)             { Clear (); }

	int   Index (void)               { return (index); }
	int   Origin (void)              { return (od.origin); }
	int   Destination (void)         { return (od.destination); }

	void  Index (int value)          { index = value; }
	void  Origin (int value)         { od.origin = (unsigned short) value; }
	void  Destination (int value)    { od.destination = (unsigned short) value; }
	
	void  Clear (void)               { index = 0; }

private:
	union {
		int index;
		struct {
			unsigned short origin;
			unsigned short destination;
		} od;
	};
};

//---- zone odt array ----

typedef vector <Zone_ODT_Data>    Zone_ODT_Array;
typedef Zone_ODT_Array::iterator  Zone_ODT_Itr;

//---------------------------------------------------------
//	Zone_Sum_Data class definition
//---------------------------------------------------------

class SYSLIB_API Zone_Sum_Data
{
public:
	Zone_Sum_Data (void)             { Clear (); }

	double Sum (void)                { return (sum); }
	int    Count (void)              { return (count); }

	void   Sum (double value)        { sum = value; }
	void   Count (int value)         { count = value; }
	
	void   Clear (void)              { count = 0; sum = 0.0; }

	void   Add_Value (double value)  { sum += value; count++; }
	double Average (void)            { return ((count) ? sum / count : 0.0); }

private:
	double sum;
	int    count;
};

//---- zone sum array ----

typedef vector <Zone_Sum_Data>    Zone_Sum_Array;
typedef Zone_Sum_Array::iterator  Zone_Sum_Itr;

//---------------------------------------------------------
//	Zone_Average_Data class definition
//---------------------------------------------------------

class SYSLIB_API Zone_Average_Data : public Zone_ODT_Data, public Zone_Sum_Array
{
public:
	Zone_Average_Data (void)         { Clear (); }
	
	void  Clear (void)               { Zone_ODT_Data::Clear (); clear (); }
};

//---- zone average array ----

typedef vector <Zone_Average_Data>   Zone_Average_Array;
typedef Zone_Average_Array::iterator Zone_Average_Itr;

//---------------------------------------------------------
//	Zone_Skim_Data class definition
//---------------------------------------------------------

class SYSLIB_API Zone_Skim_Data : public Zone_ODT_Data, public Dtimes
{
public:
	Zone_Skim_Data (void)            { Clear (); }
	
	void  Clear (void)               { Zone_ODT_Data::Clear (); clear (); }
};

//---- zone skim array ----

typedef vector <Zone_Skim_Data>      Zone_Skim_Array;
typedef Zone_Skim_Array::iterator    Zone_Skim_Itr;

//---------------------------------------------------------
//	Trip_Table_Data class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Table_Data : public Zone_ODT_Data, public Integers
{
public:
	Trip_Table_Data (void)           { Clear (); }
	
	void  Clear (void)               { Zone_ODT_Data::Clear (); clear (); }
};

//---- trip table array ----

typedef vector <Trip_Table_Data>      Trip_Table_Array;
typedef Trip_Table_Array::iterator    Trip_Table_Itr;

//---------------------------------------------------------
//	Data_Table_Data class definition
//---------------------------------------------------------

class SYSLIB_API Data_Table_Data : public Zone_ODT_Data, public Doubles
{
public:
	Data_Table_Data (void)           { Clear (); }
	
	void  Clear (void)               { Zone_ODT_Data::Clear (); clear (); }
};

//---- data table array ----

typedef vector <Data_Table_Data>      Data_Table_Array;
typedef Data_Table_Array::iterator    Data_Table_Itr;

#endif

