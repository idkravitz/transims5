//*********************************************************
//	Read_Plans.cpp - Read the Plan File
//*********************************************************

#include "ExportPlans.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void ExportPlans::Read_Plans (void)
{
	int part, num, first, part_num, org, des, period, link, dir, veh_type;
	bool start_flag, vehicle_flag;
	Dtime start;

	Plan_File *file = (Plan_File *) System_File_Handle (PLAN);
		
	Int_Map_Itr map_itr;
	Plan_Data plan_rec;
	Plan_Leg_Itr leg_itr;
	Location_Data *loc_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Vehicle_Map_Itr vehicle_itr;
	Vehicle_Index veh_index;
	Vehicle_Data *vehicle_ptr;
	Veh_Type_Data *veh_type_ptr;
	Trip_Table_Map_Stat trip_map_stat;
	Trip_Table trip_table, *table_ptr;
	Matrix_Index trip_index;
	Trip_Table_Stat trip_stat;
	Integers volumes;
	
	num = sum_periods.Num_Periods ();

	volumes.assign (MAX (num, 1), 0);

	//---- check the partition number ----

	period = first = 0;

	if (file->Part_Flag () && First_Partition () != file->Part_Number ()) {
		file->Open (0);
	} else if (First_Partition () >= 0) {
		first = First_Partition ();
	}
	veh_type = 1;
	vehicle_flag = System_File_Flag (VEHICLE);

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
		}
	
		//---- store the plan data ----

		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read_Plan (plan_rec)) {
			Show_Progress ();

			if (vehicle_flag) {
				if (plan_rec.Vehicle () <= 0) continue;
				veh_index.Household (plan_rec.Household ());
				veh_index.Vehicle (plan_rec.Vehicle ());

				vehicle_itr = vehicle_map.find (veh_index);
				if (vehicle_itr == vehicle_map.end ()) continue;
				vehicle_ptr = &vehicle_array [vehicle_itr->second];

				veh_type_ptr = &veh_type_array [vehicle_ptr->Type ()];
				veh_type = veh_type_ptr->Type ();
			}
			if (select_vehicles && !vehicle_range.In_Range (veh_type)) continue;

			start = plan_rec.Depart ();
			org = plan_rec.Origin ();
			des = plan_rec.Destination ();

			if (select_flag) {
				map_itr = location_map.find (org);
				if (map_itr != location_map.end ()) {
					loc_ptr = &location_array [map_itr->second];
					start_flag = (loc_ptr->Zone () > 0);
				} else {
					start_flag = false;
				}

				//---- trace the path ----

				for (leg_itr = plan_rec.begin (); leg_itr != plan_rec.end (); leg_itr++) {
					if (leg_itr->Mode () == DRIVE_MODE && leg_itr->Link_Type ()) {
						link = leg_itr->Link_ID ();
						dir = leg_itr->Link_Dir ();

						map_itr = link_map.find (link);
						if (map_itr != link_map.end ()) {
							link_ptr = &link_array [map_itr->second];
			
							if (link_ptr->Divided () != 0) {
								if (dir == 0) {
									if (link_ptr->Divided () == 1) {
										node_ptr = &node_array [link_ptr->Bnode ()];
										des = node_ptr->Control ();
										break;
									} else if (!start_flag) {
										node_ptr = &node_array [link_ptr->Anode ()];
										org = node_ptr->Control ();
									}
								} else {
									if (link_ptr->Divided () == 2) {
										node_ptr = &node_array [link_ptr->Anode ()];
										des = node_ptr->Control ();
										break;
									} else if (!start_flag) {
										node_ptr = &node_array [link_ptr->Bnode ()];
										org = node_ptr->Control ();
									}
								}
								start_flag = true;
							}
						}
					}
					if (!start_flag) start += leg_itr->Time ();
				}
				if (!start_flag) continue;
			}
			if (map_flag) {
				map_itr = loc_zone_map.find (org);
				if (map_itr != loc_zone_map.end ()) {
					org = map_itr->second;
				}
				map_itr = loc_zone_map.find (des);
				if (map_itr != loc_zone_map.end ()) {
					des = map_itr->second;
				}
			}
			trip_index.Origin (org);
			trip_index.Destination (des);

			if (num > 0) {
				period = sum_periods.Period (start);
				if (period < 0) continue;
			}
			trip_map_stat = trip_tables.insert (Trip_Table_Map_Data (veh_type, trip_table));
			
			table_ptr = &trip_map_stat.first->second;

			trip_stat = table_ptr->insert (Trip_Table_Data (trip_index, volumes));

			trip_stat.first->second [period]++;
		}
		End_Progress ();
	}
	file->Close ();
}
