//*********************************************************
//	Veh_Type_Data.hpp - vehicle type data classes
//*********************************************************

#ifndef VEH_TYPE_DATA_HPP
#define VEH_TYPE_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Notes_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Veh_Type_Data class definition
//---------------------------------------------------------

class SYSLIB_API Veh_Type_Data : public Notes_Data
{
public:
	Veh_Type_Data (void)               { Clear (); }

	int      Type (void)               { return (type); }
	int      Length (void)             { return (length); }
	int      Max_Speed (void)          { return (max_speed); }
	int      Max_Accel (void)          { return (max_accel); }
	int      Max_Decel (void)          { return (max_decel); }
	int      Op_Cost (void)            { return (op_cost); }
	Use_Type Use (void)                { return (use); }
	int      Capacity (void)           { return (capacity); }
	int      Loading (void)            { return (loading); }
	int      Unloading (void)          { return (unloading); }
	int      Method (void)             { return (method); }
	Dtime    Min_Dwell (void)          { return (min_dwell); }
	Dtime    Max_Dwell (void)          { return (max_dwell); }
	bool     Grade_Flag (void)         { return (grade_flag); }
	double   Grade (int num)           { return ((num > 0 && num <= 100) ? grade [num-1] : 1.0); }
	double   PCE (void)                { return (pce); }

	void     Type (int value)          { type = value; }
	void     Length (int value)        { length = value; }
	void     Max_Speed (int value)     { max_speed = value; }
	void     Max_Accel (int value)     { max_accel = value; }
	void     Max_Decel (int value)     { max_decel = value; }
	void     Op_Cost (int value)       { op_cost = value; }
	void     Use (int value)           { use = (Use_Type) value; }
	void     Use (Use_Type value)      { use = value; }
	void     Capacity (int value)      { capacity = value; }
	void     Loading (int value)       { loading = value; }
	void     Unloading (int value)     { unloading = value; }
	void     Method (int value)        { method = value; }
	void     Min_Dwell (Dtime value)   { min_dwell = value; }
	void     Max_Dwell (Dtime value)   { max_dwell = value; }
	void     Grade_Flag (bool flag)    { grade_flag = flag; }
	void     Grade (int n, double fac) { grade [n] = (float) fac; }
	void     PCE (double value)        { pce = value; }
	
	void     Length (double value)     { length = exe->Round (value); }
	void     Max_Speed (double value)  { max_speed = exe->Round (value); }
	void     Max_Accel (double value)  { max_accel = exe->Round (value); }
	void     Max_Decel (double value)  { max_decel = exe->Round (value); }
	void     Op_Cost (double value)    { op_cost = exe->Round (value); }
	void     Loading (double value)    { loading = exe->Round (value * 10.0); }
	void     Unloading (double value)  { unloading = exe->Round (value * 10.0); } 

	void     Clear (void)
	{
		type = length = max_speed = max_accel = max_decel = capacity = loading = unloading = method = min_dwell = max_dwell = op_cost = 0;
		use = ANY; grade_flag = false; memset (grade, '\0', sizeof (grade)); pce = 1.0; Notes_Data::Clear ();
	}
private:
	int      type;
	int      length;
	int      max_speed;
	int      max_accel;
	int      max_decel;
	int      op_cost;
	Use_Type use;
	int      capacity;
	int      loading;
	int      unloading;
	int      method;
	Dtime    min_dwell; 
	Dtime    max_dwell; 
	bool     grade_flag;
	float    grade [100];
	double   pce;
};

typedef vector <Veh_Type_Data>    Veh_Type_Array;
typedef Veh_Type_Array::iterator  Veh_Type_Itr;

#endif
