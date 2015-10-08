//******************************************************** 
//	Detector_File.hpp - Detector File Input/Output
//********************************************************

#ifndef DETECTOR_FILE_HPP
#define DETECTOR_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Detector_File Class definition
//---------------------------------------------------------

class SYSLIB_API Detector_File : public Db_Header
{
public:
	Detector_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Detector_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Detector (void)            { return (Get_Integer (detector)); }
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	double Offset (void)              { return (Get_Double (offset)); }
	double Length (void)              { return (Get_Double (length)); }
	int    Lanes (void)               { return (Get_Integer (lanes)); }
	int    Type (void)                { return (Get_Integer (type)); }
	int    Use (void)                 { return (Get_Integer (use)); }

	void   Detector (int value)       { Put_Field (detector, value); }
	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Offset (double value)      { Put_Field (offset, value); }
	void   Length (double value)      { Put_Field (length, value); }
	void   Lanes (int value)          { Put_Field (lanes, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Use (int value)            { Put_Field (use, value); }

	//---- version 4.0 compatibility ----

	int  Low_Lane (void)            { return (Get_Integer (low)); }
	int  High_Lane (void)           { return (Get_Integer (high)); }

	void Low_Lane (int value)       { Put_Field (low, value); }
	void High_Lane (int value)      { Put_Field (high, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int detector, link, dir, offset, length, lanes, type, use, low, high;
};

#endif
