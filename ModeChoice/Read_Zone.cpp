//*********************************************************
//	Read_Zone.cpp - read the zone file
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Read_Zone
//---------------------------------------------------------

void ModeChoice::Read_Zone (void)
{
	int zone;

	Show_Message (String ("Reading %s -- Record") % zone_file.File_Type ());
	Set_Progress ();

	while (zone_file.Read ()) {
		Show_Progress ();

		zone = zone_file.Get_Integer (zone_field);

		org_db.Copy_Fields (zone_file);
		org_db.Write_Record (zone);

		des_db.Copy_Fields (zone_file);
		des_db.Write_Record (zone);
	}
	End_Progress ();
	zone_file.Close ();
}
