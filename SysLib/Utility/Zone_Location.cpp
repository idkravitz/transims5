//*********************************************************
//	Zone_Location_Map.cpp - zone location map class
//*********************************************************

#include "Zone_Location.hpp"

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Zone_Location_Map constructor
//---------------------------------------------------------

Zone_Location_Map::Zone_Location_Map (void) : Db_File ()
{
	File_Type ("Zone Location Map File");
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Zone_Location_Map::Read (void)
{
	int num, zone, n1, n2, count;
	double low, high;
	bool zone_flag, loc_flag;

	String text;
	Strings ranges;
	Str_Itr range_itr;
	Int_Map_Itr map_itr;

	Ints_Map_Stat zone_loc_stat;
	Integers empty, *loc_ptr;

	Data_Service *data = (Data_Service *) exe;
	zone_flag = data->System_Data_Flag (ZONE);
	loc_flag = data->System_Data_Flag (LOCATION);

	loc_ptr = 0;
	count = 0;

	while (Db_File::Read ()) {

		text = Record_String ();
		if (text.empty ()) continue;

		text.Parse (ranges);

		for (range_itr = ranges.begin (); range_itr != ranges.end (); range_itr++) {

			//---- create the zone index ----

			if (range_itr == ranges.begin ()) {
				zone = range_itr->Integer ();
				if (zone == 0) break;

				if (zone_flag) {
					map_itr = data->zone_map.find (zone);
					if (map_itr == data->zone_map.end ()) break;
					zone = map_itr->second;
				}
				zone_loc_stat = insert (Ints_Map_Data (zone, empty));

				loc_ptr = &zone_loc_stat.first->second;
				continue;
			}

			//---- parse location records ----

			if (!range_itr->Range (low, high)) continue;
			n1 = (int) low;
			n2 = (int) high;

			if (n1 == 0 && n2 == 0) continue;

			if (n1 > n2) {
				if (exe->Send_Messages ()) {
					exe->Write (1, String ("Location Range %d..%d is Illogical") % n1 % n2); 
				}
				return (false);
			}
			for (num = n1; num <= n2; num++) {
				if (loc_flag) {
					map_itr = data->location_map.find (num);
					if (map_itr == data->location_map.end ()) continue;
					loc_ptr->push_back (map_itr->second);
					count++;
				} else {
					loc_ptr->push_back (num);
					count++;
				}
			}
		}
	}
	exe->Print (2, String ("%s contains %d Zones and %d Locations") % File_Type () % Num_Zones () % count);
	return (true);
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool Zone_Location_Map::Write (void)
{
	int n1, n2, nzone, count, zone, loc;
	bool first, zone_flag, loc_flag;

	fstream &file = File ();

	Ints_Map_Itr zone_loc_itr;
	Integers *loc_ptr;
	Int_Itr itr;

	Data_Service *data = (Data_Service *) exe;
	zone_flag = data->System_Data_Flag (ZONE);
	loc_flag = data->System_Data_Flag (LOCATION);

	count = nzone = 0;

	file << "ZONE\tLOCATIONS" << endl;

	for (zone_loc_itr = begin (); zone_loc_itr != end (); zone_loc_itr++) {
		loc_ptr = &zone_loc_itr->second;

		if (loc_ptr->size () == 0) continue;

		zone = zone_loc_itr->first;
		if (zone_flag) {
			zone = data->zone_array [zone].Zone ();
		}
		file << zone << "\t";

		n1 = n2 = 0;
		first = true;

		for (itr = loc_ptr->begin (); itr != loc_ptr->end (); itr++) {
			loc = *itr;

			if (loc_flag) {
				loc = data->location_array [loc].Location ();
			}
			if (*itr != n2 + 1) {
				if (n1 > 0) {
					if (first) {
						first = false;
					} else {
						file << ", ";
					}
					if (n2 > n1) {
						file << n1 << ".." << n2;
					} else {
						file << n2;
					}
				}
				n1 = n2 = 0;
			}
			n2 = *itr;
			if (n1 == 0) n1 = n2;
		}
		if (n1 > 0) {
			if (!first) {
				file << ", ";
			}
			if (n2 > n1) {
				file << n1 << ".." << n2;
			} else {
				file << n2;
			}
		}
		file << endl;
		nzone++;
		count += (int) loc_ptr->size ();
	}
	exe->Print (2, String ("%s contains %d Zones and %d Locations") % File_Type () % nzone % count);
	Close ();
	return (true);
}

//---------------------------------------------------------
//	Locations
//---------------------------------------------------------

Integers * Zone_Location_Map::Locations (int zone)
{
	Ints_Map_Itr itr;

	itr = find (zone);

	if (itr == end ()) return (0);

	return (&itr->second);
}
