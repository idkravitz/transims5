//*********************************************************
//	Sign_Data.hpp - network sign data
//*********************************************************

#ifndef SIGN_DATA_HPP
#define SIGN_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sign_Data class definition
//---------------------------------------------------------

class SYSLIB_API Sign_Data : public Notes_Data
{
public:
	Sign_Data (void)           { Clear (); }

	int  Dir_Index (void)      { return (dir_index); }
	int  Sign (void)           { return (sign); }

	void Dir_Index (int value) { dir_index = value; }
	void Sign (int value)      { sign = value; }

	void Clear (void)
	{
		dir_index = -1; sign = NO_CONTROL; Notes_Data::Clear ();
	}
private:
	int  dir_index;
	int  sign;
};

typedef vector <Sign_Data>    Sign_Array;
typedef Sign_Array::iterator  Sign_Itr;

#endif
