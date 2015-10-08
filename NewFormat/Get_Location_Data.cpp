//*********************************************************
//	Get_Location_Data.cpp - process the location file
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool NewFormat::Get_Location_Data (Location_File &file, Location_Data &location_rec)
{
	if (Data_Service::Get_Location_Data (file, location_rec)) {
		if (copy_flag && new_loc_file != 0) {
			new_loc_file->Copy_Fields (file);
			Put_Location_Data (*new_loc_file, location_rec);
		}
		return (true);
	}
	return (false);
}

