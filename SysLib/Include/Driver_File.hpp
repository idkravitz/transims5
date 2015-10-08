//******************************************************** 
//	Driver_File.hpp - Transit Driver File Input/Output
//********************************************************

#ifndef DRIVER_FILE_HPP
#define DRIVER_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Driver_File Class definition
//---------------------------------------------------------

class SYSLIB_API Driver_File : public Db_Header
{
public:
	Driver_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Driver_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int Route (void)                { return (Get_Integer (route)); }
	int Links (void)                { return (Get_Integer (links)); }
	int Type (void)                 { return (Get_Integer (type)); }
	int SubType (void)              { return (Get_Integer (subtype)); }
	int Link (void)                 { return (Get_Integer (link)); }
	int Dir (void)                  { return (Get_Integer (dir)); }

	void Route (int value)          { Put_Field (route, value); }
	void Links (int value)          { Put_Field (links, value); }
	void Type (int value)           { Put_Field (type, value); }
	void SubType (int value)        { Put_Field (subtype, value); }
	void Link (int value)           { Put_Field (link, value); }
	void Dir (int value)            { Put_Field (dir, value); }

	bool SubType_Flag (void)        { return (subtype >= 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int route, links, type, subtype, link, dir;
};

#endif
