//******************************************************** 
//	Sign_File.hpp - Sign File Input/Output
//********************************************************

#ifndef SIGN_FILE_HPP
#define SIGN_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Sign_File Class definition
//---------------------------------------------------------

class SYSLIB_API Sign_File : public Db_Header
{
public:
	Sign_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Sign_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int  Link (void)                { return (Get_Integer (link)); }
	int  Dir (void)                 { return (Get_Integer (dir)); }
	int  Sign (void)                { return (Get_Integer (sign)); }

	void Link (int value)           { Put_Field (link, value); }
	void Dir (int value)            { Put_Field (dir, value); }
	void Sign (int value)           { Put_Field (sign, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, sign;
};

#endif
