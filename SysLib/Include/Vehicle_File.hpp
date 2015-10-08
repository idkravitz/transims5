//******************************************************** 
//	Vehicle_File.hpp - Vehicle File Input/Output
//********************************************************

#ifndef VEHICLE_FILE_HPP
#define VEHICLE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Vehicle_File Class definition
//---------------------------------------------------------

class SYSLIB_API Vehicle_File : public Db_Header
{
public:
	Vehicle_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Vehicle_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int  Household (void)           { return (Get_Integer (hhold)); }
	int  Vehicle (void)             { return (Get_Integer (vehicle)); }
	int  Parking (void)             { return (Get_Integer (parking)); }
	int  Type (void)                { return (Get_Integer (type)); }
	int  SubType (void)             { return (Get_Integer (subtype)); }
	int  Partition (void)           { return (Get_Integer (partition)); }
	
	void Household (int value)      { Put_Field (hhold, value); }
	void Vehicle (int value)        { Put_Field (vehicle, (value)); }
	void Parking (int value)        { Put_Field (parking, value); }
	void Type (int value)           { Put_Field (type, value); }
	void SubType (int value)        { Put_Field (subtype, value); }
	void Partition (int value)      { Put_Field (partition, value); }

	bool SubType_Flag (void)        { return (subtype >= 0); }
	bool Partition_Flag (void)      { return (partition >= 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int hhold, vehicle, parking, type, subtype, partition;
};

#endif
