//*********************************************************
//	Access_Data.hpp - network process link data
//*********************************************************

#ifndef ACCESS_DATA_HPP
#define ACCESS_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Access class definition
//---------------------------------------------------------

class SYSLIB_API Access_Data : public Notes_Data
{
public: 
	Access_Data (void)             { Clear (); } 

	int   Link (void)              { return (link); }
	int   From_ID (void)           { return (from_id); }
	int   To_ID (void)             { return (to_id); }
	int   From_Type (void)         { return (from_type); }
	int   To_Type (void)           { return (to_type); }
	int   Dir (void)               { return (dir); }
	Dtime Time (void)              { return (time); }
	int   Cost (void)              { return (cost); }

	void  Link (int value)         { link = value; }
	void  From_ID (int value)      { from_id = value; }
	void  To_ID (int value)        { to_id = value; }
	void  From_Type (int value)    { from_type = (unsigned char) value; }
	void  To_Type (int value)      { to_type = (unsigned char) value; }
	void  Dir (int value)          { dir = (unsigned short) value; }
	void  Time (Dtime value)       { time = (unsigned short) value; }
	void  Cost (int value)         { cost = (unsigned short) value; } 

	int   ID (bool from_flag)      { return ((from_flag) ? from_id : to_id); }
	int   Type (bool from_flag)    { return ((from_flag) ? from_type : to_type); }

	void  Clear (void) 
	{
		link = from_id = to_id = 0; from_type = to_type = 0; 
		dir = time = cost = 0; Notes_Data::Clear (); 
	}
private:
	int            link;
	int            from_id;
	int            to_id;
	unsigned char  from_type;
	unsigned char  to_type;
	unsigned short dir;
	unsigned short time; 
	unsigned short cost;
};

typedef vector <Access_Data>    Access_Array;
typedef Access_Array::iterator  Access_Itr;

#endif
