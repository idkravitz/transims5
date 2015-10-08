//*********************************************************
//	File_Group.hpp - trip table control parameters
//*********************************************************

#ifndef FILE_GROUP_HPP
#define FILE_GROUP_HPP

#include "Matrix_File.hpp"
#include "Diurnal_File.hpp"
#include "Diurnal_Data.hpp"
#include "Factor_File.hpp"
#include "Factor_Data.hpp"
#include "Db_File.hpp"
#include "Time_Periods.hpp"
#include "Equiv_Data.hpp"
#include "User_Program.hpp"
#include "TypeDefs.hpp"

#include <vector>
#include <string>
using namespace std;

//---------------------------------------------------------
//	File_Group class definition
//---------------------------------------------------------

class File_Group
{
public:
	File_Group (void);
	virtual ~File_Group (void);

	int    Group (void)                    { return (group); }
	int    Type (void)                     { return (type); }
	int    Purpose (void)                  { return (purpose); }
	int    Mode (void)                     { return (mode); }
	int    Time_Point (void)               { return (point); }
	int    Constraint (void)               { return (constraint); }
	int    Org_Wt (void)                   { return (org_wt); }
	int    Des_Wt (void)                   { return (des_wt); }
	bool   Dist_Wt (void)                  { return (dist_wt); }
	int    Stop_Wt (void)                  { return (stop_wt); }
	bool   Return_Flag (void)              { return (return_flag); }
	Dtime  Duration (void)                 { return (duration); }
	Dtime  Stop_Time (void)                { return (stop); }
	int    Priority (void)                 { return (priority); }
	int    Veh_Type (void)                 { return (veh_type); }
	int    Passengers (void)               { return (passengers); }

	int    Speed (void)                    { return (speed); }
	double Scaling_Factor (void)           { return (scaling); }

	void   Group (int value)               { group = value; }
	void   Type (int value)                { type = value; }
	void   Purpose (int value)             { purpose = value; }
	void   Mode (int value)                { mode = value; }
	void   Time_Point (int value)          { point = value; }
	void   Constraint (int value)          { constraint = value; }
	void   Org_Wt (int value)              { org_wt = value; }
	void   Des_Wt (int value)              { des_wt = value; }
	void   Dist_Wt (bool flag)             { dist_wt = flag; }
	void   Stop_Wt (int value)             { stop_wt = value; }
	void   Return_Flag (bool flag)         { return_flag = flag; }
	void   Duration (Dtime value)          { duration = value; }
	void   Stop_Time (Dtime value)         { stop = value; }
	void   Priority (int value)            { priority = value; }
	void   Veh_Type (int value)            { veh_type = value; }
	void   Passengers (int value)          { passengers = value; }
	void   Speed (int value)               { speed = value; }
	void   Scaling_Factor (double value)   { scaling = value; }

	bool   In_Range (Dtime t1, Dtime t2)   { return ((periods.Num_Periods () > 0) ? periods.Span_Range (t1, t2) : true); }
	bool   Set_Periods (String &str);

	void   Open_Trips (string &name, string &format);
	bool   Trip_Flag (void)                { return (trip_file != 0); }
	Matrix_File * Trip_File (void)         { return (trip_file); }

	//---- trip factor methods ----

	void   Open_Factors (string &name, string &format);
	void   Read_Factors (Zone_Equiv &zone_equiv);
	bool   Factor_Flag (void)              { return (factor_file != 0); }
	bool   Factor_Periods_Flag (void)      { return ((factor_file != 0) ? (factor_file->Num_Periods () > 0) : false); }
	int    Factor_Period_Num (Dtime time)  { return ((factor_file != 0) ? factor_file->Period (time) : 0); }
	Factor_Period * Period_Factor (int org, int des, int period = 0);

	//---- diurnal methods ----
	
	void   Open_Diurnal (string &name, string &format);
	void   Read_Diurnal (void);
	int    Time_Field (void)               { return (diurnal_file->Share_Field ()); }
	bool   Time_Field (string field);
	int    Num_Shares (void)               { return ((diurnal_header != 0) ? diurnal_header->Num_Fields () : -1); }

	int    Diurnal_Periods (void)          { return ((int) diurnal_array.size ()); }
	Dtime  Diurnal_Period_Time (int p)	   { return (diurnal_array [p].Time_of_Day ()); }
	void   Diurnal_Time_Range (int period, Dtime &low, Dtime &high);
	double Share (int period, int num)     { return (diurnal_array [period][num].Share ()); }
	Share_Data * Share_Ptr (int p, int n)  { return (&diurnal_array [p].at (n)); }

	bool   Diurnal_Script (void)           { return (diurnal_script != 0); }	
	void   Diurnal_Script (string name);
	int    Execute_Diurnal (void);
	bool   Compile_Diurnal (bool report);
	void   Diurnal_Stack (bool header)     { diurnal.Print_Commands (header); }

	//---- traveler type methods ----

	bool   Traveler_Script (void)          { return (traveler_script != 0); }
	void   Traveler_Script (string name);
	int    Execute_Traveler (void);
	bool   Compile_Traveler (Db_Base *traveler_file, bool report);
	void   Traveler_Stack (bool header)    { traveler.Print_Commands (header); }

	void   Random_Seed (int seed = 0)      { diurnal.Random_Seed (seed); traveler.Random_Seed (seed + 1); }

private:
	int    group;
	int    type;
	int    purpose;
	int    mode;
	int    point;
	int    constraint;
	int    org_wt;
	int    des_wt;
	int    stop_wt;
	bool   dist_wt;
	bool   return_flag;
	Dtime  duration;
	Dtime  stop;
	int    priority;
	int    speed;				//---- rounded ----
	int    veh_type;
	int    passengers;
	double scaling;
	Time_Periods periods;
	
	Matrix_File   *trip_file;

	//---- trip factor data ----

	Factor_File   *factor_file;
	Int_Map        factor_map;
	Factor_Table   factor_table;
	Factor_Period  default_factor;

	//---- traveler type data ----

	Db_File       *traveler_script;
	User_Program   traveler;

	//---- diurnal data ----

	Diurnal_Array  diurnal_array;
	Diurnal_File  *diurnal_file;
	Db_File       *diurnal_script;
	Db_Base       *diurnal_header;
	User_Program   diurnal;
};

typedef vector <File_Group>         File_Group_Array;
typedef File_Group_Array::iterator  File_Group_Itr;

#endif

