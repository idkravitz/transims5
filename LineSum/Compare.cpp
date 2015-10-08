//*********************************************************
//	Compare.cpp - compare route files
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Compare
//---------------------------------------------------------

void LineSum::Compare (void)
{
	int mode;
	Dtime time;
	Db_Header *fh;
	File_Itr file_itr;
	Sort_Key sort_key;

	Route_Data route_rec, *base_ptr, *alt_ptr;
	Route_Map_Itr route_itr, base_itr;
	Route_Map_Stat route_stat;
	Route_Map base_map, alt_map;

	//---- compare files ----

	Show_Message ("Reading Base Route Files -- Record");
	Set_Progress ();

	for (mode=1, file_itr = base_routes.begin (); file_itr != base_routes.end (); file_itr++, mode++) {
		fh = *file_itr;

		while (fh->Read ()) {
			Show_Progress ();

			sort_key.mode = (short) mode;	//(short) fh->Get_Integer ("MODE");
			sort_key.leg = (short) 1;
			sort_key.name = fh->Get_String ("NAME");

			time = fh->Get_Time ("HEADWAY1");
			route_rec.headway1 = (int) time.Minutes ();

			time = fh->Get_Time ("HEADWAY2");
			route_rec.headway2 = (int) time.Minutes ();

			time = fh->Get_Time ("TTIME1");
			route_rec.ttime1 = (int) time.Minutes ();

			time = fh->Get_Time ("TTIME2");
			route_rec.ttime2 = (int) time.Minutes ();

			route_rec.nodes = fh->Get_Integer ("NODES");
			route_rec.notes = fh->Get_String ("NOTES");
			route_rec.match = 0;

			route_stat = base_map.insert (Route_Map_Data (sort_key, route_rec));
			if (!route_stat.second) {
				sort_key.leg = (short) 2;
				route_stat = base_map.insert (Route_Map_Data (sort_key, route_rec));
			}
		}
	}
	End_Progress ();

	//---- compare files ----

	Show_Message ("Reading Alternative Route Files -- Record");
	Set_Progress ();
	
	New_Page ();
	Header_Number (DIFFERENCE_REPORT);
	Compare_Header ();

	for (mode=1, file_itr = alt_routes.begin (); file_itr != alt_routes.end (); file_itr++, mode++) {
		fh = *file_itr;

		while (fh->Read ()) {
			Show_Progress ();

			sort_key.mode = (short) mode;	//(short) fh->Get_Integer ("MODE");
			sort_key.leg = (short) 1;
			sort_key.name = fh->Get_String ("NAME");

			time = fh->Get_Time ("HEADWAY1");
			route_rec.headway1 = (int) time.Minutes ();

			time = fh->Get_Time ("HEADWAY2");
			route_rec.headway2 = (int) time.Minutes ();

			time = fh->Get_Time ("TTIME1");
			route_rec.ttime1 = (int) time.Minutes ();

			time = fh->Get_Time ("TTIME2");
			route_rec.ttime2 = (int) time.Minutes ();

			route_rec.nodes = fh->Get_Integer ("NODES");
			route_rec.notes = fh->Get_String ("NOTES");

			route_rec.match = 0;

			route_stat = alt_map.insert (Route_Map_Data (sort_key, route_rec));
			if (!route_stat.second) {
				sort_key.leg = (short) 2;
				route_stat = alt_map.insert (Route_Map_Data (sort_key, route_rec));
			}
		}
	}
	End_Progress ();

	for (route_itr = alt_map.begin (); route_itr != alt_map.end (); route_itr++) {
		sort_key = route_itr->first;
		alt_ptr = &route_itr->second;

		base_itr = base_map.find (route_itr->first);

		if (base_itr == base_map.end ()) {
			Print (1, String ("%3d   %-12.12s") % sort_key.mode % sort_key.name);
			Print (0, String ("  %6.1lf   %6.1lf   %6.1lf   %6.1lf  %5d    %6.1lf   %6.1lf   %6.1lf   %6.1lf  %5d  ") %
				0 % 0 % 0 % 0 % 0 %
				alt_ptr->headway1 % alt_ptr->ttime1 % alt_ptr->headway2 % alt_ptr->ttime2 % alt_ptr->nodes);
			Print (0, alt_ptr->notes);
		} else {
			base_ptr = &base_itr->second;
			base_ptr->match = 1;

			if (base_ptr->headway1 != alt_ptr->headway1 || base_ptr->headway2 != alt_ptr->headway2 ||
				base_ptr->ttime1 != alt_ptr->ttime1 || base_ptr->ttime2 != alt_ptr->ttime2 ||
				base_ptr->nodes != alt_ptr->nodes) {

				Print (1, String ("%3d   %-12.12s") % sort_key.mode % sort_key.name);
				Print (0, String ("  %6.1lf   %6.1lf   %6.1lf   %6.1lf  %5d    %6.1lf   %6.1lf   %6.1lf   %6.1lf  %5d  ") %
					base_ptr->headway1 % base_ptr->ttime1 % base_ptr->headway2 % base_ptr->ttime2 % base_ptr->nodes %
					alt_ptr->headway1 % alt_ptr->ttime1 % alt_ptr->headway2 % alt_ptr->ttime2 % alt_ptr->nodes);
				Print (0, base_ptr->notes);
			}
		}
	}

	for (base_itr = base_map.begin (); base_itr != base_map.end (); base_itr++) {
		if (base_itr->second.match == 0) {
			sort_key = base_itr->first;
			base_ptr = &base_itr->second;

			Print (1, String ("%3d   %-12.12s") % sort_key.mode % sort_key.name);
			Print (0, String ("  %6.1lf   %6.1lf   %6.1lf   %6.1lf  %5d    %6.1lf   %6.1lf   %6.1lf   %6.1lf  %5d  ") %
				base_ptr->headway1 % base_ptr->ttime1 % base_ptr->headway2 % base_ptr->ttime2 % base_ptr->nodes %
				0 % 0 % 0 % 0 % 0);
			Print (0, base_ptr->notes);
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Compare_Header
//---------------------------------------------------------

void LineSum::Compare_Header (void)
{
	Print (2, "Difference Report  ------------------- Base ----------------   ------------ Alternative ----------------");
	Print (1, "                   ----- Peak -----  ----Offpeak-----   Num    ----- Peak -----  ----Offpeak-----   Num ");
	Print (1, "Mode  Line         Headway  RunTime  Headway  RunTime  Nodes   Headway  RunTime  Headway  RunTime  Nodes");
	Print (1);
}

/*** Compare Report ****************************************************************|**********************************************************************************

	                   ------------------- Base ----------------   ------------ Alternative ----------------  
                       ----- Peak -----  ----Offpeak-----   Num    ----- Peak -----  ----Offpeak-----   Num 
	Mode  Line         Headway  RunTime  Headway  RunTime  Nodes   Headway  RunTime  Headway  RunTime  Nodes

	ddd   sssss12sssss  ffff.f   ffff.f   ffff.f   ffff.f  ddddd    ffff.f   ffff.f   ffff.f   ffff.f  ddddd 
    
*********************************************************************************|**********************************************************************************/