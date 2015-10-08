//*********************************************************
//	Transit_Sort.hpp - sort impedance for transit path building
//*********************************************************

#ifndef TRANSIT_SORT_HPP
#define TRANSIT_SORT_HPP

#include "APIDefs.hpp"

#include <deque>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Transit_Path_Index class definition
//---------------------------------------------------------

class SYSLIB_API Transit_Path_Index
{
public:
	Transit_Path_Index (void)       { Clear (); }

	int      Index (void)           { return (index); }
	int      Type (void)            { return (type); }
	int      Path (void)            { return (path); }
	
	void     Index (int value)      { index = value; }
	void     Type (int value)       { type = (char) value; }
	void     Path (int value)       { path = (char) value; }

	void     Clear (void) 
	{
		Index (-1); Type (0); Path (0);
	}

private:
	int  index;
	char type;
	char path;
};

//---- transit queue ----

typedef deque <Transit_Path_Index>               Transit_Queue;

typedef multimap <unsigned, Transit_Path_Index>  Transit_Sort;
typedef pair <unsigned, Transit_Path_Index>      Transit_Sort_Data;
typedef Transit_Sort::iterator                   Transit_Sort_Itr;

SYSLIB_API bool operator == (Transit_Path_Index left, Transit_Path_Index right);

#endif
