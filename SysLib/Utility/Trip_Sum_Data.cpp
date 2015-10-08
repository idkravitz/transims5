//*********************************************************
//	Trip_Sum_Data.cpp - Trip Summary Data
//*********************************************************

#include "Trip_Sum_Data.hpp"
#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Add_Trip -- add data to the summary group
//---------------------------------------------------------

void Trip_Sum_Data::Add_Trip (int group, double distance, double time, int turns, int count)
{
	if (count < 1) return;

	Sum_Data *sum_ptr;
	Int_Map_Itr map_itr;

	map_itr = group_map.find (group);
	if (map_itr == group_map.end ()) {
		Sum_Data sum_data;
		Int_Map_Stat map_stat;

		memset (&sum_data, '\0', sizeof (sum_data));
		sum_data.group = group;

		map_stat = group_map.insert (Int_Map_Data (group, (int) sum_array.size ()));
		if (!map_stat.second) {
			exe->Warning ("Insufficient Memory for Trip Summary Group ") << group;
			return;
		}
		sum_array.push_back (sum_data);
		group = map_stat.first->second;
	} else {
		group = map_itr->second;
	}
	sum_ptr = &sum_array [group];

	double speed;
	if (time > 0) {
		speed = distance / time;
	} else {
		speed = distance;
	}
	distance = UnRound (distance);
	
	if (sum_ptr->count == 0) {
		sum_ptr->min_distance = sum_ptr->max_distance = distance;
		sum_ptr->distance = distance * count;
		sum_ptr->distance2 = distance * distance * count;

		sum_ptr->min_time = sum_ptr->max_time = time;
		sum_ptr->time = time * count;
		sum_ptr->time2 = time * time * count;

		sum_ptr->min_speed = sum_ptr->max_speed = speed;
		sum_ptr->speed = speed * count;
		sum_ptr->speed2 = speed * speed * count;

		sum_ptr->min_turns = sum_ptr->max_turns = turns;
		sum_ptr->turns = turns * count;
		sum_ptr->turns2 = turns * turns * count;
		sum_ptr->count = count;
	} else {
		if (distance < sum_ptr->min_distance) sum_ptr->min_distance = distance;
		if (distance > sum_ptr->max_distance) sum_ptr->max_distance = distance;

		sum_ptr->distance += distance * count;
		sum_ptr->distance2 += distance * distance * count;

		if (time < sum_ptr->min_time) sum_ptr->min_time = time;
		if (time > sum_ptr->max_time) sum_ptr->max_time = time;

		sum_ptr->time += time * count;
		sum_ptr->time2 += time * time * count;

		if (speed < sum_ptr->min_speed) sum_ptr->min_speed = speed;
		if (speed > sum_ptr->max_speed) sum_ptr->max_speed = speed;

		sum_ptr->speed += speed * count;
		sum_ptr->speed2 += speed * speed * count;

		if (turns < sum_ptr->min_turns) sum_ptr->min_turns = turns;
		if (turns > sum_ptr->max_turns) sum_ptr->max_turns = turns;

		sum_ptr->turns += turns * count;
		sum_ptr->turns2 += (double) turns * turns * count;
		sum_ptr->count += count;
	}
}

//---------------------------------------------------------
//	Add_Trip -- add data to the summary group
//---------------------------------------------------------

void Trip_Sum_Data::Add_Trip (Dtime tod, double distance, double time, int turns, int count)
{
	if (count < 1) return;

	int group = Period (tod);

	if (group >= 0) {
		Add_Trip (group, distance, time, turns, count);
	}
}

//---------------------------------------------------------
//	Replicate -- replicated the data structures
//---------------------------------------------------------

void Trip_Sum_Data::Replicate (Trip_Sum_Data &data)
{
	Copy_Periods (data);
}

//---------------------------------------------------------
//	Merge_Data -- copy data structures
//---------------------------------------------------------

void Trip_Sum_Data::Merge_Data (Trip_Sum_Data &data)
{
	Sum_Itr sum_itr;

	for (sum_itr = data.sum_array.begin (); sum_itr != data.sum_array.end (); sum_itr++) {
		if (sum_itr->count < 1) continue;

		int index;
		Sum_Data *sum_ptr;
		Int_Map_Itr map_itr;

		map_itr = group_map.find (sum_itr->group);
		if (map_itr == group_map.end ()) {
			Sum_Data sum_data;
			Int_Map_Stat map_stat;

			memset (&sum_data, '\0', sizeof (sum_data));
			sum_data.group = sum_itr->group;

			map_stat = group_map.insert (Int_Map_Data (sum_itr->group, (int) sum_array.size ()));
			if (!map_stat.second) {
				exe->Warning ("Insufficient Memory for Trip Summary Group ") << sum_itr->group;
				continue;
			}
			sum_array.push_back (sum_data);
			index = map_stat.first->second;
		} else {
			index = map_itr->second;
		}
		sum_ptr = &sum_array [index];
	
		if (sum_ptr->count == 0) {
			sum_ptr->min_distance = sum_itr->min_distance;
			sum_ptr->max_distance = sum_itr->max_distance;
			sum_ptr->min_time = sum_itr->min_time;
			sum_ptr->max_time = sum_itr->max_time;
			sum_ptr->min_speed = sum_itr->min_speed;
			sum_ptr->max_speed = sum_itr->max_speed;
			sum_ptr->min_turns = sum_itr->min_turns;
			sum_ptr->max_turns = sum_itr->max_turns;
		} else {
			if (sum_ptr->min_distance > sum_itr->min_distance) sum_ptr->min_distance = sum_itr->min_distance;
			if (sum_ptr->max_distance < sum_itr->max_distance) sum_ptr->max_distance = sum_itr->max_distance;
			if (sum_ptr->min_time > sum_itr->min_time) sum_ptr->min_time = sum_itr->min_time;
			if (sum_ptr->max_time < sum_itr->max_time) sum_ptr->max_time = sum_itr->max_time;
			if (sum_ptr->min_speed > sum_itr->min_speed) sum_ptr->min_speed = sum_itr->min_speed;
			if (sum_ptr->max_speed < sum_itr->max_speed) sum_ptr->max_speed = sum_itr->max_speed;
			if (sum_ptr->min_turns > sum_itr->min_turns) sum_ptr->min_turns = sum_itr->min_turns;
			if (sum_ptr->max_turns < sum_itr->max_turns) sum_ptr->max_turns= sum_itr->max_turns;
		}
		sum_ptr->count += sum_itr->count;
		sum_ptr->distance += sum_itr->distance;
		sum_ptr->distance2 += sum_itr->distance2;
		sum_ptr->time += sum_itr->time;
		sum_ptr->time2 += sum_itr->time2;
		sum_ptr->speed += sum_itr->speed;
		sum_ptr->speed2 += sum_itr->speed2;
		sum_ptr->turns += sum_itr->turns;
		sum_ptr->turns2 += sum_itr->turns2;
	}
}

//---------------------------------------------------------
//	Trip_Sum_Report
//---------------------------------------------------------

void Trip_Sum_Data::Trip_Sum_Report (int number, char *_title, char *_key1, char *_key2)
{
	Int_Map_Itr map_itr;
	Sum_Data *sum_ptr, total;
	Units_Type units;

	if (_title != 0) {
		title = _title;
	} else {
		title.clear ();
	}
	keys = 0;
	if (_key1 != 0) {
		key1 = _key1;
		keys++;
	} else {
		key1.clear ();
	}
	if (_key2 != 0) {
		key2 = _key2;
		keys++;
	} else {
		key2.clear ();
	}

	//---- print the report ----

	exe->Header_Number (number);

	if (!exe->Break_Check ((int) group_map.size () + 6)) {
		exe->Print (1);
		Trip_Sum_Header ();
	}
	units = (Metric_Flag ()) ? KILOMETERS : MILES;
	memset (&total, '\0', sizeof (total));

	for (map_itr = group_map.begin (); map_itr != group_map.end (); map_itr++) {
		sum_ptr = &sum_array [map_itr->second];

		if (keys == 2) {
			exe->Print (1, String ("%3d-%-3d") % (sum_ptr->group >> 16) % (sum_ptr->group & 0x00FF));
		} else {
			exe->Print (1, String ("%5d  ") % sum_ptr->group);
		}
		exe->Print (0, String (" %9d %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf") % sum_ptr->count %
			Convert_Units (sum_ptr->min_distance, units) % Convert_Units (sum_ptr->max_distance, units) % 
			Convert_Units (Average (sum_ptr->count, sum_ptr->distance), units) % 
			Convert_Units (Std_Dev (sum_ptr->count, sum_ptr->distance, sum_ptr->distance2), units) %
			Convert_Units (sum_ptr->min_time, MINUTES) % Convert_Units (sum_ptr->max_time, MINUTES) % 
			Convert_Units (Average (sum_ptr->count, sum_ptr->time), MINUTES) %
			Convert_Units (Std_Dev (sum_ptr->count, sum_ptr->time, sum_ptr->time2), MINUTES));

		if (total.count == 0) {
			total.min_distance = sum_ptr->min_distance;
			total.max_distance = sum_ptr->max_distance;
			total.min_time = sum_ptr->min_time;
			total.max_time = sum_ptr->max_time;
		} else {
			if (total.min_distance > sum_ptr->min_distance) total.min_distance = sum_ptr->min_distance;
			if (total.max_distance < sum_ptr->max_distance) total.max_distance = sum_ptr->max_distance;
			if (total.min_time > sum_ptr->min_time) total.min_time = sum_ptr->min_time;
			if (total.max_time < sum_ptr->max_time) total.max_time = sum_ptr->max_time;
		}
		total.count += sum_ptr->count;
		total.distance += sum_ptr->distance;
		total.distance2 += sum_ptr->distance2;
		total.time += sum_ptr->time;
		total.time2 += sum_ptr->time2;
	}
	exe->Print (2, String ("Total   %9d %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf %8.2lf") % total.count %
		Convert_Units (total.min_distance, units) % Convert_Units (total.max_distance, units) % 
		Convert_Units (Average (total.count, total.distance), units) % 
		Convert_Units (Std_Dev (total.count, total.distance, total.distance2), units) %
		Convert_Units (total.min_time, MINUTES) % Convert_Units (total.max_time, MINUTES) % 
		Convert_Units (Average (total.count, total.time), MINUTES) %
		Convert_Units (Std_Dev (total.count, total.time, total.time2), MINUTES));

	exe->Header_Number (0);
}

//---------------------------------------------------------
//	Trip_Sum_Header
//---------------------------------------------------------

void Trip_Sum_Data::Trip_Sum_Header (void)
{
	if (!title.empty ()) {
		exe->Print (1, title);
	} else {
		exe->Print (1, "Trip Length Summary Report");
	}
	if (keys < 2 || key1.empty ()) {
		exe->Print (2, String ("%19c") % BLANK);
	} else {
		exe->Print (2, String ("%-7.7s%12c") % key1 % BLANK);
	}
	if (Metric_Flag ()) {
		exe->Print (0, "----- Distance (kilometers) ------  --------- Time (minutes) ---------");
	} else {
		exe->Print (0, "-------- Distance (miles) --------  --------- Time (minutes) ---------");
	}

	if (keys == 2 && !key2.empty ()) {
		exe->Print (1, String ("%-7.7s") % key2);
	} else if (keys == 1 && !key1.empty ()) {
		exe->Print (1, String ("%-7.7s") % key1);
	} else {
		exe->Print (1, "Group  ");
	}
	exe->Print (0, "     Trips  Minimum  Maximum  Average   StdDev  Minimum  Maximum  Average   StdDev");
	exe->Print (1);
}

/*********************************************|***********************************************

	[title]

	[key1]             -------- Distance (miles) --------  --------- Time (minutes) ---------
	[key1]             ----- Distance (kilometers) ------  --------- Time (minutes) ---------
	[key2]      Trips  Minimum  Maximum  Average   StdDev  Minimum  Maximum  Average   StdDev 

	ddd-ddd ddddddddd  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff
 
	Total   ddddddddd  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff  ffff.ff

**********************************************|***********************************************/ 

//---------------------------------------------------------
//	Travel_Sum_Report
//---------------------------------------------------------

void Trip_Sum_Data::Travel_Sum_Report (int number, bool _veh_flag)
{
	Int_Map_Itr map_itr;
	Sum_Data *sum_ptr, total;
	Units_Type units, spd_units;

	veh_flag = _veh_flag;

	//---- print the report ----

	exe->Header_Number (number);

	if (!exe->Break_Check ((int) group_map.size () + 6)) {
		exe->Print (1);
		Travel_Sum_Header ();
	}
	if (Metric_Flag ()) {
		units = KILOMETERS;
		spd_units = KPH;
	} else {
		units = MILES;
		spd_units = MPH;
	}
	memset (&total, '\0', sizeof (total));

	for (map_itr = group_map.begin (); map_itr != group_map.end (); map_itr++) {
		sum_ptr = &sum_array [map_itr->second];

		//---- print each time period ---

		exe->Print (1, String (" %3d %7d %7.0lf %8.0lf %7.2lf %7.2lf %6.2lf %6.2lf %7.2lf %7.2lf %6.2lf %6.2lf") %
			sum_ptr->group % sum_ptr->count %
			Convert_Units (sum_ptr->time, HOURS) % Convert_Units (sum_ptr->distance, units) %
			Convert_Units (Average (sum_ptr->count, sum_ptr->time), MINUTES) %
			Convert_Units (Average (sum_ptr->count, sum_ptr->distance), units) %
			Convert_Units (Average (sum_ptr->count, sum_ptr->speed), spd_units) %
			Average (sum_ptr->count, sum_ptr->turns) %
			Convert_Units (Std_Dev (sum_ptr->count, sum_ptr->time, sum_ptr->time2), MINUTES) %
			Convert_Units (Std_Dev (sum_ptr->count, sum_ptr->distance, sum_ptr->distance2), units) %
			Convert_Units (Std_Dev (sum_ptr->count, sum_ptr->speed, sum_ptr->speed2), spd_units) %
			Std_Dev (sum_ptr->count, sum_ptr->turns, sum_ptr->turns2));

		if (total.count == 0) {
			total.min_distance = sum_ptr->min_distance;
			total.max_distance = sum_ptr->max_distance;
			total.min_time = sum_ptr->min_time;
			total.max_time = sum_ptr->max_time;
			total.min_speed = sum_ptr->min_speed;
			total.max_speed = sum_ptr->max_speed;
			total.min_turns = sum_ptr->min_turns;
			total.max_turns = sum_ptr->max_turns;
		} else {
			if (total.min_distance > sum_ptr->min_distance) total.min_distance = sum_ptr->min_distance;
			if (total.max_distance < sum_ptr->max_distance) total.max_distance = sum_ptr->max_distance;
			if (total.min_time > sum_ptr->min_time) total.min_time = sum_ptr->min_time;
			if (total.max_time < sum_ptr->max_time) total.max_time = sum_ptr->max_time;
			if (total.min_speed > sum_ptr->min_speed) total.min_speed = sum_ptr->min_speed;
			if (total.max_speed < sum_ptr->max_speed) total.max_speed = sum_ptr->max_speed;
			if (total.min_turns > sum_ptr->min_turns) total.min_turns = sum_ptr->min_turns;
			if (total.max_turns < sum_ptr->max_turns) total.max_turns = sum_ptr->max_turns;
		}
		total.count += sum_ptr->count;
		total.distance += sum_ptr->distance;
		total.distance2 += sum_ptr->distance2;
		total.time += sum_ptr->time;
		total.time2 += sum_ptr->time2;
		total.speed += sum_ptr->speed;
		total.speed2 += sum_ptr->speed2;
		total.turns += sum_ptr->turns;
		total.turns2 += sum_ptr->turns2;
	}
	exe->Header_Number (0);

	//---- print the total report ----

	if (exe->Break_Check (12) ) {
		if (veh_flag) {
			exe->Print (2, "Travel Summary Report");
		} else {
			exe->Print (2, "Transit Passenger Summary Report");
		}
	}
	if (veh_flag) {
		exe->Print (2, "Total Vehicle Trips = ") << total.count;
		exe->Print (1, String ("Total Vehicle Hours of Travel = %.1lf hours") % Convert_Units (total.time, HOURS));
		if (Metric_Flag ()) {
			exe->Print (1, String ("Total Vehicle Kilometers of Travel = %.1lf kilometers") % Convert_Units (total.distance, units));
		} else {
			exe->Print (1, String ("Total Vehicle Miles of Travel = %.1lf miles") % Convert_Units (total.distance, units));
		}
		exe->Print (1, String ("Total Number of Turns = %.0lf") % total.turns);
	} else {
		exe->Print (2, "Total Passenger Trips = ") << total.count;
		exe->Print (1, String ("Total Passengers Hours of Travel = %.1lf hours") % Convert_Units (total.time, HOURS));
		if (Metric_Flag ()) {
			exe->Print (1, String ("Total Passengers Kilometers of Travel = %.1lf kilometers") % Convert_Units (total.distance, units));
		} else {
			exe->Print (1, String ("Total Passenger Miles of Travel = %.1lf miles") % Convert_Units (total.distance, units));
		}
		exe->Print (1, String ("Total Number of Transfers = %.0lf") % total.turns);
	}

	exe->Print (2, "                        Minimum  Maximum  Average   StdDev");
	
	exe->Print (1, String ("Travel Time (minutes)   %7.2lf  %7.2lf  %7.2lf  %7.2lf") %
		Convert_Units (total.min_time, MINUTES) % Convert_Units (total.max_time, MINUTES) %
		Convert_Units (Average (total.count, total.time), MINUTES) %
		Convert_Units (Std_Dev (total.count, total.time, total.time2), MINUTES));

	exe->Print (1, String ("Trip Length %s     %7.2lf  %7.2lf  %7.2lf  %7.2lf") % 
		((Metric_Flag ()) ? "(kmtrs)" : "(miles)") %
		Convert_Units (total.min_distance, units) % Convert_Units (total.max_distance, units) %
		Convert_Units (Average (total.count, total.distance), units) %
		Convert_Units (Std_Dev (total.count, total.distance, total.distance2), units));

	exe->Print (1, String ("Travel Speed %s      %7.2lf  %7.2lf  %7.2lf  %7.2lf") % 
		((Metric_Flag ()) ? "(kph)" : "(mph)") %
		Convert_Units (total.min_speed, spd_units) % Convert_Units (total.max_speed, spd_units) %
		Convert_Units (Average (total.count, total.speed), spd_units) %
		Convert_Units (Std_Dev (total.count, total.speed, total.speed2), spd_units));

	exe->Print (1, String ("Number of %s     %7.2lf  %7.2lf  %7.2lf  %7.2lf") % 
		((veh_flag) ? "Turns    " : "Transfers") %
		(double) total.min_turns % (double) total.max_turns %
		Average (total.count, total.turns) % Std_Dev (total.count, total.turns, total.turns2));
}

//---------------------------------------------------------
//	Travel_Sum_Header
//---------------------------------------------------------

void Trip_Sum_Data::Travel_Sum_Header (void)
{
	if (veh_flag) {
		exe->Print (1, "Travel Summary Report");
	} else {
		exe->Print (1, "Transit Passenger Suummary Report");
	}
	exe->Print (1);
	exe->Print (1, "             ---- Total ----- --------- Average ----------- ----------- StdDev ----------");
	if (veh_flag) {
		exe->Print (1, "Period Trips Vehicle  Vehicle   Time  Length   Speed  Turns   Time  Length   Speed  Turns");
	} else {
		exe->Print (1, "Period Trips ----Passenger---   Time  Length   Speed  Xfers   Time  Length   Speed  Xfers");
	}
	if (Metric_Flag ()) {
		exe->Print (1, "              Hours    KMtrs (minutes)  (km)   (kph)   (#) (minutes)  (km)   (kph)   (#)");
	} else {
		exe->Print (1, "              Hours    Miles (minutes)(miles)  (mph)   (#) (minutes)(miles)  (mph)   (#)");
	}
	exe->Print (1);
}
	 
/*********************************************|***********************************************

	Travel Summary Report

	             ---- Total ----- --------- Average ----------- ----------- StdDev ----------
	Period Trips Vehicle  Vehicle   Time  Length   Speed  Turns   Time  Length   Speed  Turns
	              Hours    Miles (minutes)(miles)  (mph)   (#) (minutes)(miles)  (mph)   (#)

	 ddd ddddddd fffffff ffffffff ffff.ff ffff.ff fff.ff fff.ff ffff.ff ffff.ff fff.ff fff.ff

	Total Vehicle Trips = dddddd
	Total Vehicle Hours of Travel = fffffff.f hours
	Total Vehicle Miles of Travel = ffffffff.f miles;
	Total Number of Turns = ddddd

	                        Minimum  Maximum  Average   StdDev   
	Travel Time (minutes)    fff.ff   fff.ff   fff.ff   fff.ff
	Trip Length (miles)      fff.ff   fff.ff   fff.ff   fff.ff
	Travel Speed (mph)       fff.ff   fff.ff   fff.ff   fff.ff
	Number of Turns          fff.ff   fff.ff   fff.ff   fff.ff

**********************************************|***********************************************/ 

