//*********************************************************
//	Split_Data.hpp - VISSIM parking entrance links
//*********************************************************

#ifndef SPLIT_DATA_HPP
#define SPLIT_DATA_HPP

#include <vector>
using namespace std;

typedef struct {
	int offset;
	int link;
	int node;
	int park_ab;
	int park_ba;
	int link_ab;
	int link_ba;
} Split_Data;

//---- split_data array ----

typedef vector <Split_Data>         Split_Array;
typedef Split_Array::iterator       Split_Itr;

typedef vector <Split_Array>        Link_Split_Array;
typedef Link_Split_Array::iterator  Link_Split_Itr;

#endif

