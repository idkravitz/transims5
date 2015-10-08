//*********************************************************
//	List_Data.hpp - two-way list of record indices
//*********************************************************

#ifndef LIST_DATA_HPP
#define LIST_DATA_HPP

#include "APIDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	List_Data class definition
//---------------------------------------------------------

class SYSLIB_API List_Data
{
public:
	List_Data (void)                 { Clear (); }

	int  From_List (void)            { return (from_list); }
	int  To_List (void)              { return (to_list); }
	
	void From_List (int value)       { from_list = value; }
	void To_List (int value)         { to_list = value; }

	int  Next (bool from_flag)       { return ((from_flag) ? from_list : to_list); }

	void Clear (void)                { from_list = to_list = -1; }

private:
	int from_list;
	int to_list;
};

typedef vector <List_Data>    List_Array;
typedef List_Array::iterator  List_Itr;

#endif
