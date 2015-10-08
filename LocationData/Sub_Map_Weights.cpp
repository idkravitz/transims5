//*********************************************************
//	Subzone_Map_Weights.cpp - subzone weight processing
//*********************************************************

#include "LocationData.hpp"

#include <math.h>

//---------------------------------------------------------
//	Subzone_Map_Weights
//---------------------------------------------------------

void LocationData::Subzone_Map_Weights (void)
{
	int loc, weight;
	int x, y, zone, nzone;
	double dx, dy, total, value;

	Location_Itr location_itr;
	Subzone_Data *subzone_ptr;
	Sub_Group_Itr sub_itr;
	Int2_Key key, key2;
	Int2_Map_Itr int2_itr;
	Int_Map_Itr int_itr;
	I2_Dbl_Map_Itr i2_itr, wt_itr;
	Int_Dbl_Map_Itr zone_itr, loc_itr;

	nzone = 0;
		
	Show_Message ("Calculating Subzone Allocation Weights -- Record");
	Set_Progress ();

	//---- count location records ----

	for (location_itr = location_array.begin (); location_itr != location_array.end (); location_itr++) {
		Show_Progress ();

		loc = location_itr->Location ();
		zone = location_itr->Zone ();

		x = location_itr->X ();
		y = location_itr->Y ();

		if (zone >= 0 && zone_file_flag) {
			zone = zone_array [zone].Zone ();
		}

		//---- create working space for location weights ----

		loc_weight.insert (Int_Dbl_Map_Data (loc, 0.0));

		if (zone > nzone) nzone = zone;

		//---- get a subzone data for coordinates ----

		for (sub_itr = sub_group.begin (); sub_itr != sub_group.end (); sub_itr++) {
			if (sub_itr->loc_field < 0 || sub_itr->max_distance > 0) continue;

			//---- distance from the activity location to the subzone ----

			key.first = zone;
			key.second = 0;

			for (i2_itr = subzone_map.lower_bound (key); i2_itr != subzone_map.end (); i2_itr++) {
				if (i2_itr->first.first != zone) break;
			
				key.first = i2_itr->first.second;
				key.second = loc;

				wt_itr = subzone_weight.find (key);

				if (wt_itr == subzone_weight.end ()) {
					int2_itr = sub_itr->data_map.find (Int2_Key (0, key.first));
					if (int2_itr == sub_itr->data_map.end ()) continue;

					subzone_ptr = &sub_itr->data [int2_itr->second];

					dx = UnRound (subzone_ptr->X () - x);
					dy = UnRound (subzone_ptr->Y () - y);

					value = sqrt (dx * dx + dy * dy);

					subzone_weight.insert (I2_Dbl_Map_Data (key, value));
				}
			}			
			break;
		}
	}
	if (nzone == 0) {
		End_Progress ();
		return;
	} 

	//---- calculate the subzone weights ----

	Set_Progress ();

	for (zone=1; zone <= nzone; zone++) {
		Show_Progress ();

		//---- distance from the activity location to the subzone ----

		key.first = zone;
		key.second = 0;

		for (i2_itr = subzone_map.lower_bound (key); i2_itr != subzone_map.end (); i2_itr++) {
			if (i2_itr->first.first != zone) break;
		
			key.first = i2_itr->first.second;
			key.second = 0;

			dx = -1.0;

			for (wt_itr = subzone_weight.lower_bound (key); wt_itr != subzone_weight.end (); wt_itr++) {
				if (wt_itr->first.first != key.first) break;
				if (wt_itr->second > dx) dx = wt_itr->second;
			}
			if (dx < 0.0) continue;
			dy = 0.0;

			for (wt_itr = subzone_weight.lower_bound (key); wt_itr != subzone_weight.end (); wt_itr++) {
				if (wt_itr->first.first != key.first) break;
				wt_itr->second = dx - wt_itr->second;
				if (wt_itr->second < 1.0) wt_itr->second = 1.0;
				dy += wt_itr->second;
			}
			for (wt_itr = subzone_weight.lower_bound (key); wt_itr != subzone_weight.end (); wt_itr++) {
				if (wt_itr->first.first != key.first) break;
				wt_itr->second /= dy;
			}
		}
	}
	End_Progress ();

	Show_Message ("Distribute Zone Attributes -- Record");
	Set_Progress ();

	for (sub_itr = sub_group.begin (); sub_itr != sub_group.end (); sub_itr++) {
		if (sub_itr->loc_field < 0 || sub_itr->max_distance > 0) continue;
		Set_Progress ();

		//---- process each zone ----

		for (zone_itr = sub_itr->zone_data.begin (); zone_itr != sub_itr->zone_data.end (); zone_itr++) {
			Show_Progress ();
			if (zone_itr->second == 0) continue;

			//---- zero the location weights ----

			for (loc_itr = loc_weight.begin (); loc_itr != loc_weight.end (); loc_itr++) {
				loc_itr->second = 0.0;
			}
			total = 0.0;

			key.first = zone = zone_itr->first;
			key.second = 0;

			for (i2_itr = subzone_map.lower_bound (key); i2_itr != subzone_map.end (); i2_itr++) {
				if (i2_itr->first.first != zone) break;

				int2_itr = sub_itr->data_map.find (Int2_Key (0, i2_itr->first.second));
				if (int2_itr == sub_itr->data_map.end ()) continue;

				subzone_ptr = &sub_itr->data [int2_itr->second];

				weight = DTOI (subzone_ptr->Data () * i2_itr->second);
				if (weight == 0) continue;

				key2.first = i2_itr->first.second;
				key2.second = 0;

				for (wt_itr = subzone_weight.lower_bound (key2); wt_itr != subzone_weight.end (); wt_itr++) {
					if (wt_itr->first.first != key2.first) break;

					int_itr = loc_zone_map.find (wt_itr->first.second);
					if (int_itr == loc_zone_map.end () || int_itr->second != zone) continue;

					loc_itr = loc_weight.find (wt_itr->first.second);
					if (loc_itr == loc_weight.end ()) continue;

					value = wt_itr->second * weight;

					total += value;
					loc_itr->second += value;
				}
			}

			//---- use distance weights ----

			if (total <= 0.01) {
				for (i2_itr = subzone_map.lower_bound (key); i2_itr != subzone_map.end (); i2_itr++) {
					if (i2_itr->first.first != zone) break;
					if (i2_itr->second == 0.0) continue;

					key2.first = i2_itr->first.second;
					key2.second = 0;

					for (wt_itr = subzone_weight.lower_bound (key2); wt_itr != subzone_weight.end (); wt_itr++) {
						if (wt_itr->first.first != key2.first) break;

						int_itr = loc_zone_map.find (wt_itr->first.second);
						if (int_itr == loc_zone_map.end () || int_itr->second != zone) continue;

						loc_itr = loc_weight.find (wt_itr->first.second);
						if (loc_itr == loc_weight.end ()) continue;

						value = wt_itr->second * i2_itr->second;

						total += value;
						loc_itr->second += value;
					}
				}
			}

			//---- normalize the weight ----

			if (total > 0.0) {
				value = 0.5;

				for (loc_itr = loc_weight.begin (); loc_itr != loc_weight.end (); loc_itr++) {
					if (loc_itr->second == 0.0) continue;

					loc_itr->second *= zone_itr->second / total;

					if (!sub_itr->float_flag) {
						loc_itr->second += value;
						weight = DTOI (loc_itr->second);
						value = loc_itr->second - weight;
						if (weight <= 0) continue;
						loc_itr->second = weight;
					}
					if (loc_itr->second > 0) {
						sub_itr->loc_weight.insert (*loc_itr);
					}
				}
			}
		}
	}
	End_Progress ();
}
