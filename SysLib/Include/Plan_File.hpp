//********************************************************* 
//	Plan_File.hpp - Plan File Input/Output
//*********************************************************

#ifndef PLAN_FILE_HPP
#define PLAN_FILE_HPP

#include "Trip_File.hpp"
#include "Plan_Data.hpp"

//---------------------------------------------------------
//	Plan_File Class definition
//---------------------------------------------------------

class SYSLIB_API Plan_File : public Trip_File
{
public:
	Plan_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Plan_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	//---- read/write functions ----

	bool Read_Plan (Plan_Data &plan);
	bool Write_Plan (Plan_Data &plan);

	void Add_Leg (int num_legs = 1)   { Add_Records (num_legs); }

	Dtime  Depart (void)              { return (Get_Time (depart)); }
	Dtime  Arrive (void)              { return (Get_Time (arrive)); }
	Dtime  Activity (void)            { return (Get_Time (activity)); }
	Dtime  Walk (void)                { return (Get_Time (walk)); }
	Dtime  Drive (void)               { return (Get_Time (drive)); }
	Dtime  Transit (void)             { return (Get_Time (transit)); }
	Dtime  Wait (void)                { return (Get_Time (wait)); }
	Dtime  Other (void)               { return (Get_Time (other)); }
	double Length (void)              { return (Get_Double (length)); }
	double Cost (void)                { return (Get_Double (cost)); }
	int    Impedance (void)           { return (Get_Integer (impedance)); }

	int    Leg_Mode (void)            { return (Get_Integer (leg_mode)); }
	int    Leg_Type (void)            { return (Get_Integer (leg_type)); }
	int    Leg_ID (void)              { return (Get_Integer (leg_id)); }
	Dtime  Leg_Time (void)            { return (Get_Time (leg_time)); }
	double Leg_Length (void)          { return (Get_Double (leg_length)); }
	double Leg_Cost (void)            { return (Get_Double (leg_cost)); }
	int    Leg_Impedance (void)       { return (Get_Integer (leg_imp)); }

	void   Depart (Dtime value)       { Put_Field (depart, value); }
	void   Arrive (Dtime value)       { Put_Field (arrive, value); }
	void   Activity (Dtime value)     { Put_Field (activity, value); }
	void   Walk (Dtime value)         { Put_Field (walk, value); }
	void   Drive (Dtime value)        { Put_Field (drive, value); }
	void   Transit (Dtime value)      { Put_Field (transit, value); }
	void   Wait (Dtime value)         { Put_Field (wait, value); }
	void   Other (Dtime value)        { Put_Field (other, value); }
	void   Length (double value)      { Put_Field (length, value); }
	void   Cost (double value)        { Put_Field (cost, value); }
	void   Impedance (int value)      { Put_Field (impedance, value); }

	void   Leg_Mode (int value)       { Put_Field (leg_mode, value); }
	void   Leg_Type (int value)       { Put_Field (leg_type, value); }
	void   Leg_ID (int value)         { Put_Field (leg_id, value); }
	void   Leg_Time (Dtime value)     { Put_Field (leg_time, value); }
	void   Leg_Length (double value)  { Put_Field (leg_length, value); }
	void   Leg_Cost (double value)    { Put_Field (leg_cost, value); }
	void   Leg_Impedance (int value)  { Put_Field (leg_imp, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int depart, arrive, activity, walk, drive, transit, wait, other, length, cost, impedance;
	int leg_mode, leg_type, leg_id, leg_time, leg_length, leg_cost, leg_imp;
};

#endif
