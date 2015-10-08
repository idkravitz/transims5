//*********************************************************
//	Problem_Data.hpp - problem data classes
//*********************************************************

#ifndef PROBLEM_DATA_HPP
#define PROBLEM_DATA_HPP

#include "APIDefs.hpp"
#include "Trip_Data.hpp"
#include "Notes_Data.hpp"
#include "Link_Dir_Data.hpp"
#include "Execution_Service.hpp"

#include <vector>
using namespace std;

///---------------------------------------------------------
//	Problem_Data class definition
//---------------------------------------------------------

class SYSLIB_API Problem_Data : public Trip_Data, public Notes_Data
{
public:
	Problem_Data (void)                     { Clear (); }

	int    Problem (void)                   { return (problem); }
	Dtime  Time (void)                      { return (time); }
	int    Dir_Index (void)                 { return (dir_index); }
	int    Lane (void)                      { return (lane); }
	int    Offset (void)                    { return (offset); }
	int    Route (void)                     { return (route); }
	int    Survey (void)                    { return (survey); }
	
	void   Problem (int value)              { problem = (short) value; }
	void   Time (Dtime value)               { time = value; }
	void   Dir_Index (int value)            { dir_index = value; }
	void   Lane (int value)                 { lane = (short) value; }
	void   Offset (int value)               { offset = value; }
	void   Route (int value)                { route = value; }
	void   Survey (int value)               { survey = value; }
	
	void   Offset (double value)            { offset = exe->Round (value); }

	void   Clear (void)
	{
		problem = lane = 0; time = 0; dir_index = route = -1; offset = survey = 0;
		Trip_Data::Clear (); Notes_Data::Clear ();
	}

private:
	short      problem;
	short      lane;
	Dtime      time;
	int        dir_index;
	int        offset;
	int        route;
	int        survey;
};

typedef vector <Problem_Data>     Problem_Array;
typedef Problem_Array::iterator   Problem_Itr;

#endif
