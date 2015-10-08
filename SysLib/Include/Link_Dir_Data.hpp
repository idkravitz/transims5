//*********************************************************
//	Link_Dir_Data.hpp - link direction data
//*********************************************************

#ifndef LINK_DIR_DATA_HPP
#define LINK_DIR_DATA_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Link_Dir_Data class definition
//---------------------------------------------------------

class SYSLIB_API Link_Dir_Data
{
public:
	Link_Dir_Data (int lnk_dir = 0)    { link_dir = lnk_dir; }

	int  Link_Dir (void)               { return (link_dir); }
	int  Link (void)                   { return (x.link); }
	int  Dir (void)                    { return (x.dir); }

	void Link_Dir (int value)          { link_dir = value; }
	void Link (int value)              { x.link = (unsigned int) value; }
	void Dir (int value)               { x.dir = (unsigned int) value; }

private:
#pragma pack (push, 4)
	union {
		int link_dir;
		struct {
			unsigned int dir  : 1;
			unsigned int link : 31;
		} x;
	};
#pragma pack (pop)
};

typedef vector <Link_Dir_Data>    Link_Dir_Array;
typedef Link_Dir_Array::iterator  Link_Dir_Itr;

#endif
