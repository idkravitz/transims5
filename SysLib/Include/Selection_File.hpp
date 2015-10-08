//******************************************************** 
//	Selection_File.hpp - household selection file input/output
//********************************************************

#ifndef SELECTION_FILE_HPP
#define SELECTION_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Selection_File Class definition
//---------------------------------------------------------

class SYSLIB_API Selection_File : public Db_Header
{
public:
	Selection_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Selection_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int Household (void)            { return (Get_Integer (hhold)); }
	int Person (void)               { return (Get_Integer (person)); }
	int Tour (void)                 { return (Get_Integer (tour)); }
	int Trip (void)                 { return (Get_Integer (trip)); }
	int Type (void)                 { return (Get_Integer (type)); }
	int Partition (void)            { return (Get_Integer (partition)); }

	void Household (int value)      { Put_Field (hhold, value); }
	void Person (int value)         { Put_Field (person, value); }
	void Tour (int value)           { Put_Field (tour, value); }
	void Trip (int value)           { Put_Field (trip, value); }
	void Type (int value)           { Put_Field (type, value); }
	void Partition (int value)      { Put_Field (partition, value); }

	bool Type_Flag (void)           { return (type_flag); }
	bool Partition_Flag (void)      { return (part_flag); }

	void Type_Flag (bool flag)      { type_flag = flag; };
	void Partition_Flag (bool flag) { part_flag = flag; };

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int hhold, person, tour, trip, type, partition;
	bool type_flag, part_flag;
};

#endif
