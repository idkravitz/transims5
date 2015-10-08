//*********************************************************
//	Write_Location.cpp - write the activity location file
//*********************************************************

#include "LocationData.hpp"

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Write_Location
//---------------------------------------------------------

void LocationData::Write_Location (void)
{
	int i, field, loc, num_in, num_out;
	int x, y, zone, zone_field, join;
	double dx, dy, weight, weight1, weight2, distance;
	Dtime time;

	Db_Sort_Array *data;
	Location_Data *location_ptr, *loc_ptr;
	Link_Data *link_ptr;
	Loc_Walk_Itr loc_walk_itr;
	Subzone_Data *subzone_ptr;
	Sub_Group_Itr sub_itr;
	Data_Itr data_itr;
	Points_Map_Itr boundary_itr;
	Points_Map *polygon;
	Polygon_Itr poly_itr;
	Int2_Key key, key2;
	Int2_Map_Itr int2_itr;
	Int_Map_Itr int_itr;
	I2_Dbl_Map_Itr wt_itr;
	Int_Dbl_Map_Itr loc_itr;
	Int_Itr fld_itr, code_itr;

	num_in = num_out = 0;

	//---- reopen the activity location file ----

	input_file->Open (0);
	zone_field = input_file->Zone_Field ();

	Show_Message (String ("Writing %s -- Record") % output_file->File_Type ());
	Set_Progress ();

	while (input_file->Read ()) {
		Show_Progress ();

		loc = input_file->Location ();
		if (loc == 0) continue;
			
		int_itr = location_map.find (loc);
		location_ptr = &location_array [int_itr->second];

		num_in++;

		output_file->Copy_Fields (*input_file);

		//---- copy standard field types -----

		output_file->Location (loc);
		output_file->Link (input_file->Link ());
		output_file->Dir (input_file->Dir ());
		output_file->Offset (input_file->Offset ());
		output_file->Setback (input_file->Setback ());

		//---- get the zone number ----

		zone = location_ptr->Zone ();

		if (zone_file_flag && zone >= 0) {
			zone = zone_array [zone].Zone ();
		}
		output_file->Zone (zone);

		//---- set the link use flag field ----

		if (use_flag) {
			int_itr = link_map.find (output_file->Link ());
			if (int_itr != link_map.end ()) {
				link_ptr = &link_array [int_itr->second];

				code_itr = use_code.begin ();

				for (fld_itr = use_field.begin (); fld_itr != use_field.end (); fld_itr++, code_itr++) {
					if ((link_ptr->Use () & (*code_itr)) > 0) {
						output_file->Put_Field (*fld_itr, 1);
					} else {
						output_file->Put_Field (*fld_itr, 0);
					}
				}
			}
		}

		//---- calculate the walk access field ----

		if (walk_access_flag) {
			link_ptr = &link_array [location_ptr->Link ()];
			weight = 0;

			//---- check for walk permissions ----

			if (Use_Permission (link_ptr->Use (), WALK)) {
				x = location_ptr->X ();
				y = location_ptr->Y ();

				//---- find the best transit access points ----

				weight1 = weight2 = 0;

				for (i=0, loc_walk_itr = loc_walk_array.begin (); loc_walk_itr != loc_walk_array.end (); loc_walk_itr++, i++) {
					if (loc_walk_itr->weight == 0 || loc_walk_itr->count == 0) continue;

					loc_ptr = &location_array [i];

					dx = UnRound (loc_ptr->X () - x);
					dy = UnRound (loc_ptr->Y () - y);

					distance = sqrt (dx * dx + dy * dy);

					distance += loc_walk_itr->distance;

					if (distance < walk_distance) {
						weight = loc_walk_itr->weight * (walk_distance - distance);
						if (weight > weight1) {
							weight2 = weight1;
							weight1 = weight;
						} else if (weight > weight2) {
							weight2 = weight;
						}
					}
				}
				weight = (weight1 + weight2) / 2.0;
			}
			output_file->Put_Field (walk_access_field, weight);
		}

		//---- calculate the subzone fields ----

		for (sub_itr = sub_group.begin (); sub_itr != sub_group.end (); sub_itr++) {
			if (sub_itr->loc_field < 0) continue;

			output_file->Put_Field (sub_itr->loc_field, 0);

			//---- allocate subzone data ----

			if (subzone_map_flag) {
				loc_itr = sub_itr->loc_weight.find (loc);
				if (loc_itr != sub_itr->loc_weight.end ()) {
					weight = loc_itr->second;
				} else {
					weight = 0;
				}
			} else if (sub_itr->max_distance == 0) {
				weight = 0;

				key.first = zone;
				key.second = 0;

				for (int2_itr = sub_itr->data_map.lower_bound (key); int2_itr != sub_itr->data_map.end (); int2_itr++) {
					if (int2_itr->first.first != zone) break;

					subzone_ptr = &sub_itr->data [int2_itr->second];
					if (subzone_ptr->Data () == 0) continue;

					key2.first = int2_itr->first.second;
					key2.second = loc;

					wt_itr = subzone_weight.find (key2);

					if (wt_itr != subzone_weight.end () && subzone_ptr->Data () > 0) {
						weight += subzone_ptr->Data () * wt_itr->second;
					}
				}
				if (weight < 0) weight = 0;

			} else {

				//---- distance weighted attribute ----

				x = location_ptr->X ();
				y = location_ptr->Y ();

				weight1 = weight2 = 0;

				key.first = zone;
				key.second = 0;

				for (int2_itr = sub_itr->data_map.lower_bound (key); int2_itr != sub_itr->data_map.end (); int2_itr++) {
					if (int2_itr->first.first != zone) break;

					subzone_ptr = &sub_itr->data [int2_itr->second];

					dx = UnRound (subzone_ptr->X () - x);
					dy = UnRound (subzone_ptr->Y () - y);

					distance = sqrt (dx * dx + dy * dy);

					if (distance < sub_itr->max_distance) {
						weight = subzone_ptr->Data () * (sub_itr->max_distance - distance);
						if (weight > weight1) {
							weight2 = weight1;
							weight1 = weight;
						} else if (weight > weight2) {
							weight2 = weight;
						}
					}
				}
				weight = (weight1 + weight2) / 2.0;
			}
			output_file->Put_Field (sub_itr->loc_field, weight);
		}

		//---- set the data field id ----

		if (script_flag) {
			for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
				data = data_itr->data_db;
				field = data_itr->loc_field;

				if (field == zone_field) {
					join = zone;
				} else {
					join = input_file->Get_Integer (field);
				}
				if (!data->Read_Record (join)) {
					data->Reset_Record ();
				}
			}

			//---- set the polygon record index ----

			if (polygon_flag) {
				dx = UnRound (location_ptr->X ());
				dy = UnRound (location_ptr->Y ());
		
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

			//---- execute the conversion script ----

			if (program.Execute (num_in) == 0) continue;
		}

		//---- save the output fields ----

		if (!output_file->Write ()) {
			Error (String ("Writing %s Record %d") % output_file->File_Type () % loc);
		}
		num_out++;
	}
	End_Progress ();

	input_file->Close ();
	output_file->Close ();
}
