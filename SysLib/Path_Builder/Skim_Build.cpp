//*********************************************************
//	Skim_Build - Build a Path and Skim records
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Skim_Build
//---------------------------------------------------------

bool Path_Builder::Skim_Build (One_To_Many *data)
{
	int stat, index;
	Trip_End trip_end;
	Path_End path_end;
	Trip_End_Array *from_ptr, *to_ptr;
	Many_Itr many_itr;

	Location_Data *loc_ptr;
	Link_Data *link_ptr;

	Int_Map_Itr map_itr;

	if (data == 0) {
		cout << "\tOne to Many Pointer is Zero" << endl;
		return (false);
	}
	if (!initialized) {
		cout << "\tPath Building Requires TRANSIMS Router Services" << endl;
		return (false);
	}
	data_ptr = data;
	plan_ptr = &plan;

	if (data_ptr->Mode () >= MAX_MODE || !mode_path_flag [data_ptr->Mode ()]) return (false);

	//---- set the traveler parameters ----

	exe->Set_Parameters (param, data_ptr->Type ());

	param.mode = (Mode_Type) data_ptr->Mode ();
	param.use = (Use_Type) data_ptr->Use ();
	param.veh_type = data_ptr->Veh_Type ();

	parking_duration = 0;
	plan_flag = false;
	forward_flag = (data_ptr->Direction () != END_TIME);
	reroute_flag = false;

	//---- initialize the trip ends ----

	trip_org.clear ();
	trip_des.clear ();

	if (forward_flag) {
		from_ptr = &trip_org;
		to_ptr = &trip_des;
		time_limit = MAX_INTEGER;
	} else {
		from_ptr = &trip_des;
		to_ptr = &trip_org;
		time_limit = 0;
	}

	//---- set the from record ----

	map_itr = exe->location_map.find (data_ptr->Location ());

	if (map_itr == exe->location_map.end ()) {
		exe->Warning (String ("Location %d was Not Found") % data_ptr->Location ());
		data_ptr->Problem (LOCATION_PROBLEM);
		return (true);
	}
	loc_ptr = &exe->location_array [map_itr->second];

	trip_end.Index (map_itr->second);
	trip_end.Time (data_ptr->Time ());

	from_ptr->push_back (trip_end);

	path_end.Clear ();

	path_end.Trip_End (0);
	path_end.End_Type (LOCATION_ID);
	path_end.Type (LINK_ID);
	path_end.Index (loc_ptr->Link ());

	if (loc_ptr->Dir () == 1) {
		link_ptr = &exe->link_array [loc_ptr->Link ()];
		path_end.Offset (link_ptr->Length () - loc_ptr->Offset ());
	} else {
		path_end.Offset (loc_ptr->Offset ());
	}
	from_array.push_back (path_end);

	//---- set the to record ----

	for (index=0, many_itr = data_ptr->begin (); many_itr != data_ptr->end (); many_itr++, index++) {
		many_itr->Clear_Totals ();

		if (many_itr->Location () == data_ptr->Location ()) {
			
			trip_end.Index (-1);

		} else {
			map_itr = exe->location_map.find (many_itr->Location ());

			if (map_itr == exe->location_map.end ()) {
				exe->Warning (String ("Location %d was Not Found") % many_itr->Location ());
				many_itr->Problem (LOCATION_PROBLEM);

				trip_end.Index (-1);

			} else {
				loc_ptr = &exe->location_array [map_itr->second];

				trip_end.Index (map_itr->second);

				path_end.Clear ();
				path_end.Trip_End (index);
				path_end.End_Type (LOCATION_ID);
				path_end.Type (LINK_ID);
				path_end.Index (loc_ptr->Link ());

				if (loc_ptr->Dir () == 1) {
					link_ptr = &exe->link_array [loc_ptr->Link ()];
					path_end.Offset (link_ptr->Length () - loc_ptr->Offset ());
				} else {
					path_end.Offset (loc_ptr->Offset ());
				}
				to_array.push_back (path_end);
			}
		}
		trip_end.Time (time_limit);

		to_ptr->push_back (trip_end);
	}

	//---- build the paths ----

	stat = Build_Path (-1);
	if (stat > 0) data_ptr->Problem (stat);

	return ((stat >= 0));
}
