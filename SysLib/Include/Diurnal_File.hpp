//******************************************************** 
//	Diurnal_File.hpp - Diurnal Distribution Input/Output
//********************************************************

#ifndef DIURNAL_FILE_HPP
#define DIURNAL_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Diurnal_File Class definition
//---------------------------------------------------------

class SYSLIB_API Diurnal_File : public Db_Header
{
public:
	Diurnal_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Diurnal_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	double Share (void)               { return (Get_Double (share)); }
	
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Share (double value)       { Put_Field (share, value); }
	
	int    Start_Field (void)         { return (start); }
	int    End_Field (void)           { return (end); }
	int    Share_Field (void)         { return (share); }
	void   Share_Field (int value)    { share = value; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int start, end, share;
};

#endif
