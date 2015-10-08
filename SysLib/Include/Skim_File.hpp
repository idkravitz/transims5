//********************************************************* 
//	Skim_File.hpp - Skim File Input/Output
//*********************************************************

#ifndef SKIM_FILE_HPP
#define SKIM_FILE_HPP

#include "Matrix_File.hpp"
#include "Skim_Data.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Skim_File Class definition
//---------------------------------------------------------

class SYSLIB_API Skim_File : public Matrix_File
{
public:
	Skim_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Matrix_Type type = SKIM_TABLE, Units_Type od = ZONE_OD);
	Skim_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Matrix_Type type = SKIM_TABLE, Units_Type od = ZONE_OD);
	~Skim_File (void)                    { Clear_Table (); }

	Dtime  Time (void)                   { return (Get_Time (time)); }
	Dtime  Walk (void)                   { return (Get_Time (walk)); }
	Dtime  Drive (void)                  { return (Get_Time (drive)); }
	Dtime  Transit (void)                { return (Get_Time (transit)); }
	Dtime  Wait (void)                   { return (Get_Time (wait)); }
	Dtime  Other (void)                  { return (Get_Time (other)); }
	double Length (void)                 { return (Get_Double (length)); }
	double Cost (void)                   { return (Get_Double (cost)); }
	int    Impedance (void)              { return (Get_Integer (impedance)); }

	void   Time (Dtime value)            { Put_Field (time, value); }
	void   Walk (Dtime value)            { Put_Field (walk, value); }
	void   Drive (Dtime value)           { Put_Field (drive, value); }
	void   Transit (Dtime value)         { Put_Field (transit, value); }
	void   Wait (Dtime value)            { Put_Field (wait, value); }
	void   Other (Dtime value)           { Put_Field (other, value); }
	void   Length (double value)         { Put_Field (length, value); }
	void   Cost (double value)           { Put_Field (cost, value); }
	void   Impedance (int value)         { Put_Field (impedance, value); }

	virtual bool Create_Fields (void);
	
	bool   Time_Table_Flag (void)        { return (table_flag); }
	void   Time_Table_Flag (bool flag)   { table_flag = flag; }

	bool   Total_Time_Flag (void)        { return (time_flag); }
	void   Total_Time_Flag (bool flag)   { time_flag = flag; }

	Units_Type Time_Format (void)        { return (time_format); }
	void Time_Format (Units_Type type)   { time_format = type; }

	Units_Type Length_Format (void)      { return (len_format); }
	void Length_Format (Units_Type type) { len_format = type; }

	void Length_Flag (bool flag)         { len_flag = flag; }

	Mode_Type Skim_Mode (void)           { return (mode); }
	void Skim_Mode (Mode_Type type)      { mode = type; }

	int    Neighbor_Factor (void)        { return (near_factor); }
	void   Neighbor_Factor (int value)   { near_factor = value; }

	int    Table_Size (void)             { return (Num_Org () * Num_Des () * Num_Periods ()); }
	bool   Set_Table (void)              { return (Set_Table (Num_Org (), Num_Des (), Num_Periods ())); }
	bool   Set_Table (int num_org, int num_des, int periods = 1); 
	void   Clear_Table (void);

	Skim_Data * Table (int org, int des, int period = 0)  { return (table [org] [des] + period); }

	Dtime * Time_Skim (int org, int des, int period = 0)  { return (ttime [org] [des] + period); }

	Dtime Skim_Time (int org, int des, int period = 0)    { return (ttime [org] [des] [period]); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	virtual void Setup (void);

	int  time, walk, drive, transit, wait, other, length, cost, impedance, near_factor;
	Units_Type time_format, len_format;
	Mode_Type mode;
	bool time_flag, table_flag, len_flag;

	Skim_Data ***table;
	Dtime ***ttime;
};

#endif
