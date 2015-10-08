//*********************************************************
//	Sum_Zone.cpp - Sum the Zone Attributes by Distance
//*********************************************************

#include "ZoneData.hpp"

#include <math.h>

//---------------------------------------------------------
//	Sum_Zone
//---------------------------------------------------------

void ZoneData::Sum_Zone ()
{
	int i, num, field, zone, z;
	double x0, y0, dx, dy, distance;

	Db_Sort_Array *data;
	Sum_Itr sum_itr;
	Zone_Itr zone_itr;

	//---- initialize the database -----

	for (sum_itr = sum_group.begin (); sum_itr != sum_group.end (); sum_itr++) {
		data = sum_itr->data_db;

		for (zone_itr = zone_array.begin (); zone_itr != zone_array.end (); zone_itr++) {
			zone = zone_itr->Zone ();

			data->Reset_Record ();
			data->Put_Field (0, zone);

			if (!data->Write_Record (zone)) {
				Error ("Writing Sum Distance Database");
			}
		}
	}

	//---- reopen the activity location file ----

	input_file->Open (0);

	Show_Message (String ("Summing %s -- Record") % input_file->File_Type ());
	Set_Progress ();

	while (input_file->Read ()) {
		Show_Progress ();

		zone = input_file->Zone ();
		if (zone == 0) continue;

		x0 = Scale (input_file->X ());
		y0 = Scale (input_file->Y ());

		for (zone_itr = zone_array.begin (); zone_itr != zone_array.end (); zone_itr++) {
			z = zone_itr->Zone ();
			if (z == zone) continue;

			dx = zone_itr->X () - x0;
			dy = zone_itr->Y () - y0;

			distance = sqrt (dx * dx + dy * dy);

			for (sum_itr = sum_group.begin (); sum_itr != sum_group.end (); sum_itr++) {
				if (distance <= sum_itr->distance) {
					data = sum_itr->data_db;
					if (!data->Read_Record (z)) continue;

					num = data->Num_Fields ();
					data->Put_Field (1, data->Get_Integer (1) + 1);

					for (i=2; i < num; i++) {
						field = sum_itr->field_map [i];

						data->Put_Field (i, data->Get_Double (i) + input_file->Get_Double (field));
					}
					data->Write_Record (z);
				}
			}
		}
	}
	End_Progress ();
}

