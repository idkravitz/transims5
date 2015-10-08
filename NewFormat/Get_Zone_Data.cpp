//*********************************************************
//	Get_Zone_Data.cpp - process the zone file
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool NewFormat::Get_Zone_Data (Zone_File &file, Zone_Data &zone_rec)
{
	if (Data_Service::Get_Zone_Data (file, zone_rec)) {
		if (copy_flag && new_zone_file != 0) {
			new_zone_file->Copy_Fields (file);
			Put_Zone_Data (*new_zone_file, zone_rec);
		}
		return (true);
	}
	return (false);
}
