//*********************************************************
//	Get_Vehicle_Data.cpp - copy the vehicle file
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Get_Vehicle_Data
//---------------------------------------------------------

bool ConvertTrips::Get_Vehicle_Data (Vehicle_File &file, Vehicle_Data &data, int partition)
{
	int hhold;

	hhold = file.Household ();
	if (hhold > max_hh_in) max_hh_in = hhold;

	if (select_households && !hhold_range.In_Range (hhold)) return (false);

	vehicle_file->Copy_Fields (file);

	if (!vehicle_file->Write ()) {
		Error ("Writing Vehicle File");
	}
	veh_copy++;

	data.Household (hhold);
	hhold = partition;

	//---- don't save the record ----

	return (false);
}
