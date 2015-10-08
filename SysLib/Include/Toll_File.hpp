//******************************************************** 
//	Toll_File.hpp - Toll File Input/Output
//********************************************************

#ifndef TOLL_FILE_HPP
#define TOLL_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Toll_File Class definition
//---------------------------------------------------------

class SYSLIB_API Toll_File : public Db_Header
{
public:
	Toll_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Toll_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	int    Use (void)                 { return (Get_Integer (use)); }
	int    Toll (void)                { return (Get_Integer (toll)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Use (int value)            { Put_Field (use, value); }
	void   Toll (int value)           { Put_Field (toll, value); }

	bool   Use_Flag (void)            { return (use >= 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, start, end, use, toll;
};

#endif
