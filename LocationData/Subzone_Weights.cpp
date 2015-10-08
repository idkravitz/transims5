//*********************************************************
//	Subzone_Weights.cpp - subzone weight processing
//*********************************************************

#include "LocationData.hpp"

#include <math.h>

//---------------------------------------------------------
//	Subzone_Weights
//---------------------------------------------------------

void LocationData::Subzone_Weights (void)
{
	int loc;
	int x, y, zone, nzone, count;
	double dx, dy, value, data;

	Location_Itr location_itr;
	Subzone_Data *subzone_ptr;
	Sub_Group_Itr sub_itr;
	Int2_Key key, key2;
	Int2_Map_Itr int2_itr;
	I2_Dbl_Map_Itr wt_itr;
	Int_Dbl_Map_Itr zone_itr;

	nzone = 0;
	
	Show_Message ("Calculating Subzone Allocation Weights -- Record");
	Set_Progress ();

	//---- scan location records ----

	for (location_itr = location_array.begin (); location_itr != location_array.end (); location_itr++) {

		loc = location_itr->Location ();
		zone = location_itr->Zone ();

		if (zone >= 0 && zone_file_flag) {
			zone = zone_array [zone].Zone ();
		}
		x = location_itr->X ();
		y = location_itr->Y ();

		//---- calculate the subzone distances ----

		for (sub_itr = sub_group.begin (); sub_itr != sub_group.end (); sub_itr++) {
			if (sub_itr->loc_field < 0 || sub_itr->max_distance > 0) continue;

			if (zone > nzone) nzone = zone;

			key.first = zone;
			key.second = 0;

			for (int2_itr = sub_itr->data_map.lower_bound (key); int2_itr != sub_itr->data_map.end (); int2_itr++) {
				if (int2_itr->first.first != zone) break;

				key.first = int2_itr->first.second;
				key.second = loc;

				wt_itr = subzone_weight.find (key);

				if (wt_itr == subzone_weight.end ()) {
					subzone_ptr = &sub_itr->data [int2_itr->second];

					dx = UnRound (subzone_ptr->X () - x);
					dy = UnRound (subzone_ptr->Y () - y);

					value = sqrt (dx * dx + dy * dy);

					subzone_weight.insert (I2_Dbl_Map_Data (key, value));
				}
			}
		}
	}
 
	//---- calculate the subzone weights ----

	if (nzone == 0) {
		End_Progress ();
		return;
	} 

	//---- calculate the subzone weights ----

	Set_Progress ();

	for (sub_itr = sub_group.begin (); sub_itr != sub_group.end (); sub_itr++) {
		if (sub_itr->loc_field < 0 || sub_itr->max_distance > 0) continue;

		for (zone=1; zone <= nzone; zone++) {
			Show_Progress ();

			key.first = zone;
			key.second = 0;
			data = 0;
			count = 0;

			for (int2_itr = sub_itr->data_map.lower_bound (key); int2_itr != sub_itr->data_map.end (); int2_itr++) {
				if (int2_itr->first.first != zone) break;

				subzone_ptr = &sub_itr->data [int2_itr->second];
				data += subzone_ptr->Data ();
				count++;

				key2.first = subzone_ptr->ID ();
				key2.second = 0;
				dx = -1.0;

				for (wt_itr = subzone_weight.lower_bound (key2); wt_itr != subzone_weight.end (); wt_itr++) {
					if (wt_itr->first.first != key2.first) break;
					if (wt_itr->second > dx) dx = wt_itr->second;
				}
				if (dx < 0.0) continue;
				dy = 0.0;

				for (wt_itr = subzone_weight.lower_bound (key2); wt_itr != subzone_weight.end (); wt_itr++) {
					if (wt_itr->first.first != key2.first) break;
					wt_itr->second = dx - wt_itr->second;
					if (wt_itr->second < 1.0) wt_itr->second = 1.0;
					dy += wt_itr->second;
				}
				for (wt_itr = subzone_weight.lower_bound (key2); wt_itr != subzone_weight.end (); wt_itr++) {
					if (wt_itr->first.first != key2.first) break;
					wt_itr->second /= dy;
				}
			}

			//---- normalize the zone data ----

			if (count == 0) continue;

			zone_itr = sub_itr->zone_data.find (zone);
			if (zone_itr->second != data) {
				if (data == 0) {
					key.first = zone;
					key.second = 0;
					data = zone_itr->second / count;

					for (int2_itr = sub_itr->data_map.lower_bound (key); int2_itr != sub_itr->data_map.end (); int2_itr++) {
						if (int2_itr->first.first != zone) break;

						subzone_ptr = &sub_itr->data [int2_itr->second];
						subzone_ptr->Data (data);
					}
				} else {
					key.first = zone;
					key.second = 0;
					data = zone_itr->second / data;

					for (int2_itr = sub_itr->data_map.lower_bound (key); int2_itr != sub_itr->data_map.end (); int2_itr++) {
						if (int2_itr->first.first != zone) break;

						subzone_ptr = &sub_itr->data [int2_itr->second];
						subzone_ptr->Data (subzone_ptr->Data () * data);
					}
				}
			}
		}

		break;
	}
	End_Progress ();
}
