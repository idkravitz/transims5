//******************************************************** 
//	Activity_File.hpp - Version 4 activity file i/o
//********************************************************

#ifndef ACTIVITY_FILE_HPP
#define ACTIVITY_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Activity_File Class definition
//---------------------------------------------------------

class SYSLIB_API Activity_File : public Db_Header
{
public:

	Activity_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Activity_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int   Household (void)          { return (Get_Integer (hhold)); }
	int   Person (void)             { return (Get_Integer (person)); }
	int   Activity (void)           { return (Get_Integer (activity)); }
	int   Purpose (void)            { return (Get_Integer (purpose)); }
	int   Priority (void)           { return (Get_Integer (priority)); }
	Dtime Start_Min (void)          { return (Get_Time (st_min)); }
	Dtime Start_Max (void)          { return (Get_Time (st_max)); }
	Dtime End_Min (void)            { return (Get_Time (end_min)); }
	Dtime End_Max (void)            { return (Get_Time (end_max)); }
	Dtime Time_Min (void)           { return (Get_Time (time_min)); }
	Dtime Time_Max (void)           { return (Get_Time (time_max)); }
	int   Mode (void)               { return (Get_Integer (mode)); }
	int   Vehicle (void)            { return (Get_Integer (vehicle)); }
	int   Location (void)           { return (Get_Integer (location)); }
	int   Passengers (void)         { return (Get_Integer (num_pass)); }
	int   Constraint (void)         { return (Get_Integer (limit)); }

	void Household (int value)      { Put_Field (hhold, value); }
	void Person (int value)         { Put_Field (person, value); }
	void Activity (int value)       { Put_Field (activity, value); }
	void Purpose (int value)        { Put_Field (purpose, value); }
	void Priority (int value)       { Put_Field (priority, value); }
	void Start_Min (Dtime value)    { Put_Field (st_min, value); }
	void Start_Max (Dtime value)    { Put_Field (st_max, value); }
	void End_Min (Dtime value)      { Put_Field (end_min, value); }
	void End_Max (Dtime value)      { Put_Field (end_max, value); }
	void Time_Min (Dtime value)     { Put_Field (time_min, value); }
	void Time_Max (Dtime value)     { Put_Field (time_max, value); }
	void Mode (int value)           { Put_Field (mode, value); }
	void Vehicle (int value)        { Put_Field (vehicle, value); }
	void Location (int value)       { Put_Field (location, value); }
	void Passengers (int value)     { Put_Field (num_pass, value); }
	void Constraint (int value)     { Put_Field (limit, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int hhold, person, activity, purpose, priority;
	int st_min, st_max, end_min, end_max, time_min, time_max;
	int mode, vehicle, location, num_pass, limit;
};

#endif
