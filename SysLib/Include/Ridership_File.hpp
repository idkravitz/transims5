//******************************************************** 
//	Ridership_File.hpp - Transit Ridership Input/Output
//********************************************************

#ifndef RIDERSHIP_FILE_HPP
#define RIDERSHIP_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Ridership_File Class definition
//---------------------------------------------------------

class SYSLIB_API Ridership_File : public Db_Header
{
public:
	Ridership_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Ridership_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Mode (void)                { return (Get_Integer (mode)); }
	int    Route (void)               { return (Get_Integer (route)); }
	int    Run (void)                 { return (Get_Integer (run)); }
	int    Stop (void)                { return (Get_Integer (stop)); }
	Dtime  Schedule (void)            { return (Get_Time (schedule)); }
	Dtime  Time (void)                { return (Get_Time (time)); }
	int    Board (void)               { return (Get_Integer (board)); }
	int    Alight (void)              { return (Get_Integer (alight)); }
	int    Load (void)                { return (Get_Integer (load)); }
	double Factor (void)              { return (Get_Double (factor)); }

	void   Mode (int value)           { Put_Field (mode, value); }
	void   Route (int value)          { Put_Field (route, value); }
	void   Run (int value)            { Put_Field (run, value); }
	void   Stop (int value)           { Put_Field (stop, value); }
	void   Schedule (Dtime value)     { Put_Field (schedule, value); }
	void   Time (Dtime value)         { Put_Field (time, value); }
	void   Board (int value)          { Put_Field (board, value); }
	void   Alight (int value)         { Put_Field (alight, value); }
	void   Load (int value)           { Put_Field (load, value); }
	void   Factor (double value)      { Put_Field (factor, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int mode, route, run, stop, schedule, time, board, alight, load, factor;
};

#endif
