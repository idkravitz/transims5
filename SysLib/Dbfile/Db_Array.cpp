//********************************************************* 
//	Db_Array.cpp - database arrays
//*********************************************************

#include "Db_Array.hpp"

//-----------------------------------------------------------
//	Read_Record
//-----------------------------------------------------------

bool Db_Data_Array::Read_Record (int number)
{
	if (number > 0) {
		index = number - 1;
	} else {
		index++;
	}
	if (index < 0 || index >= (int) size ()) {
		return (false);
	} else {
		return (Record (at (index))); 
	}
}

//-----------------------------------------------------------
//	Write_Record
//-----------------------------------------------------------

bool Db_Data_Array::Write_Record (int number)
{
	if (!Record ().OK ()) return (false);

	if (number > 0) {
		index = number - 1;
	} else {
		index++;
	}
	if (index < 0 || index > (int) size ()) return (false);

	if (index == (int) size ()) {
		push_back (Record ());
	} else {
		Buffer &buffer = at (index);
		buffer = Record ();
	}
	return (true); 
}

//-----------------------------------------------------------
//	Add_Record
//-----------------------------------------------------------

bool Db_Data_Array::Add_Record (void)
{
	if (!Record ().OK ()) return (false);
	push_back (Record ());
	return (true);
}

//-----------------------------------------------------------
//	Read_Record
//-----------------------------------------------------------

bool Db_Sort_Array::Read_Record (int index)
{
	Buf_Map_Itr itr = find (index);

	if (itr == end ()) {
		return (false);
	} else {
		return (Record (itr->second));
	}
}

//-----------------------------------------------------------
//	Write_Record
//-----------------------------------------------------------

bool Db_Sort_Array::Write_Record (int index)
{
	if (!Record ().OK ()) return (false);

	Buf_Map_Itr itr = find (index);

	if (itr == end ()) {
		Buf_Map_Stat map_stat = insert (Buf_Map_Data (index, Record ()));
		return (map_stat.second);
	} else {
		itr->second = Record ();
		return (true);
	}
}

