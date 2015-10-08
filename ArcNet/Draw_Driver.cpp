//*********************************************************
//	Draw_Driver - draw a transit driver file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Draw_Driver
//---------------------------------------------------------

void ArcNet::Draw_Driver (void)
{
	int j, dir, overlap;
	int route_field, links_field, type_field, notes_field;
	double offset, length, side, side_offset;
	bool offset_flag, first;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Link_Itr link_itr;
	Line_Itr line_itr;
	Points_Itr pt_itr;
	Driver_Itr driver_itr;
	Db_Base *file;
	
	file = System_File_Base (TRANSIT_DRIVER);

	route_field = file->Required_Field ("ROUTE");
	links_field = file->Required_Field ("LINKS", "NLINKS", "NUM_LINKS");
	type_field = file->Optional_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	notes_field = file->Optional_Field ("NOTES");


	Show_Message ("Draw Transit Driver -- Record");
	Set_Progress ();

	side = 0;
	offset_flag = (route_offset != 0.0);

	//---- set the overlap count ----

	if (!overlap_flag) {
		for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
			link_itr->Aoffset (0);
			link_itr->Boffset (0);
		}
	}

	//---- process each transit route ----

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		Show_Progress ();

		arcview_driver.Put_Field (route_field, line_itr->Route ());
		arcview_driver.Put_Field (links_field, (int) line_itr->driver_array.size ());
		arcview_driver.Put_Field (type_field, line_itr->Type ());
		arcview_driver.Put_Field (notes_field, line_itr->Notes ());

		//---- create the route shape ----

		arcview_driver.clear ();
		first = true;

		for (driver_itr = line_itr->driver_array.begin (); driver_itr != line_itr->driver_array.end (); driver_itr++) {

			dir_ptr = &dir_array [*driver_itr];
			dir = dir_ptr->Dir ();

			link_ptr = &link_array [dir_ptr->Link ()];

			offset = length = UnRound (link_ptr->Length ());
			side_offset = route_offset;

			if (side_offset > length / 3.0) {
				side_offset = length / 3.0;
			}
			if (offset_flag) {
				if (!overlap_flag) {
					if (dir == 0) {
						overlap = link_ptr->Aoffset () + 1;
						link_ptr->Aoffset (overlap);
					} else {
						overlap = link_ptr->Boffset () + 1;
						link_ptr->Boffset (overlap);
					}
					side = side_offset = route_offset * overlap;

					if (side_offset > length / 3.0) {
						side_offset = length / 3.0;
					}
					offset -= side_offset;
					length -= 2.0 * side_offset;
				} else {
					offset -= side_offset;
					length -= 2.0 * side_offset;

					if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
						side = side_offset;
					} else {
						side = 0.0;
					}
				}
				j = 1;
			} else if (first) {
				j = 1;
				first = false;
			} else {
				j = 2;
			}
			Link_Shape (link_ptr, dir, points, side_offset, length, side);

			pt_itr = points.begin ();
			if (j > 1 && pt_itr != points.end ()) pt_itr++;

			for (; pt_itr != points.end (); pt_itr++) {
				arcview_driver.push_back (*pt_itr);
			}
		}
		if (!arcview_driver.Write_Record ()) {
			Error (String ("Writing %s") % arcview_driver.File_Type ());
		}
	}
	End_Progress ();

	arcview_driver.Close ();
}
