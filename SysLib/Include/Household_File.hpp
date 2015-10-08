//******************************************************** 
//	Household_File.hpp - Household File Input/Output
//********************************************************

#ifndef HOUSEHOLD_FILE_HPP
#define HOUSEHOLD_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Household_File Class definition
//---------------------------------------------------------

class SYSLIB_API Household_File : public Db_Header
{
public:
	Household_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Household_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int Household (void)            { return (Get_Integer (hhold)); }
	int Location (void)             { return (Get_Integer (location)); }
	int Persons (void)              { return (Get_Integer (persons)); }
	int Workers (void)              { return (Get_Integer (workers)); }
	int Vehicles (void)             { return (Get_Integer (vehicles)); }
	int Type (void)                 { return (Get_Integer (type)); }
	int Partition (void)            { return (Get_Integer (partition)); }

	int Person (void)               { return (Get_Integer (person)); }
	int Age (void)                  { return (Get_Integer (age)); }
	int Relate (void)               { return (Get_Integer (relate)); }
	int Gender (void)               { return (Get_Integer (gender)); }
	int Work (void)                 { return (Get_Integer (work)); }
	int Drive (void)                { return (Get_Integer (drive)); }

	void Household (int value)      { Put_Field (hhold, value); }
	void Location (int value)       { Put_Field (location, value); }
	void Persons (int value)        { Put_Field (persons, value); }
	void Workers (int value)        { Put_Field (workers, value); }
	void Vehicles (int value)       { Put_Field (vehicles, value); }
	void Type (int value)           { Put_Field (type, value); }
	void Partition (int value)      { Put_Field (partition, value); }

	void Person (int value)         { Put_Field (person, value); }
	void Age (int value)            { Put_Field (age, value); }
	void Relate (int value)         { Put_Field (relate, value); }
	void Gender (int value)         { Put_Field (gender, value); }
	void Work (int value)           { Put_Field (work, value); }
	void Drive (int value)          { Put_Field (drive, value); }
	
	bool Partition_Flag (void)      { return (partition >= 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int hhold, location, persons, workers, vehicles, type, partition;
	int person, age, relate, gender, work, drive;
};

#endif

