//*********************************************************
//	Node_Data.hpp - network node data
//*********************************************************

#ifndef NODE_DATA_HPP
#define NODE_DATA_HPP

#include "APIDefs.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Node_Data class definition
//---------------------------------------------------------

class SYSLIB_API Node_Data : public Notes_Data
{
public:
	Node_Data (void)             { Clear (); }

	int  Node (void)             { return (node); }
	int  X (void)                { return (x); }
	int  Y (void)                { return (y); }
	int  Z (void)                { return (z); }
	int  Control (void)          { return (control); }
	int  Subarea (void)          { return (subarea); }
	int  Partition (void)        { return (part); }

	void Node (int value)        { node = value; }
	void X (int value)           { x = value; }
	void Y (int value)           { y = value; }
	void Z (int value)           { z = value; }
	void Control (int value)     { control = value; }
	void Subarea (int value)     { subarea = (short) value; }
	void Partition (int value)   { part = (short) value; }

	int  Count (void)            { return (part); }
	void Count (int value)       { part = (short) value; }
	void Add_Count (int add = 1) { part = (short) (part + add); }
	void Drop_Count (int n = 1)  { part = (short) (part - n); }

	//---- units rounding ----
	
	void X (double value)        { x = exe->Round (value); }
	void Y (double value)        { y = exe->Round (value); }
	void Z (double value)        { z = exe->Round (value); }

	void Clear (void)     
	{ 
		node = x = y = z = 0; control = -1; subarea = part = 0; Notes_Data::Clear (); 
	}
private:
	int   node;
	int   x;
	int   y;
	int   z;
	int   control;
	short subarea;
	short part;
};

typedef vector <Node_Data>    Node_Array;
typedef Node_Array::iterator  Node_Itr;

#endif
