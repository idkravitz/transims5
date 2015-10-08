//******************************************************** 
//	Person_File.hpp - household person file input/output
//********************************************************

#ifndef PERSON_FILE_HPP
#define PERSON_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Person_File Class definition
//---------------------------------------------------------

class SYSLIB_API Person_File : public Db_Header
{
public:
	Person_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Person_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int Household (void)            { return (Get_Integer (hhold)); }
	int Person (void)               { return (Get_Integer (person)); }
	int Age (void)                  { return (Get_Integer (age)); }
	int Relate (void)               { return (Get_Integer (relate)); }
	int Gender (void)               { return (Get_Integer (gender)); }
	int Work (void)                 { return (Get_Integer (work)); }
	int Drive (void)                { return (Get_Integer (drive)); }

	void Household (int value)      { Put_Field (hhold, value); }
	void Person (int value)         { Put_Field (person, value); }
	void Age (int value)            { Put_Field (age, value); }
	void Relate (int value)         { Put_Field (relate, value); }
	void Gender (int value)         { Put_Field (gender, value); }
	void Work (int value)           { Put_Field (work, value); }
	void Drive (int value)          { Put_Field (drive, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int hhold, person, age, relate, gender, work, drive;
};

#endif
