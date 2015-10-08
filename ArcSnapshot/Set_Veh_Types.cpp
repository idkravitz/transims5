//*********************************************************
//	Set_Veh_Types.cpp - set the vehicle type length
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Set_Veh_Types
//---------------------------------------------------------

void ArcSnapshot::Set_Veh_Types (void)
{
	int type, fld;
	double size;
	String buffer;
	Integers int_rec;

	Veh_Type_Itr veh_type_itr;

	//---- process vehicle type data ----

	max_type = 0;

	for (veh_type_itr = veh_type_array.begin (); veh_type_itr != veh_type_array.end (); veh_type_itr++) {
		type = veh_type_itr->Type ();
		if (type > max_type) max_type = type;
	}
	max_type++;

	//---- initialize the vehicle length array ----

	veh_length.assign (max_type, cell_size);

	//---- set vehicle length and cell size ----

	size = 1000;

	for (veh_type_itr = veh_type_array.begin (); veh_type_itr != veh_type_array.end (); veh_type_itr++) {
		type = veh_type_itr->Type ();
		veh_length [type] = UnRound (veh_type_itr->Length ());

		if (veh_length [type] < size) {
			size = veh_length [type];
		}
	}
	if (size < cell_size) cell_size = size;

	//---- allocate link arrays ----

	if (sum_flag) {
		num_links = (int) link_array.size ();

		vehicles.assign (max_type, int_rec);
		persons.assign (max_type, int_rec);
		veh_fld.assign (max_type, -1);
		per_fld.assign (max_type, -1);

		summary_file.Add_Field ("LINK", DB_INTEGER, 10);
		summary_file.Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		summary_file.Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		summary_file.Add_Field ("VEHICLES", DB_INTEGER, 10);
		summary_file.Add_Field ("PERSONS", DB_INTEGER, 10);

		link_fld = summary_file.Field_Number ("LINK");
		start_fld = summary_file.Field_Number ("START");
		end_fld = summary_file.Field_Number ("END");

		veh_fld [0] = summary_file.Field_Number ("VEHICLES");
		per_fld [0] = fld = summary_file.Field_Number ("PERSONS");

		for (type=0; type < max_type; type++) {
			if (type > 0 && select_vehicles && !vehicle_range.In_Range (type)) continue;

			if (type > 0) {
				buffer = String ("VEH%d") % type;
				summary_file.Add_Field (buffer, DB_INTEGER, 10);
				veh_fld [type] = fld++;

				buffer = String ("PER%d") % type;
				summary_file.Add_Field (buffer, DB_INTEGER, 10);
				per_fld [type] = fld++;
			}
			vehicles [type].assign (num_links, 0);
			persons [type].assign (num_links, 0);
		}
		summary_file.Write_Header ();
	}
}
