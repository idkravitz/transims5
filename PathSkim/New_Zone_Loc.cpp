//*********************************************************
//	New_Zone_Loc.cpp - write the new zone location files
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	New_Zone_Locations
//---------------------------------------------------------

void PathSkim::New_Zone_Locations (void)
{
	bool first;

	Ints_Map_Itr map_itr;
	Integers *int_ptr;
	Int_Itr int_itr;

	//---- origin zone locations ----

	if (org_loc_flag) {
		fstream &file = org_loc_file.File ();

		if (zone_skim_flag) {
			file << "ZONE\tLOCATIONS" << endl;
		} else {
			file << "DISTRICT\tLOCATIONS" << endl;
		}
		for (map_itr = org_zone_loc.begin (); map_itr != org_zone_loc.end (); map_itr++) {
			int_ptr = &(map_itr->second);
			if (int_ptr->size () == 0) continue;

			file << map_itr->first << "\t";
			first = true;

			for (int_itr = int_ptr->begin (); int_itr != int_ptr->end (); int_itr++) {
				if (first) {
					first = false;
				} else {
					file << ", ";
				}
				file << *int_itr;
			}
			file << endl;
		}
		org_loc_file.Close ();
	}

	//---- destination zone locations ----

	if (des_loc_flag) {
		fstream &file = des_loc_file.File ();

		if (zone_skim_flag) {
			file << "ZONE\tLOCATIONS" << endl;
		} else {
			file << "DISTRICT\tLOCATIONS" << endl;
		}
		for (map_itr = des_zone_loc.begin (); map_itr != des_zone_loc.end (); map_itr++) {
			int_ptr = &(map_itr->second);
			if (int_ptr->size () == 0) continue;

			file << map_itr->first << "\t";
			first = true;

			for (int_itr = int_ptr->begin (); int_itr != int_ptr->end (); int_itr++) {
				if (first) {
					first = false;
				} else {
					file << ", ";
				}
				file << *int_itr;
			}
			file << endl;
		}
		des_loc_file.Close ();
	}
}
