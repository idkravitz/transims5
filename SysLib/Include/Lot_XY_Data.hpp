//*********************************************************
//	Lot_XY_Data.hpp - XY location of parking lots
//*********************************************************

#ifndef LOT_XY_DATA_HPP
#define LOT_XY_DATA_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Lot_XY_Data class definition
//---------------------------------------------------------

class SYSLIB_API Lot_XY_Data
{
public:
	Lot_XY_Data (void)          { Clear (); }

	int  Lot (void)             { return (lot); }
	int  X (void)               { return (x); }
	int  Y (void)               { return (y); }

	void Lot (int value)        { lot = value; }
	void X (int value)          { x = value; }
	void Y (int value)          { y = value; }

	void Clear (void)           { lot = x = y = 0; }

private:
	int lot;
	int x;
	int y;
};

typedef vector <Lot_XY_Data>    Lot_XY_Array;
typedef Lot_XY_Array::iterator  Lot_XY_Itr;

#endif
