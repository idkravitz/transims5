//******************************************************** 
//	Access_File.hpp - Process Link File Input/Output
//********************************************************

#ifndef ACCESS_FILE_HPP
#define ACCESS_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Access_File Class definition
//---------------------------------------------------------

class SYSLIB_API Access_File : public Db_Header
{
public:
	Access_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Access_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int   Link (void)                { return (Get_Integer (link)); }
	int   From_ID (void)             { return (Get_Integer (from_id)); }
	int   From_Type (void)           { return (Get_Integer (from_type)); }
	int   To_ID (void)               { return (Get_Integer (to_id)); }
	int   To_Type (void)             { return (Get_Integer (to_type)); }
	int   Dir (void)                 { return (Get_Integer (dir)); }
	Dtime Time (void)                { return (Get_Time (time)); }
	int   Cost (void)                { return (Get_Integer (cost)); }

	void  Link (int value)           { Put_Field (link, value); }
	void  From_ID (int value)        { Put_Field (from_id, value); }
	void  From_Type (int value)      { Put_Field (from_type, value); }
	void  To_ID (int value)          { Put_Field (to_id, value); }
	void  To_Type (int value)        { Put_Field (to_type, value); }
	void  Dir (int value)            { Put_Field (dir, value); }
	void  Time (Dtime value)         { Put_Field (time, value); }
	void  Cost (int value)           { Put_Field (cost, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, from_id, to_id, from_type, to_type, dir, time, cost;
};

#endif
