//*********************************************************
//	Write_Zone.cpp - Write the New Zone File
//*********************************************************

#include "ZoneData.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Write_Zone
//---------------------------------------------------------

void ZoneData::Write_Zone ()
{
	int field, num_in, num_out;
	int zone, join;
	double dx, dy;

	Db_Sort_Array *data;
	Data_Itr data_itr;
	Points_Map *polygon;
	Polygon_Itr poly_itr;
	Points_Map_Itr boundary_itr;
	Sum_Itr sum_itr;

	num_in = num_out = 0;

	//---- reopen the zone file ----

	input_file->Open (0);

	Show_Message (String ("Writing %s -- Record") % output_file->File_Type ());
	Set_Progress ();

	while (input_file->Read ()) {
		Show_Progress ();

		zone = input_file->Zone ();
		if (zone == 0) continue;

		num_in++;

		output_file->Copy_Fields (*input_file);
		output_file->Zone (zone);

		//---- set the data field id ----

		if (script_flag) {
			for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
				data = data_itr->data_db;
				field = data_itr->zone_field;

				join = input_file->Get_Integer (field);

				if (!data->Read_Record (join)) {
					data->Reset_Record ();
				}
			}

			//---- set the polygon record index ----

			if (polygon_flag) {
				dx = input_file->X ();
				dy = input_file->Y ();
		
				for (poly_itr = polygons.begin (); poly_itr != polygons.end (); poly_itr++) {
					data = poly_itr->data_db;
					polygon = &poly_itr->polygon;

					join = 0;

					for (boundary_itr = polygon->begin (); boundary_itr != polygon->end (); boundary_itr++) {
						if (In_Polygon (boundary_itr->second, dx, dy)) {
							join = boundary_itr->first;
							break;
						}
					}
					if (join == 0 || !data->Read_Record (join)) {
						data->Reset_Record ();
					}
				}
			}

			//---- set the sum zone id ----

			if (sum_flag) {
				for (sum_itr = sum_group.begin (); sum_itr != sum_group.end (); sum_itr++) {
					if (!sum_itr->data_db->Read_Record (zone)) {
						sum_itr->data_db->Reset_Record ();
					}
				}
			}

			//---- execute the conversion script ----

			if (program.Execute (num_in) == 0) continue;
		}

		//---- save the output fields ----

		if (!output_file->Write ()) {
			Error (String ("Writing %s Record %d") % output_file->File_Type () % zone);
		}
		num_out++;
	}
	End_Progress ();

	input_file->Close ();
	output_file->Close ();

	Write (2, "Number of Zone Records Read = ") << num_in;
	Write (1, "Number of Zone Records Written = ") << num_out;
}

