//*********************************************************
//	Matrix_Data.hpp - trip/factor/skim data classes
//*********************************************************

#ifndef MATRIX_DATA_HPP
#define MATRIX_DATA_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Dtime.hpp"

#include <map>
using namespace std;

//---------------------------------------------------------
//	Matrix_Index class definition
//---------------------------------------------------------

class SYSLIB_API Matrix_Index : public Int2_Key
{
public:
	Matrix_Index (void)              { Clear (); }

	int   Origin (void)              { return (first); }
	int   Destination (void)         { return (second); }

	void  Origin (int value)         { first = value; }
	void  Destination (int value)    { second = value; }
	
	void  Clear (void)               { first = second = 0; }
};

//---- matrix map ----

typedef map <Matrix_Index, int>        Matrix_Map;
typedef pair <Matrix_Index, int>       Matrix_Map_Data;
typedef Matrix_Map::iterator           Matrix_Map_Itr;
typedef pair <Matrix_Map_Itr, bool>    Matrix_Map_Stat;

//---- trip table ----

typedef map <Matrix_Index, Integers>   Trip_Table;
typedef pair <Matrix_Index, Integers>  Trip_Table_Data;
typedef Trip_Table::iterator           Trip_Table_Itr;
typedef pair <Trip_Table_Itr, bool>    Trip_Table_Stat;

//---- time table ----

typedef map <Matrix_Index, Dtimes>     Dtime_Table;
typedef pair <Matrix_Index, Dtimes>    Dtime_Table_Data;
typedef Dtime_Table::iterator          Dtime_Table_Itr;
typedef pair <Dtime_Table_Itr, bool>   Dtime_Table_Stat;

//---- trip table map ----

typedef map <int, Trip_Table>           Trip_Table_Map;
typedef pair <int, Trip_Table>          Trip_Table_Map_Data;
typedef Trip_Table_Map::iterator        Trip_Table_Map_Itr;
typedef pair <Trip_Table_Map_Itr, bool> Trip_Table_Map_Stat;

#endif

