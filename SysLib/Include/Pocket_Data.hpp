//*********************************************************
//	Pocket_Data.hpp - network pocket lane data
//*********************************************************

#ifndef POCKET_DATA_HPP
#define POCKET_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Pocket_Data class definition
//---------------------------------------------------------

class SYSLIB_API Pocket_Data : public Notes_Data
{
public:
	Pocket_Data (void)           { Clear (); }

	int  Dir_Index (void)        { return (dir_index); }
	int  Type (void)             { return (type); }
	int  Lanes (void)            { return (lanes); }
	int  Length (void)           { return (length); } 
	int  Offset (void)           { return (offset); }
	int  Next_Index (void)       { return (next_index); }

	void Dir_Index (int value)   { dir_index = value; }
	void Type (int value)        { type = (short) value; }
	void Lanes (int value)       { lanes = (short) value; }
	void Length (int value)      { length = value; }
	void Offset (int value)      { offset = value; }
	void Next_Index (int value)  { next_index = value; }
	
	void Length (double value)   { length = exe->Round (value); }
	void Offset (double value)   { offset = exe->Round (value); }

	void Clear (void)
	{
		dir_index = next_index = -1; type = lanes = 0; length = offset = 0; Notes_Data::Clear ();
	}
private:
	int   dir_index;
	short type;
	short lanes;
	int   length; 
	int   offset;
	int   next_index;
};

typedef vector <Pocket_Data>    Pocket_Array;
typedef Pocket_Array::iterator  Pocket_Itr;

#endif
