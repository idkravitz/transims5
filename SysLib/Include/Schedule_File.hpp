//******************************************************** 
//	Schedule_File.hpp - Transit Schedule File Input/Output
//********************************************************

#ifndef SCHEDULE_FILE_HPP
#define SCHEDULE_FILE_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Schedule_File Class definition
//---------------------------------------------------------

class SYSLIB_API Schedule_File : public Db_Header
{
public:
	Schedule_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Schedule_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int   Route (void)                { return (Get_Integer (route)); }
	int   Stops (void)                { return (Get_Integer (stops)); }
	int   Run (int num)               { return ((Check (num)) ? Get_Integer (run [num]) : 0); }
	int   Stop (void)                 { return (Get_Integer (stop)); }
	Dtime Time (int num)              { return ((Check (num)) ? Get_Time (time [num]) : Dtime (0)); }

	void  Route (int value)           { Put_Field (route, value); }
	void  Stops (int value)           { Put_Field (stops, value); }
	void  Run (int num, int value)    { if (Check (num)) Put_Field (run [num], value); }
	void  Stop (int value)            { Put_Field (stop, value); }
	void  Time (int num, Dtime value) { if (Check (num)) Put_Field (time [num], value); }

	virtual bool Create_Fields (void);

	void  Clear_Columns (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);
	bool Check (int num)           { return (num >= 0 && num < NUM_SCHEDULE_COLUMNS); }

	int route, stops, run [NUM_SCHEDULE_COLUMNS];
	int stop, time [NUM_SCHEDULE_COLUMNS];
};

#endif
