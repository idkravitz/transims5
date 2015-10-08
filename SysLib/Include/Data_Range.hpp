//*********************************************************
//	Data_Range.hpp - data range class
//*********************************************************

#ifndef DATA_RANGE_HPP
#define DATA_RANGE_HPP

#include "APIDefs.hpp"
#include "Range_Data.hpp"
#include "Static_Service.hpp"
#include "System_Defines.hpp"

#include <string>
#include <vector>
using namespace std;

//---------------------------------------------------------
//	Data_Range Class definition
//---------------------------------------------------------

class SYSLIB_API Data_Range : public Range_Array, public Static_Service
{
public:
	Data_Range (int factor = 1, int minimum = 0, int maximum = MAX_INTEGER, int increment = 0);

	void Setup (int factor = 1, int minimum = 0, int maximum = MAX_INTEGER, int increment = 0);

	bool Add_Ranges (string ranges);
	bool Add_Breaks (string breaks);

private:
	int factor, minimum, maximum, increment;
};

//---- array of Data Ranges ----

typedef vector <Data_Range>          Data_Ranges;
typedef Data_Ranges::iterator        Data_Range_Itr;

#endif
