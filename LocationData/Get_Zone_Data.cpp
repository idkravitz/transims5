//*********************************************************
//	Get_Zone_Data.cpp - Read the Zone File
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool LocationData::Get_Zone_Data (Zone_File &file, Zone_Data &data)
{
	if (Data_Service::Get_Zone_Data (file, data)) {
		int zone;
		double dvalue;

		Sub_Group_Itr sub_itr;

		zone = data.Zone ();

		for (sub_itr = sub_group.begin (); sub_itr != sub_group.end (); sub_itr++) {
			if (sub_itr->zone_field >= 0) {
				dvalue = file.Get_Double (sub_itr->zone_field);
				sub_itr->zone_data.insert (Int_Dbl_Map_Data (zone, dvalue));
			}
		}
		return (true);
	}
	return (false);
}
