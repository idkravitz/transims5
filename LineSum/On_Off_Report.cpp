//*********************************************************
//	On_Off_Report.cpp - Create a Boarding/Alighting Report
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	On_Off_Report
//---------------------------------------------------------

void LineSum::On_Off_Report (On_Off_Report_Data &report)
{
	int i, stop, mode, count, vol_on, vol_off;
	int stop_fld, mode_fld, name_fld, pk_on_fld, pk_off_fld, op_on_fld, op_off_fld, day_on_fld, day_off_fld;
	String name;

	Range_Array_Itr range_itr;
	On_Off_Map_Itr on_off_itr;
	On_Off_Data *access_ptr, stop_data, mode_data, total_data;
	Str_Map_Itr name_itr;

	report_code = report.details;

	stop_fld = mode_fld = name_fld = pk_on_fld = pk_off_fld = op_on_fld = op_off_fld = day_on_fld = day_off_fld = -1;

	if (report.file != 0) {
		stop_fld = report.file->Add_Field ("STOP", DB_INTEGER, 8);
		if (report_code > 0) {
			mode_fld = report.file->Add_Field ("MODE", DB_INTEGER, 4);
			if (report_code == 1) name_fld = report.file->Add_Field ("LINE", DB_STRING, 12);
		}
		pk_on_fld = report.file->Add_Field ("PK_ON", DB_INTEGER, 8);
		pk_off_fld = report.file->Add_Field ("PK_OFF", DB_INTEGER, 8);
		op_on_fld = report.file->Add_Field ("OP_ON", DB_INTEGER, 8);
		op_off_fld = report.file->Add_Field ("OP_OFF", DB_INTEGER, 8);
		day_on_fld = report.file->Add_Field ("DAY_ON", DB_INTEGER, 8);
		day_off_fld = report.file->Add_Field ("DAY_OFF", DB_INTEGER, 8);

		report.file->Write_Header ();
	}
	Show_Message (String ("Creating On-Off Report #%d -- Stops") % report.number);
	Set_Progress ();

	//---- report header ----

	New_Page ();
	Print (2, "Title: ") << report.title;
	Print (1, "Modes:");

	if (report.all_modes) {
		Print (0, " All");
	} else {
		for (i=0, range_itr = report.modes.begin (); range_itr != report.modes.end (); range_itr++, i++) {
			if (!(i % 10)) {
				if (i) {
					Print (1, "       ") << range_itr->Low ();
				} else {
					Print (0, " ") << range_itr->Low ();
				}
			} else {
				Print (0, ", ") << range_itr->Low ();
			}
			if (range_itr->High () > range_itr->Low ()) {
				Print (0, "..") << range_itr->High ();
			}
		}
	}
	Header_Number (ON_OFF_REPORT);
	On_Off_Header ();

	if (report.all_stops) {
		Range_Data range_data;
		range_data.Low (1);
		range_data.High (MAX_INTEGER);

		report.stops.push_back (range_data);
	}

	//---- process each range ----

	memset (&total_data, '\0', sizeof (total_data));

	for (range_itr = report.stops.begin (); range_itr != report.stops.end (); range_itr++) {
		Show_Progress ();

		stop = mode = count = 0;
		memset (&mode_data, '\0', sizeof (mode_data));
		memset (&stop_data, '\0', sizeof (stop_data));

		for (on_off_itr = on_off_map.begin (); ; on_off_itr++) {

			if (on_off_itr == on_off_map.end () || on_off_itr->first.stop != stop) {
				if (stop > 0) {
					if (report_code == 1) {
						if (count > 1) {
							vol_on = stop_data.pk_on + stop_data.op_on;
							vol_off = stop_data.pk_off + stop_data.op_off;

							Print (1, String ("        Total   %4d        %6d  %6d  %6d  %6d  %6d  %6d") % count % 
								stop_data.pk_on % stop_data.pk_off % stop_data.op_on % stop_data.op_off % vol_on % vol_off);
						}
						Print (1);
					} else if (report_code == 2) {
						if (!count) {
							vol_on = mode_data.pk_on + mode_data.op_on;
							vol_off = mode_data.pk_off + mode_data.op_off;

							Print (1, String ("%-5d    %3d   %6d  %6d  %6d  %6d  %6d  %6d") % stop % mode % 
								mode_data.pk_on % mode_data.pk_off % mode_data.op_on % mode_data.op_off % vol_on % vol_off);

							if (station_flag) {
								name_itr = stop_names.find (stop);

								if (name_itr != stop_names.end ()) {
									Print (0, "  ") << name_itr->second;
								}
							}
						} else if (mode > 0) {
							vol_on = mode_data.pk_on + mode_data.op_on;
							vol_off = mode_data.pk_off + mode_data.op_off;

							Print (1, String ("         %3d   %6d  %6d  %6d  %6d  %6d  %6d") % mode % 
								mode_data.pk_on % mode_data.pk_off % mode_data.op_on % mode_data.op_off % vol_on % vol_off);	

							vol_on = stop_data.pk_on + stop_data.op_on;
							vol_off = stop_data.pk_off + stop_data.op_off;

							Print (1, String ("        Total  %6d  %6d  %6d  %6d  %6d  %6d") % 
								stop_data.pk_on % stop_data.pk_off % stop_data.op_on % stop_data.op_off % vol_on % vol_off);
						}
						Print (1);
						if (report.file != 0) {
							vol_on = mode_data.pk_on + mode_data.op_on;
							vol_off = mode_data.pk_off + mode_data.op_off;

							report.file->Put_Field (stop_fld, stop);
							report.file->Put_Field (mode_fld, mode);
							report.file->Put_Field (pk_on_fld, mode_data.pk_on);
							report.file->Put_Field (pk_off_fld, mode_data.pk_off);
							report.file->Put_Field (op_on_fld, mode_data.op_on);
							report.file->Put_Field (op_off_fld, mode_data.op_off);
							report.file->Put_Field (day_on_fld, vol_on);
							report.file->Put_Field (day_off_fld, vol_off);

							report.file->Write ();
						}
					} else {
						vol_on = stop_data.pk_on + stop_data.op_on;
						vol_off = stop_data.pk_off + stop_data.op_off;

						Print (1, String ("%-6d  %6d  %6d  %6d  %6d  %6d  %6d") % stop %
							stop_data.pk_on % stop_data.pk_off % stop_data.op_on % stop_data.op_off % vol_on % vol_off);

						if (station_flag) {
							name_itr = stop_names.find (stop);

							if (name_itr != stop_names.end ()) {
								Print (0, "  ") << name_itr->second;
							}
						}
						if (report.file != 0) {
							report.file->Put_Field (stop_fld, stop);
							report.file->Put_Field (pk_on_fld, stop_data.pk_on);
							report.file->Put_Field (pk_off_fld, stop_data.pk_off);
							report.file->Put_Field (op_on_fld, stop_data.op_on);
							report.file->Put_Field (op_off_fld, stop_data.op_off);
							report.file->Put_Field (day_on_fld, vol_on);
							report.file->Put_Field (day_off_fld, vol_off);

							report.file->Write ();
						}
					}
				}
				if (on_off_itr == on_off_map.end ()) break;

				count = mode = 0;
				memset (&stop_data, '\0', sizeof (stop_data));

				stop = on_off_itr->first.stop;
				if (stop < range_itr->Low () || stop > range_itr->High ()) {
					stop = 0;
					continue;
				}
			}
			if (mode != on_off_itr->first.mode) {
				if (mode > 0 && report_code == 2) {
					if (!count) {
						vol_on = mode_data.pk_on + mode_data.op_on;
						vol_off = mode_data.pk_off + mode_data.op_off;

						Print (1, String ("%-5d    %3d   %6d  %6d  %6d  %6d  %6d  %6d") % stop % mode % 
							mode_data.pk_on % mode_data.pk_off % mode_data.op_on % mode_data.op_off % vol_on % vol_off);	

						if (station_flag) {
							name_itr = stop_names.find (stop);

							if (name_itr != stop_names.end ()) {
								Print (0, "  ") << name_itr->second;
							}
						}
					} else {
						vol_on = mode_data.pk_on + mode_data.op_on;
						vol_off = mode_data.pk_off + mode_data.op_off;

						Print (1, String ("         %3d   %6d  %6d  %6d  %6d  %6d  %6d") % mode % 
							mode_data.pk_on % mode_data.pk_off % mode_data.op_on % mode_data.op_off % vol_on % vol_off);
					}
					if (report.file != 0) {
						vol_on = mode_data.pk_on + mode_data.op_on;
						vol_off = mode_data.pk_off + mode_data.op_off;

						report.file->Put_Field (stop_fld, stop);
						report.file->Put_Field (mode_fld, mode);
						report.file->Put_Field (pk_on_fld, mode_data.pk_on);
						report.file->Put_Field (pk_off_fld, mode_data.pk_off);
						report.file->Put_Field (op_on_fld, mode_data.op_on);
						report.file->Put_Field (op_off_fld, mode_data.op_off);
						report.file->Put_Field (day_on_fld, vol_on);
						report.file->Put_Field (day_off_fld, vol_off);

						report.file->Write ();
					}
					count++;
				}
				mode = on_off_itr->first.mode;
				memset (&mode_data, '\0', sizeof (mode_data));
			}
			if (!report.all_modes && !report.modes.In_Range (mode)) {
				mode = 0;
				continue;
			}
			name = on_off_itr->first.name;
			access_ptr = &on_off_itr->second;

			vol_on = access_ptr->pk_on + access_ptr->op_on;
			vol_off = access_ptr->pk_off + access_ptr->op_off;

			if (report_code == 1) {
				if (!count) {
					Print (1, String ("%-5d    %3d   %-12.12s %6d  %6d  %6d  %6d  %6d  %6d") % stop % mode % name %
						access_ptr->pk_on % access_ptr->pk_off % access_ptr->op_on % access_ptr->op_off % vol_on % vol_off);

					if (station_flag) {
						name_itr = stop_names.find (stop);

						if (name_itr != stop_names.end ()) {
							Print (0, "  ") << name_itr->second;
						}
					}
				} else {
					Print (1, String ("         %3d   %-12.12s %6d  %6d  %6d  %6d  %6d  %6d") % mode % name %
						access_ptr->pk_on % access_ptr->pk_off % access_ptr->op_on % access_ptr->op_off % vol_on % vol_off);
				}
				if (report.file != 0) {
					report.file->Put_Field (stop_fld, stop);
					report.file->Put_Field (mode_fld, mode);
					report.file->Put_Field (name_fld, name);
					report.file->Put_Field (pk_on_fld, access_ptr->pk_on);
					report.file->Put_Field (pk_off_fld, access_ptr->pk_off);
					report.file->Put_Field (op_on_fld, access_ptr->op_on);
					report.file->Put_Field (op_off_fld, access_ptr->op_off);
					report.file->Put_Field (day_on_fld, vol_on);
					report.file->Put_Field (day_off_fld, vol_off);

					report.file->Write ();
				}
				count++;
			}
			mode_data.pk_on += access_ptr->pk_on;
			mode_data.pk_off += access_ptr->pk_off;
			mode_data.op_on += access_ptr->op_on;
			mode_data.op_off += access_ptr->op_off;

			stop_data.pk_on += access_ptr->pk_on;
			stop_data.pk_off += access_ptr->pk_off;
			stop_data.op_on += access_ptr->op_on;
			stop_data.op_off += access_ptr->op_off;

			total_data.pk_on += access_ptr->pk_on;
			total_data.pk_off += access_ptr->pk_off;
			total_data.op_on += access_ptr->op_on;
			total_data.op_off += access_ptr->op_off;
		}
	}
	End_Progress ();

	vol_on = total_data.pk_on + total_data.op_on;
	vol_off = total_data.pk_off + total_data.op_off;

	if (report_code == 1) {
		Print (1, String ("Total                      %7d %7d %7d %7d %7d %7d") % 
			total_data.pk_on % total_data.pk_off % total_data.op_on % total_data.op_off % vol_on % vol_off);
	} else if (report_code == 2) {
		Print (1, String ("Total         %7d %7d %7d %7d %7d %7d") % 
			total_data.pk_on % total_data.pk_off % total_data.op_on % total_data.op_off % vol_on % vol_off);
	} else {
		Print (2, String ("Total  %7d %7d %7d %7d %7d %7d") % 
			total_data.pk_on % total_data.pk_off % total_data.op_on % total_data.op_off % vol_on % vol_off);
	}
	if (report.file != 0) {
		report.file->Close ();
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	On_Off_Header
//---------------------------------------------------------

void LineSum::On_Off_Header (void)
{
	if (report_code == 0) {
		Print (2, "        ---- Peak ----  -- Offpeak ---  ---- Daily ---");
		Print (1, "Stop     Board  Alight   Board  Alight   Board  Alight");
	} else if (report_code == 1) {
		Print (2, "                            ---- Peak ----  -- Offpeak ---  ---- Daily ---");
		Print (1, "Stop     Mode   Line         Board  Alight   Board  Alight   Board  Alight");
	} else {
		Print (2, "              ---- Peak ----  -- Offpeak ---  ---- Daily ---");
		Print (1, "Stop     Mode  Board  Alight   Board  Alight   Board  Alight");
	}
	Print (1);
}

/*** ON_OFF Report ****************************************************************|**********************************************************************************

if (detail == 0)

	Title: ssssssssssssssssssssssssssssssssssssss
	Modes: dd-dd, dd, ...

	        ---- Peak ----  -- Offpeak ---  ---- Daily ---
	Stop     Board  Alight   Board  Alight   Board  Alight

	dddddd  dddddd  dddddd  dddddd  dddddd  dddddd  dddddd   ssssssssssssssss
	dddddd  dddddd  dddddd  dddddd  dddddd  dddddd  dddddd
	 ....
	Total   dddddd  dddddd  dddddd  dddddd  dddddd  dddddd

if (detail == 1)

	Title: sssssssssssssssssssssssssssssssssssssss
	Modes: dd-dd, dd, ...

	                           ---- Peak ----  -- Offpeak ---  ---- Daily ---
	Stop     Mode   Line        Board  Alight   Board  Alight   Board  Alight

	ddddd    ddd  ssssssssssss dddddd  dddddd  dddddd  dddddd  dddddd  dddddd   sssssssssssssssss
	         ddd  ssssssssssss dddddd  dddddd  dddddd  dddddd  dddddd  dddddd  
	         ddd  ssssssssssss dddddd  dddddd  dddddd  dddddd  dddddd  dddddd
				...
	        Total   dddd       dddddd  dddddd  dddddd  dddddd  dddddd  dddddd

	ddddd    ddd  ssssssssssss dddddd  dddddd  dddddd  dddddd  dddddd  dddddd   sssssssssssssssss
	         ddd  ssssssssssss dddddd  dddddd  dddddd  dddddd  dddddd  dddddd  
	         ddd  ssssssssssss dddddd  dddddd  dddddd  dddddd  dddddd  dddddd
				...
	        Total   dddd       dddddd  dddddd  dddddd  dddddd  dddddd  dddddd

	Total                      dddddd  dddddd  dddddd  dddddd  dddddd  dddddd

if (detail == 2)

	Title: ssssssssssssssssssssssssssssssssssssss
	Modes: dd-dd, dd, ...
	              ---- Peak ----  -- Offpeak ---  ---- Daily ---
	Stop     Mode  Board  Alight   Board  Alight   Board  Alight

	ddddd    ddd  dddddd  dddddd  dddddd  dddddd  dddddd  dddddd   ssssssssssss
	         ddd  dddddd  dddddd  dddddd  dddddd  dddddd  dddddd
	         ddd  dddddd  dddddd  dddddd  dddddd  dddddd  dddddd
				  ...
	        Total dddddd  dddddd  dddddd  dddddd  dddddd  dddddd

	ddddd    ddd  dddddd  dddddd  dddddd  dddddd  dddddd  dddddd
	         ddd  dddddd  dddddd  dddddd  dddddd  dddddd  dddddd
				  ...
	        Total dddddd  dddddd  dddddd  dddddd  dddddd  dddddd

	Total         dddddd  dddddd  dddddd  dddddd  dddddd  dddddd   
*********************************************************************************|**********************************************************************************/
