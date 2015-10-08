//******************************************************** 
//	Time_Sum_File.hpp - Time Summary Input/Output
//********************************************************

#ifndef TIME_SUM_FILE_HPP
#define TIME_SUM_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Time_Sum_File Class definition
//---------------------------------------------------------

class SYSLIB_API Time_Sum_File : public Db_Header
{
public:

	Time_Sum_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Time_Sum_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	Dtime From_Time (void)              { return (Get_Time (from_time)); }
	Dtime To_Time (void)                { return (Get_Time (to_time)); }
	int   Trip_Start (void)             { return (Get_Integer (trip_start)); }
	int   Started (void)                { return (Get_Integer (started)); }
	Dtime Start_Diff (void)             { return (Get_Time (start_diff)); }
	Dtime Start_Error (void)            { return (Get_Time (start_abs)); }
	int   Trip_End (void)               { return (Get_Integer (trip_end)); }
	int   Ended (void)                  { return (Get_Integer (ended)); }
	Dtime End_Diff (void)               { return (Get_Time (end_diff)); }
	Dtime End_Error (void)              { return (Get_Time (end_abs)); }
	int   Mid_Trip (void)               { return (Get_Integer (mid_trip)); }
	Dtime Travel_Time (void)            { return (Get_Time (ttime)); }
	Dtime TTime_Diff (void)             { return (Get_Time (ttime_diff)); }
	Dtime TTime_Error (void)            { return (Get_Time (ttime_abs)); }
	int   Sum_Trips (void)              { return (Get_Integer (sum_trip)); }
	Dtime Sum_Error (void)              { return (Get_Time (sum_error)); }
	Dtime Avg_Error (void)              { return (Get_Time (avg_error)); }
	
	void  From_Time (Dtime value)       { Put_Field (from_time, value); }
	void  To_Time (Dtime value)         { Put_Field (to_time, value); }
	void  Trip_Start (int value)        { Put_Field (trip_start, value); }
	void  Started (int value)           { Put_Field (started, value); }
	void  Start_Diff (Dtime value)      { Put_Field (start_diff, value); }
	void  Start_Error (Dtime value)     { Put_Field (start_abs, value); }
	void  Trip_End (int value)          { Put_Field (trip_end, value); }
	void  Ended (int value)             { Put_Field (ended, value); }
	void  End_Diff (Dtime value)        { Put_Field (end_diff, value); }
	void  End_Error (Dtime value)       { Put_Field (end_abs, value); }
	void  Mid_Trip (int value)          { Put_Field (mid_trip, value); }
	void  Travel_Time (Dtime value)     { Put_Field (ttime, value); }
	void  TTime_Diff (Dtime value)      { Put_Field (ttime_diff, value); }
	void  TTime_Error (Dtime value)     { Put_Field (ttime_abs, value); }
	void  Sum_Trips (int value)         { Put_Field (sum_trip, value); }
	void  Sum_Error (Dtime value)       { Put_Field (sum_error, value); }
	void  Avg_Error (Dtime value)       { Put_Field (avg_error, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int from_time, to_time, trip_start, started, start_diff, start_abs, trip_end, ended, end_diff, end_abs;
	int mid_trip, ttime, ttime_diff, ttime_abs, sum_trip, sum_error, avg_error;
};

//---------------------------------------------------------
//	Link_Time_Sum_File Class definition
//---------------------------------------------------------

class SYSLIB_API Link_Time_Sum_File : public Time_Sum_File
{
public:

	Link_Time_Sum_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Link_Time_Sum_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int   Link (void)             { return (Get_Integer (link)); }
	void  Link (int value)        { Put_Field (link, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link;
};

#endif
