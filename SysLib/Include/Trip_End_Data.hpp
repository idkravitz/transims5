//*********************************************************
//	Trip_End_Data.hpp - trip end data classes
//*********************************************************

#ifndef TRIP_END_DATA_HPP
#define TRIP_END_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Path_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Trip_End class definition
//---------------------------------------------------------

class SYSLIB_API Trip_End
{
public:
	Trip_End (void)              { Clear (); }

	int   Type (void)            { return (type); }
	int   Index (void)           { return (index); }
	Dtime Time (void)            { return (time); }
	int   Best (void)            { return (best); }

	void  Type (int value)       { type = value; }
	void  Index (int value)      { index = value; }
	void  Time (Dtime value)     { time = value; }
	void  Best (int value)       { best = value; }

	void  Clear (void)           { type = index = 0; time = 0; best = -1; }

private:
	int   type;
	int   index;
	Dtime time;
	int   best;
};

typedef vector <Trip_End>         Trip_End_Array;
typedef Trip_End_Array::iterator  Trip_End_Itr;

//---------------------------------------------------------
//	Path_End class definition
//---------------------------------------------------------

class SYSLIB_API Path_End : public Path_Array
{
public:
	Path_End (void)               { Clear (); }

	int  Trip_End (void)          { return (num); }
	int  End_Type (void)          { return (end_type); }
	int  Type (void)              { return (type); }
	int  Index (void)             { return (index); }
	int  Offset (void)            { return (offset); }

	void Trip_End (int value)     { num = value; }
	void End_Type (int value)     { end_type = (short) value; }
	void Type (int value)         { type = (short) value; }
	void Index (int value)        { index = value; }
	void Offset (int value)       { offset = value; }

	void Clear (void)
	{
		num = index = offset = 0; end_type = type = 0; Path_Array::clear ();;
	}

private:
	int   num;
	short end_type;
	short type;
	int   index;
	int   offset;
};

typedef vector <Path_End>                 Path_End_Array;
typedef Path_End_Array::iterator          Path_End_Itr;

#endif
