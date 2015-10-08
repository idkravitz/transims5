//*********************************************************
//	Get_Location_Data.cpp - additional location processing
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool ConvertTrips::Get_Location_Data (Location_File &file, Location_Data &data)
{
	if (Data_Service::Get_Location_Data (file, data)) {
		if (data.Zone () < 0) return (false);

		Int_Map_Itr field;
		Convert_Location loc_rec;

		loc_rec.Num_Weight ((int) field_map.size ());

		for (field = field_map.begin (); field != field_map.end (); field++) {
			loc_rec [field->second] = file.Get_Double (field->first);
		}
		convert_array.push_back (loc_rec);
		return (true);
	}
	return (false);
}
