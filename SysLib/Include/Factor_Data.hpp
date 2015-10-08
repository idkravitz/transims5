//*********************************************************
//	Factor_Data.hpp - factor data classes
//*********************************************************

#ifndef FACTOR_DATA_HPP
#define FACTOR_DATA_HPP

#include "APIDefs.hpp"
#include "Matrix_Data.hpp"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Factor_Period class definition
//---------------------------------------------------------

class SYSLIB_API Factor_Period
{
public:
	Factor_Period (void)           { Clear (); }

	double Factor (void)           { return (factor); }
	double Bucket (void)           { return (bucket); }

	void   Factor (double value)   { factor = (float) value; }
	void   Bucket (double value)   { bucket = (float) value; }

	int Bucket_Factor (double value)
	{
		value = value * factor + bucket;
		int lvalue = (int) value;
		bucket = (float) (value - lvalue);
		return (lvalue);
	}
	void Clear (void)              { factor = (float) 0.0; bucket = (float) 0.45; }

private:
	float factor;
	float bucket;
};

//---- factor array ----

typedef vector <Factor_Period>    Factor_Periods;
typedef Factor_Periods::iterator  Factor_Period_Itr;

//---- factor table ----

typedef map <Matrix_Index, Factor_Periods>   Factor_Table;
typedef pair <Matrix_Index, Factor_Periods>  Factor_Table_Data;
typedef Factor_Table::iterator               Factor_Table_Itr;
typedef pair <Factor_Table_Itr, bool>        Factor_Table_Stat;

//---------------------------------------------------------
//	Factor_Data class definition
//---------------------------------------------------------

class SYSLIB_API Factor_Data
{
public:
	Factor_Data (void)              { Clear (); }

	int    Origin (void)            { return (origin); }
	int    Destination (void)       { return (destination); }
	int    Period (void)            { return (period); }
	double Factor (void)            { return (factor); }

	void   Origin (int value)       { origin = value; }
	void   Destination (int value)  { destination = value; }
	void   Period (int value)       { period = value; }
	void   Factor (double value)    { factor = value; }
	
	void  Clear (void)
	{
		origin = destination = period = 0; factor = 1.0;
	}
private:
	int    origin;
	int    destination;
	int    period;
	double factor;
};

#endif

