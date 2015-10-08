//*********************************************************
//	Get_Access_Data.cpp - additional access link processing
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Get_Access_Data
//---------------------------------------------------------

bool ConvertTrips::Get_Access_Data (Access_File &file, Access_Data &data)
{
	if (Data_Service::Get_Access_Data (file, data)) {
		Convert_Location *loc_ptr;

		if (data.From_Type () == LOCATION_ID && data.To_Type () == PARKING_ID) {
			loc_ptr = &convert_array [data.From_ID ()];
			loc_ptr->Org_Parking (data.To_ID ());
		} else if (data.From_Type () == PARKING_ID && data.To_Type () == LOCATION_ID) {
			loc_ptr = &convert_array [data.To_ID ()];
			loc_ptr->Des_Parking (data.From_ID ());
		}
	}

	//---- don't save the record ----

	return (false);
}
