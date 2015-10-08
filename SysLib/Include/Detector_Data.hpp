//*********************************************************
//	Detector_Data.hpp - network detector data
//*********************************************************

#ifndef DETECTOR_DATA_HPP
#define DETECTOR_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Detector_Data class definition
//---------------------------------------------------------

class SYSLIB_API Detector_Data : public Notes_Data
{
public:
	Detector_Data (void)           { Clear (); }

	int  Detector (void)           { return (detector); }
	int  Dir_Index (void)          { return (dir_index); }
	int  Offset (void)             { return (offset); }
	int  Length (void)             { return (length); }
	int  Low_Lane (void)           { return (low_lane); }
	int  High_Lane (void)          { return (high_lane); }
	int  Type (void)               { return (type); }
	int  Use (void)                { return (use); }

	void Detector (int value)      { detector = value; }
	void Dir_Index (int value)     { dir_index = value; }
	void Offset (int value)        { offset = value; }
	void Length (int value)        { length = (short) value; }
	void Low_Lane (int value)      { low_lane = (char) value; }
	void High_Lane (int value)     { high_lane = (char) value; }
	void Type (int value)          { type = (short) value; }
	void Use (int value)           { use = (short) value; }

	void Offset (double value)     { offset = exe->Round (value); }
	void Length (double value)     { length = (short) exe->Round (value); }
	
	void Clear (void) 
	{
		detector = offset = 0; dir_index = -1; length = type = use = 0; low_lane = high_lane = 0; Notes_Data::Clear (); 
	}
private:
	int        detector;
	int        dir_index;
	int        offset;
	short      length;
	char       low_lane;
	char       high_lane;
	short      type;
	short      use;
};

typedef vector <Detector_Data>    Detector_Array;
typedef Detector_Array::iterator  Detector_Itr;

#endif
