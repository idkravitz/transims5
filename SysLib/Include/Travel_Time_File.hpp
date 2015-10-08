//******************************************************** 
//	Travel_Time_File.hpp - Travel Time Input/Output
//********************************************************

#ifndef TRAVEL_TIME_FILE_HPP
#define TRAVEL_TIME_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Travel_Time_File Class definition
//---------------------------------------------------------

class SYSLIB_API Travel_Time_File : public Db_Header
{
public:

	Travel_Time_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Travel_Time_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Household (void)           { return (Get_Integer (hhold)); }
	int    Person (void)              { return (Get_Integer (person)); }
	int    Tour (void)                { return (Get_Integer (tour)); }
	int    Trip (void)                { return (Get_Integer (trip)); }
	int    Mode (void)                { return (Get_Integer (mode)); }
	int    Purpose (void)             { return (Get_Integer (purpose)); }
	int    Constraint (void)          { return (Get_Integer (constraint)); }
	int    Start_Link (void)          { return (Get_Integer (start_link)); }
	int    End_Link (void)            { return (Get_Integer (end_link)); }
	Dtime  Trip_Start (void)          { return (Get_Time (trip_start)); }
	Dtime  Base_Start (void)          { return (Get_Time (base_start)); }
	Dtime  Started (void)             { return (Get_Time (started)); }
	Dtime  Start_Diff (void)          { return (Get_Time (start_diff)); }
	Dtime  Trip_End (void)            { return (Get_Time (trip_end)); }
	Dtime  Base_End (void)            { return (Get_Time (base_end)); }
	Dtime  Ended (void)               { return (Get_Time (ended)); }
	Dtime  End_Diff (void)            { return (Get_Time (end_diff)); }
	Dtime  Mid_Trip (void)            { return (Get_Time (mid_trip)); }
	Dtime  Travel_Time (void)         { return (Get_Time (ttime)); }

	void   Household (int value)      { Put_Field (hhold, value); }
	void   Person (int value)         { Put_Field (person, value); }
	void   Tour (int value)           { Put_Field (tour, value); }
	void   Trip (int value)           { Put_Field (trip, value); }
	void   Mode (int value)           { Put_Field (mode, value); }
	void   Purpose (int value)        { Put_Field (purpose, value); }
	void   Constraint (int value)     { Put_Field (constraint, value); }
	void   Start_Link (int value)     { Put_Field (start_link, value); }
	void   End_Link (int value)       { Put_Field (end_link, value); }
	void   Trip_Start (Dtime value)   { Put_Field (trip_start, value); }
	void   Base_Start (Dtime value)   { Put_Field (base_start, value); }
	void   Started (Dtime value)      { Put_Field (started, value); }
	void   Start_Diff (Dtime value)   { Put_Field (start_diff, value); }
	void   Trip_End (Dtime value)     { Put_Field (trip_end, value); }
	void   Base_End (Dtime value)     { Put_Field (base_end, value); }
	void   Ended (Dtime value)        { Put_Field (ended, value); }
	void   End_Diff (Dtime value)     { Put_Field (end_diff, value); }
	void   Mid_Trip (Dtime value)     { Put_Field (mid_trip, value); }
	void   Travel_Time (Dtime value)  { Put_Field (ttime, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int hhold, person, tour, trip, mode, base_start, started, start_diff;
	int base_end, ended, end_diff, mid_trip, ttime;
	int purpose, constraint, start_link, end_link, trip_start, trip_end;
};

#endif
