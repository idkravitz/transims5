//*********************************************************
//	Location_Access.cpp - walk location <-> node/stop/link
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Origin_Location
//---------------------------------------------------------

bool Path_Builder::Origin_Location (Trip_End_Array *org_ptr, Path_End_Array *from_ptr, bool stop_flag)
{
	int length, index, org, imped, ttime, acc, type;
	bool ab_flag;

	Location_Data *loc_ptr;
	Link_Data *link_ptr;
	Trip_End_Itr org_itr;
	Path_End path_end;
	Path_Data path_data;
	List_Data *acc_ptr;
	Access_Data *access_ptr;

	from_ptr->clear ();

	for (org=0, org_itr = org_ptr->begin (); org_itr != org_ptr->end (); org_itr++, org++) {
		if (org_itr->Index () < 0) continue;

		loc_ptr = &exe->location_array [org_itr->Index ()];

		//---- add the access links to nodes/stops ----

		if (exe->access_link_flag) {
			acc = exe->loc_access [org_itr->Index ()].Next (forward_flag);

			//---- process each access link leaving the origin ----

			for (; acc >= 0; acc = acc_ptr->Next (ab_flag)) {
				access_ptr = &exe->access_array [acc];
				acc_ptr = &exe->next_access (forward_flag) [acc];

				ab_flag = (access_ptr->From_Type () == LOCATION_ID && access_ptr->From_ID () == org_itr->Index ());

				type = access_ptr->Type (!ab_flag);
				if (type != NODE_ID && (!stop_flag || type != STOP_ID)) continue;

				index = access_ptr->ID (!ab_flag);

				//---- save the node/stop connection ----

				path_end.Clear ();
				path_end.Trip_End (org);
				path_end.End_Type (LOCATION_ID);
				path_end.Type (type);
				path_end.Index (index);
				path_end.Offset (0);

				path_data.Clear ();
				path_data.Imped (0);
				path_data.Time (org_itr->Time ());

				//--- save the access link attributes ----

				ttime = access_ptr->Time ();
				length = (int) (ttime * param.walk_speed + 0.5);
				imped = DTOI (ttime * param.value_walk);

				path_data.Add_Imped (imped);
				path_data.Add_Time ((forward_flag) ? ttime : -ttime);
				path_data.Length (length);
				path_data.Walk (length);
				path_data.Mode (WALK_MODE);
				path_data.From (acc);
				path_data.Type (ACCESS_ID);
				if (forward_flag) {
					if (!ab_flag) path_data.Dir (1);
				} else {
					if (ab_flag) path_data.Dir (1);
				}
				path_data.Status (1);

				path_end.push_back (path_data);

				from_ptr->push_back (path_end);
			}
		}

		//---- walk from the location to the link ----

		link_ptr = &exe->link_array [loc_ptr->Link ()];

		path_end.Clear ();
		path_end.Trip_End (org);
		path_end.End_Type (LOCATION_ID);
		path_end.Type (LINK_ID);
		path_end.Index (loc_ptr->Link ());

		if (loc_ptr->Dir () == 1) {
			path_end.Offset (link_ptr->Length () - loc_ptr->Offset ());
		} else {
			path_end.Offset (loc_ptr->Offset ());
		}
		path_data.Clear ();
		path_data.Imped (0);
		path_data.Time (org_itr->Time ());

		length = loc_ptr->Setback ();
		if (length < 1) length = 1;

		ttime = (int) (length / param.walk_speed + 0.5);
		imped = DTOI (ttime * param.value_walk);

		path_data.Add_Imped (imped);
		path_data.Add_Time ((forward_flag) ? ttime : -ttime);
		path_data.Add_Length (length);
		path_data.Add_Walk (length);
		path_data.Mode (WALK_MODE);
		path_data.From (org_itr->Index ());
		path_data.Type (LOCATION_ID);
		path_data.Status (1);

		path_end.push_back (path_data);

		from_ptr->push_back (path_end);
	}
	return (from_ptr->size () > 0);
}

//---------------------------------------------------------
//	Destination_Location
//---------------------------------------------------------

bool Path_Builder::Destination_Location (Trip_End_Array *des_ptr, Path_End_Array *to_ptr, bool stop_flag)
{
	int length, index, des, imped, ttime, acc, type, low_imp, hi_imp, tot_imp;
	bool ab_flag;

	Location_Data *loc_ptr;
	Link_Data *link_ptr;
	Trip_End_Itr des_itr;
	Path_End path_end;
	Path_End_Itr to_itr;
	Path_Data path_data;
	List_Data *acc_ptr;
	Access_Data *access_ptr;

	to_ptr->clear ();
	low_imp = MAX_INTEGER;
	hi_imp = 0;

	for (des=0, des_itr = des_ptr->begin (); des_itr != des_ptr->end (); des_itr++, des++) {
		if (des_itr->Index () < 0) continue;

		loc_ptr = &exe->location_array [des_itr->Index ()];

		//---- add the access links to nodes/stops ----

		if (exe->access_link_flag) {
			acc = exe->loc_access [des_itr->Index ()].Next (!forward_flag);

			//---- process each access link to the destination ----

			for (; acc >= 0; acc = acc_ptr->Next (ab_flag)) {
				access_ptr = &exe->access_array [acc];
				acc_ptr = &exe->next_access (!forward_flag) [acc];

				ab_flag = (access_ptr->From_Type () == LOCATION_ID && access_ptr->From_ID () == des_itr->Index ());

				type = access_ptr->Type (!ab_flag);
				if (type != NODE_ID && (!stop_flag || type != STOP_ID)) continue;

				index = access_ptr->ID (!ab_flag);

				//---- save a node/stop connection ----

				path_end.Clear ();
				path_end.Trip_End (des);
				path_end.End_Type (LOCATION_ID);
				path_end.Type (type);
				path_end.Index (index);
				path_end.Offset (0);

				//---- add a minimum wait ----

				tot_imp = 0;
				path_data.Clear ();	

				if (!forward_flag && param.min_wait > 0) {
					ttime = param.min_wait;
					tot_imp += imped = DTOI (ttime * param.value_wait);

					path_data.Imped (imped);
					path_data.Time (ttime);
					path_data.Length (0);
					path_data.Walk (0);
					path_data.Mode (WAIT_MODE);
					path_data.From (des_itr->Index ());
					path_data.Type (LOCATION_ID);

					path_end.push_back (path_data);
				}

				//--- save the access link attributes ----

				ttime = access_ptr->Time ();
				length = (int) (ttime * param.walk_speed + 0.5);
				tot_imp += imped = DTOI (ttime * param.value_walk);

				path_data.Imped (imped);
				path_data.Time (ttime);
				path_data.Length (length);
				path_data.Walk (length);
				path_data.Mode (WALK_MODE);
				path_data.From (acc);
				path_data.Type (ACCESS_ID);
				if (forward_flag) {
					if (ab_flag) path_data.Dir (1);
				} else {
					if (!ab_flag) path_data.Dir (0);
				}
				path_end.push_back (path_data);

				//---- check the impedance range ----

				if (tot_imp < low_imp) low_imp = tot_imp;
				if (tot_imp > hi_imp) hi_imp = tot_imp;

				path_data.Clear ();

				path_end.push_back (path_data);

				to_ptr->push_back (path_end);
			}
		}

		//---- connect to the link offset ----

		link_ptr = &exe->link_array [loc_ptr->Link ()];

		path_end.Clear ();
		path_end.Trip_End (des);
		path_end.End_Type (LOCATION_ID);
		path_end.Type (LINK_ID);
		path_end.Index (loc_ptr->Link ());

		if (loc_ptr->Dir () == 1) {
			path_end.Offset (link_ptr->Length () - loc_ptr->Offset ());
		} else {
			path_end.Offset (loc_ptr->Offset ());
		}

		//---- add a minimum wait ----

		tot_imp = 0;
		path_data.Clear ();

		if (!forward_flag && param.min_wait > 0) {
			ttime = param.min_wait;
			tot_imp += imped = DTOI (ttime * param.value_wait);

			path_data.Imped (imped);
			path_data.Time (ttime);
			path_data.Length (0);
			path_data.Walk (0);
			path_data.Mode (WAIT_MODE);
			path_data.From (des_itr->Index ());
			path_data.Type (LOCATION_ID);

			path_end.push_back (path_data);
		}

		//---- walk from the location to the link ----

		length = loc_ptr->Setback ();
		if (length < 1) length = 1;

		ttime = (int) (length / param.walk_speed + 0.5);
		tot_imp += imped = DTOI (ttime * param.value_walk);

		path_data.Imped (imped);
		path_data.Time (ttime);
		path_data.Length (length);
		path_data.Walk (length);
		path_data.Mode (WALK_MODE);
		path_data.From (des_itr->Index ());
		path_data.Type (LOCATION_ID);

		path_end.push_back (path_data);

		//---- check the impedance range ----

		if (tot_imp < low_imp) low_imp = tot_imp;
		if (tot_imp > hi_imp) hi_imp = tot_imp;

		path_data.Clear ();

		path_end.push_back (path_data);

		to_ptr->push_back (path_end);
	}

	//---- set the destination impedance difference ----

	if (low_imp < MAX_INTEGER) {
		imp_diff = hi_imp - low_imp;
	} else {
		imp_diff = 0;
	}
	if (to_ptr->size () == 0) return (0);

	//---- set the destination flags ----

	link_to_flag = node_to_flag = stop_to_flag = false;

	link_to.assign (exe->link_array.size (), 0);
	node_to.assign (exe->node_array.size (), 0);
	if (stop_flag) stop_to.assign (exe->stop_array.size (), 0);

	for (to_itr = to_ptr->begin (); to_itr != to_ptr->end (); to_itr++) {
		if (to_itr->Type () == LINK_ID) {
			link_to [to_itr->Index ()] = 1;
			link_to_flag = true;
		}
		if (to_itr->Type () == NODE_ID) {
			node_to [to_itr->Index ()] = 1;
			node_to_flag = true;
		}
		if (stop_flag && to_itr->Type () == STOP_ID) {
			stop_to [to_itr->Index ()] = 1;
			stop_to_flag = true;
		}
	}
	return (true);
}
