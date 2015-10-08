//********************************************************* 
//	Db_Array.hpp - database arrays interface
//*********************************************************

#ifndef DB_ARRAY_HPP
#define DB_ARRAY_HPP

#include "APIDefs.hpp"
#include "Db_Base.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Db_Data_Array Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Data_Array : public Db_Base, public Buffers
{
public:
	Db_Data_Array () : Db_Base (MODIFY, BINARY) { index = -1; }

	virtual bool Read_Record (int number = 0); 
	virtual bool Write_Record (int number = 0);
	virtual bool Add_Record (void);

	virtual bool Rewind (void)          { index = -1; return (true); }

	int  Num_Records (void)             { return ((int) size ()); }
	bool Num_Records (int num)          { reserve (num); return ((int) capacity () == num); }

private:
	int index;
};
typedef Buf_Itr  Db_Data_Itr;

//---------------------------------------------------------
//	Db_Sort_Array Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Sort_Array : public Db_Base, public Buf_Map
{
public:
	Db_Sort_Array () : Db_Base (MODIFY, BINARY) {}

	virtual bool Read_Record (int index); 
	virtual bool Write_Record (int index);

	int  Num_Records (void)             { return ((int) size ()); }
};
typedef Buf_Map_Itr  Db_Sort_Itr;

#endif
