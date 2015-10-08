//*********************************************************
//	Sim_Park_Data.hpp - simulator parking data
//*********************************************************

#ifndef SIM_PARK_DATA_HPP
#define SIM_PARK_DATA_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Park_Data class definition
//---------------------------------------------------------
 
class Sim_Park_Data
{
public:
	Sim_Park_Data (void)            { Clear (); }

	int   Cell_AB (void)            { return (cell_ab); }
	int   Cell_BA (void)            { return (cell_ba); }
	int   Part_AB (void)            { return (part_ab); }
	int   Part_BA (void)            { return (part_ba); }
	int   Dir (void)                { return (dir); }
	int   Type (void)               { return (type); }
	int   Min_Lane_AB (void)        { return (min_ab); }
	int   Max_Lane_AB (void)        { return (max_ab); }
	int   Min_Lane_BA (void)        { return (min_ba); }
	int   Max_Lane_BA (void)        { return (max_ba); }

	void  Cell_AB (int value)       { cell_ab = (short) value; }
	void  Cell_BA (int value)       { cell_ba = (short) value; }
	void  Part_AB (int value)       { part_ab = (short) value; }
	void  Part_BA (int value)       { part_ba = (short) value; }
	void  Dir (int value)           { dir = (short) value; }
	void  Type (int value)          { type = (short) value; }
	void  Min_Lane_AB (int value)   { min_ab = (char) value; }
	void  Max_Lane_AB (int value)   { max_ab = (char) value; }
	void  Min_Lane_BA (int value)   { min_ba = (char) value; }
	void  Max_Lane_BA (int value)   { max_ba = (char) value; }

	void  Clear (void)              { cell_ab = cell_ba = part_ab = part_ba = dir = type = 0; min_ab = max_ab = min_ba = max_ba = 0; }

private:
	short cell_ab;
	short cell_ba;
	short part_ab;
	short part_ba;
	short dir;
	short type;
	char  min_ab;
	char  max_ab;
	char  min_ba;
	char  max_ba;
};

typedef vector <Sim_Park_Data>      Sim_Park_Array;
typedef Sim_Park_Array::iterator    Sim_Park_Itr;
typedef Sim_Park_Data *             Sim_Park_Ptr;

#endif
