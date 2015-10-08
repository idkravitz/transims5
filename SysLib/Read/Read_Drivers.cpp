//*********************************************************
//	Read_Drivers.cpp - read the transit driver file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Drivers
//---------------------------------------------------------

void Data_Service::Read_Drivers (void)
{
	Driver_File *file = (Driver_File *) System_File_Handle (TRANSIT_DRIVER);

	int i, num, count;
	bool keep_flag;
	Line_Data *line_ptr;
	Driver_Data driver_rec;

	//---- store the transit driver data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Drivers (*file);
	count = 0;

	while (file->Read (false)) {
		Show_Progress ();

		driver_rec.Clear ();

		keep_flag = Get_Driver_Data (*file, driver_rec);

		num = file->Num_Nest ();
		if (num > 0) driver_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error (String ("Number of Link Records for Route %d") % file->Route ());
			}
			Show_Progress ();

			Get_Driver_Data (*file, driver_rec);
		}
		if (keep_flag) {
			line_ptr = &line_array [driver_rec.Route ()];

			if (driver_rec.Type () > 0 && line_ptr->Type () == 0) line_ptr->Type (driver_rec.Type ());

			line_ptr->driver_array.swap (driver_rec);
			count += (int) driver_rec.size ();
		}
	}
	End_Progress ();
	file->Close ();

	line_array.Driver_Records (Progress_Count ());

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % count);
	}
	if (count > 0) System_Data_True (TRANSIT_DRIVER);

	//---- check for driver path for each route ----
		
	if ((int) line_array.size () > 0 && (int) stop_array.size () > 0) {
		int length, offset;
		bool first;

		Line_Itr line_itr;
		Line_Stop_Itr stop_itr;
		Driver_Itr driver_itr;
		Stop_Data *stop_ptr;
		Dir_Data *dir_ptr;
		Link_Data *link_ptr;

		for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
			length = offset = 0;
			first = true;

			driver_itr = line_itr->driver_array.begin ();
			if (driver_itr == line_itr->driver_array.end ()) continue;

			for (stop_itr = line_itr->begin(); stop_itr != line_itr->end (); stop_itr++) {
				stop_ptr = &stop_array [stop_itr->Stop ()];

				for (; driver_itr != line_itr->driver_array.end (); driver_itr++) {
					dir_ptr = &dir_array [*driver_itr];

					if (stop_ptr->Link_Dir () == dir_ptr->Link_Dir ()) break;

					link_ptr = &link_array [dir_ptr->Link ()];
					length += link_ptr->Length () - offset;
					offset = 0;
				}
				if (driver_itr == line_itr->driver_array.end ()) {
					Error (String ("Route %d Stops and Driver Links are Incompatible") % line_itr->Route ());
				}
				if (first) {
					first = false;
					length = 0;
				} else {
					length += stop_ptr->Offset () - offset;
					stop_itr->Length (length);
				}
				offset = stop_ptr->Offset ();
			}
		}
	}
}

//---------------------------------------------------------
//	Initialize_Drivers
//---------------------------------------------------------

void Data_Service::Initialize_Drivers (Driver_File &file)
{
	Required_File_Check (file, TRANSIT_ROUTE);
	Required_File_Check (file, LINK);
	Required_File_Check (file, VEHICLE_TYPE);
}

//---------------------------------------------------------
//	Get_Driver_Data
//---------------------------------------------------------

bool Data_Service::Get_Driver_Data (Driver_File &file, Driver_Data &driver_rec)
{
	int route;

	//---- process a header line ----

	if (!file.Nested ()) {
		int type;
		Int_Map_Itr map_itr;

		route = file.Route ();
		if (route == 0) return (false);
		if (file.Links () < 1) return (false);

		map_itr = line_map.find (route);
		if (map_itr == line_map.end ()) {
			Warning (String ("Transit Route %d was Not Found") % route);
			return (false);
		}
		driver_rec.Route (map_itr->second);

		type = file.Type ();

		driver_rec.Type (type);

		if (file.SubType_Flag () && file.Version () <= 40) {
			driver_rec.Type (VehType40_Map (type, file.SubType ()));
		}
		if (driver_rec.Type () > 0) {
			map_itr = veh_type_map.find (driver_rec.Type ());
			if (map_itr == veh_type_map.end ()) {
				Warning (String ("Driver Route %d Vehicle Type %d was Not Found") % route % driver_rec.Type ());
				return (false);
			}
			driver_rec.Type (map_itr->second);
		}
		return (true);
	}

	//---- process a link record ----

	int link, dir, dir_index;
	Link_Data *link_ptr;

	link = file.Link ();
	dir = file.Dir ();

	link_ptr = Set_Link_Direction (file, link, dir);

	if (link_ptr == 0) return (false);

	if (dir) {
		dir_index = link_ptr->BA_Dir ();
	} else {
		dir_index = link_ptr->AB_Dir ();
	}
	if (dir_index < 0) {
		Line_Data *line_ptr = &line_array [driver_rec.Route ()];

		route = line_ptr->Route ();
		link = file.Link ();

		Warning (String ("Route %d Link %d Direction %s was Not Found") % route % link % ((dir) ? "BA" : "AB"));
		return (false);
	}
	driver_rec.push_back (dir_index);
	return (true);
}
