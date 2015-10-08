//*********************************************************
//	Get_Zone_Data.cpp - Read the Zone File
//*********************************************************

#include "ZoneData.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool ZoneData::Get_Zone_Data (Zone_File &file, Zone_Data &data)
{
	if (Data_Service::Get_Zone_Data (file, data)) {
		int zone = data.Zone ();

		//---- count the records for the data file -----

		for (Data_Itr data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
			Db_Sort_Array *data = data_itr->data_db;

			int field = data_itr->zone_field;

			if (field >= 0) {
				zone = file.Get_Integer (field);
			}
			if (zone < 0) continue;

			if (data->Read_Record (zone)) {
				int count = data->Get_Integer (1);
				data->Put_Field (1, ++count);
				data->Write_Record (zone);
			}
		}
		return (true);
	}
	return (false);
}

