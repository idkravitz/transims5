//*********************************************************
//	Subzone_Data.hpp - subzone data
//*********************************************************

#ifndef SUBZONE_DATA_HPP
#define SUBZONE_DATA_HPP

#include "TypeDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Subzone_Data class definition
//---------------------------------------------------------

class Subzone_Data : public Notes_Data
{
public:
	Subzone_Data (void)          { Clear (); }

	int    ID (void)             { return (id); }
	int    Zone (void)           { return (zone); }
	int    X (void)              { return (x); }
	int    Y (void)              { return (y); }
	double Data (void)           { return (data); }

	void   ID (int value)        { id = value; }
	void   Zone (int value)      { zone = value; }
	void   X (int value)         { x = value; }
	void   Y (int value)         { y = value; }
	void   Data (double value)   { data = value; }

	void   X (double value)      { x = exe->Round (value); }
	void   Y (double value)      { y = exe->Round (value); }

	void   Clear (void)
	{
		id = zone = x = y = 0; data = 0; Notes_Data::Clear ();
	}
private:
	int    id;
	int    zone;
	int    x;				//---- rounded ----
	int    y;				//---- rounded ----
	double data;
};

typedef vector <Subzone_Data>    Subzone_Array;
typedef Subzone_Array::iterator  Subzone_Itr;

#endif
