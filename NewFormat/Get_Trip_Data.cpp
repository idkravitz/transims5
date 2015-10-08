//*********************************************************
//	Get_Trip_Data.cpp - Read the Trip File
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Get_Trip_Data
//---------------------------------------------------------

bool NewFormat::Get_Trip_Data (Trip_File &file, Trip_Data &trip_rec, int partition)
{
	static int part_num = -1;
	static Trip_File *out_file;

	if (part_num != partition) {
		part_num = partition;
		out_file = (Trip_File *) System_File_Handle (NEW_TRIP);

		if (out_file->Part_Flag () && part_num > 1) {
			if (!out_file->Open (part_num)) {
				Error (String ("Opening %s") % out_file->Filename ());
			}
			if (part_num > max_trip_part) max_trip_part = part_num;
		} else {
			max_trip_part = 1;
		}
	}
	if (Data_Service::Get_Trip_Data (file, trip_rec, partition)) {
		num_new_trip += Put_Trip_Data (*out_file, trip_rec);
	}
	return (false);
}
