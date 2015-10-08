//******************************************************** 
//	Pocket_File.hpp - Pocket File Input/Output
//********************************************************

#ifndef POCKET_FILE_HPP
#define POCKET_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Pocket_File Class definition
//---------------------------------------------------------

class SYSLIB_API Pocket_File : public Db_Header
{
public:
	Pocket_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Pocket_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Type (void)                { return (Get_Integer (type)); }
	int    Lanes (void)               { return (Get_Integer (lanes)); }
	double Length (void)              { return (Get_Double (length)); }
	double Offset (void)              { return (Get_Double (offset)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Lanes (int value)          { Put_Field (lanes, value); }
	void   Length (double value)      { Put_Field (length, value); }
	void   Offset (double value)      { Put_Field (offset, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, type, lanes, length, offset;
};

#endif
