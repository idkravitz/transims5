//*********************************************************
//	Stop_Names.cpp - read the stop names
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Stop_Names
//---------------------------------------------------------

void LineSum::Stop_Names (void)
{
	int stop, stop_field, name_field;
	String name;

	Str_Map_Stat map_stat;

	stop_field = stop_name_file.Required_Field ("STOP", "NODE", "N", "S", "ID");
	name_field = stop_name_file.Required_Field ("NAME", "STATION", "LABEL");

	//---- read the stop names ----

	while (stop_name_file.Read ()) {
		stop = stop_name_file.Get_Integer (stop_field);
		name = stop_name_file.Get_String (name_field);

		map_stat = stop_names.insert (Str_Map_Data (stop, name));

		if (!map_stat.second) {
			Warning (String ("Duplicate Stop Name (%d = %s vs %s)") % stop % map_stat.first->second % name);
		}
	}
	Write (2, "Number of Stop Names = ") << stop_names.size ();
}
