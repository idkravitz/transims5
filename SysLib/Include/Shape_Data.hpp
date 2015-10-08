//*********************************************************
//	Shape_Data.hpp - shape point classes
//*********************************************************

#ifndef SHAPE_DATA_HPP
#define SHAPE_DATA_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "TypeDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Shape_Data class definition
//---------------------------------------------------------

class SYSLIB_API Shape_Data : public Notes_Data, public XYZ_Array
{
public:
	Shape_Data (void)                    { Clear (); }

	int  Link (void)                     { return (link); }
	void Link (int value)                { link = value; }

	void Clear (void)
	{
		link = 0; Notes_Data::Clear (); clear ();
	}
private:
	int link;
};

typedef vector <Shape_Data>    Shape_Array;
typedef Shape_Array::iterator  Shape_Itr;

#endif
