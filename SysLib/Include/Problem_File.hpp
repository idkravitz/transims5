//******************************************************** 
//	Problem_File.hpp - Problem File Input/Output
//********************************************************

#ifndef PROBLEM_FILE_HPP
#define PROBLEM_FILE_HPP

#include "Trip_File.hpp"

//---------------------------------------------------------
//	Problem_File Class definition
//---------------------------------------------------------

class SYSLIB_API Problem_File : public Trip_File
{
public:

	Problem_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Problem_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	
	int    Problem (void)             { return (Get_Integer (problem)); }
	Dtime  Time (void)                { return (Get_Time (time)); }
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Lane (void)                { return (Get_Integer (lane)); }
	double Offset (void)              { return (Get_Double (offset)); }
	int    Route (void)               { return (Get_Integer (route)); }
	int    Survey (void)              { return (Get_Integer (survey)); }

	void   Problem (int value)        { Put_Field (problem, value); }
	void   Time (Dtime value)         { Put_Field (time, value); }
	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Lane (int value)           { Put_Field (lane, value); }
	void   Offset (double value)      { Put_Field (offset, value); }
	void   Route (int value)          { Put_Field (route, value); }
	void   Survey (int value)         { Put_Field (survey, value); }

	void   PopSyn_Data (void)         { Program_Type (POPSYN_CODE); }
	void   ActGen_Data (void)         { Program_Type (ACTGEN_CODE); }
	void   Router_Data (void)         { Program_Type (ROUTER_CODE); }
	void   Simulator_Data (void)      { Program_Type (MSIM_CODE); }

	virtual bool Create_Fields (void);

	int  Link_Field (void)			{ return (link); }
	int  Time_Field (void)          { return (time); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	enum Program_Code { POPSYN_CODE = 1, ACTGEN_CODE, ROUTER_CODE, MSIM_CODE };

	void Setup (void);

	void Program_Type (int code = 0);

	int program_code;

	int problem, time, link, dir, lane, offset, route, survey;
};

#endif
