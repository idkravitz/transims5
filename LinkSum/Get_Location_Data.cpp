//*********************************************************
//	Get_Location_Data.cpp - Read the Activity Location File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Initialize_Locations
//---------------------------------------------------------

void LinkSum::Initialize_Locations (Location_File &file)
{
	Required_File_Check (file, LINK);

	//---- initialize the link activity data ----

	if (activity_flag) {
		int i;
		Int_Itr itr;
		Int_Map_Itr map_itr;

		//---- initialize the fields ----

		for (i=1, itr = field_list.begin (); itr != field_list.end (); itr++, i++) {
			link_db.Put_Field (i, 0);
		}

		//---- set the record key in sorted order ----

		for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
			link_db.Put_Field (0, map_itr->first);

			if (!link_db.Write_Record (map_itr->first)) {
				Error ("Adding Link Database");
			}
		}
	}
}

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool LinkSum::Get_Location_Data (Location_File &file, Location_Data &data)
{
	int i, field, link, lvalue1, lvalue2, zone;
	double dvalue1, dvalue2;
	String buffer;

	Db_Field *fld;
	Link_Location loc_rec;
	Int_Itr int_itr;
	Int_Map_Itr map_itr;

	//---- get the link number ----

	link = file.Link ();
	if (link == 0) return (false);
	data.Link (link);

	//---- sum the activities on links ----

	if (activity_flag) {
		if (!link_db.Read_Record (link)) {
			Error (String ("Reading Link Database Record %d") % link);
		}

		//---- sum the data fields ----

		for (i=1, int_itr = field_list.begin (); int_itr != field_list.end (); int_itr++, i++) {
			field = *int_itr;

			fld = file.Field (field);

			if (fld->Type () == DB_INTEGER) {
				lvalue1 = file.Get_Integer (field);
				lvalue2 = link_db.Get_Integer (i);

				lvalue2 += lvalue1;
				link_db.Put_Field (i, lvalue2);
			} else if (fld->Type () == DB_DOUBLE) {
				dvalue1 = file.Get_Double (field);
				dvalue2 = link_db.Get_Double (i);

				dvalue2 += dvalue1;
				link_db.Put_Field (i, dvalue2);
			} else if (fld->Type () == DB_STRING) {
				link_db.Put_Field (i, file.Get_String (field));
			}
		}

		//---- save the changes ----

		if (!link_db.Write_Record (link)) {
			Error (String ("Writing Link Database Record %d") % link);
		}
	}

	//---- add a link location record ----

	if (zone_flag || group_flag) {
		zone = file.Zone ();

		if (zone > 0) {
			map_itr = link_map.find (link);
			if (map_itr == link_map.end ()) return (false);
			link = map_itr->second;

			loc_rec.link = link;
			loc_rec.location = i = file.Location ();
			loc_rec.zone = zone;
			loc_rec.count = 0;

			link_location.insert (Link_Loc_Map_Data (Int2_Key (link, i), loc_rec));
		}
	}

	//---- don't save the location data ----

	return (false);
}
