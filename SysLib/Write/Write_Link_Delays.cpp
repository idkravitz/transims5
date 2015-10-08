//*********************************************************
//	Write_Link_Delays.cpp - write a new link delay file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Link_Delays
//---------------------------------------------------------

void Data_Service::Write_Link_Delays (bool fill_flag)
{
	int i, index, dir, count, num_periods;
	double length, speed;
	Dtime low, high, time;
	bool turn_flag;

	Int_Map_Itr itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Connect_Data *connect_ptr;
	Flow_Time_Data *flow_time_ptr;
	Flow_Time_Array *link_delay_ptr, *turn_delay_ptr;
	Link_Delay_Data link_delay;
	Turn_Delay_Data turn_delay;

	Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (NEW_LINK_DELAY);

	Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	Set_Progress ();

	count = 0;

	num_periods = time_periods.Num_Periods ();

	turn_flag = (System_File_Flag (CONNECTION) && file->Turn_Flag () && (turn_delay_array.size () > 0));
	turn_delay_ptr = 0;

	//---- process each time period ----

	for (i=0; i < num_periods; i++) {
		time_periods.Period_Range (i, low, high);

		link_delay.Start (low);
		link_delay.End (high);

		link_delay_ptr = &link_delay_array [i];

		if (turn_flag) {
			turn_delay_ptr = &turn_delay_array [i];
		}

		//---- sort the links ----

		for (itr = link_map.begin (); itr != link_map.end (); itr++) {
			link_ptr = &link_array [itr->second];
			
			length = link_ptr->Length ();

			for (dir=0; dir < 2; dir++) {
				index = (dir) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
				if (index < 0) continue;

				Show_Progress ();

				flow_time_ptr = &link_delay_ptr->at (index);
				if (!fill_flag && flow_time_ptr->Flow () == 0) continue;
				
				dir_ptr = &dir_array [index];

				link_delay.Dir_Index (index);
				link_delay.Type (0);

				link_delay.Flow (flow_time_ptr->Flow ());

				if (flow_time_ptr->Time () > 0) {
					speed = length / flow_time_ptr->Time ();
				} else {
					speed = length / dir_ptr->Time0 ();
				}
				if (speed < 0.1) speed = 0.1;

				time = (int) (length / speed + 0.5);
				if (time < 1) time = 1;

				link_delay.Time (time);

				//---- gather the turn delays ----

				if (turn_flag) {
					link_delay.clear ();

					for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
						connect_ptr = &connect_array [index];

						flow_time_ptr = &turn_delay_ptr->at (index);
						if (!fill_flag && flow_time_ptr->Flow () == 0) continue;

						turn_delay.To_Index (connect_ptr->To_Index ());
						turn_delay.Flow (flow_time_ptr->Flow ());

						time = link_delay.Time () + flow_time_ptr->Time ();

						if (time > 0) {
							speed = length / time;
						} else {
							speed = length / dir_ptr->Time0 ();
						}
						if (speed < 0.1) speed = 0.1;

						time = (int) (length / speed + 0.5);
						if (time < 1) time = 1;

						turn_delay.Time (time);

						link_delay.push_back (turn_delay);
					}
				}
				count += Put_Link_Delay_Data (*file, link_delay);

				//---- lane use flows ----

				if (dir_ptr->Flow_Index () >= 0) {
					flow_time_ptr = &link_delay_ptr->at (dir_ptr->Flow_Index ());
					if (!fill_flag && flow_time_ptr->Flow () == 0) continue;

					link_delay.Type (1);
					link_delay.Flow (flow_time_ptr->Flow ());

					if (flow_time_ptr->Time () > 0) {
						speed = length / flow_time_ptr->Time ();
					} else {
						speed = length / dir_ptr->Time0 ();
					}
					if (speed < 0.1) speed = 0.1;

					time = (int) (length / speed + 0.5);
					if (time < 1) time = 1;

					link_delay.Time (time);
					link_delay.clear ();

					count += Put_Link_Delay_Data (*file, link_delay);
				}
			}
		}
	}
	Show_Progress (count);
	End_Progress ();
	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
}

//---------------------------------------------------------
//	Put_Link_Delay_Data
//---------------------------------------------------------

int Data_Service::Put_Link_Delay_Data (Link_Delay_File &file, Link_Delay_Data &data)
{
	int count;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Turn_Delay_Itr turn_itr;

	dir_ptr = &dir_array [data.Dir_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];

	//---- save the link record ----

	file.Link (link_ptr->Link ());
	file.Dir (dir_ptr->Dir ());

	file.Type (data.Type ());

	file.Start (data.Start ());
	file.End (data.End ());

	file.Flow (data.Flow ());
	file.Time (data.Time ());

	file.Num_Nest ((int) data.size ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	count = 1;

	//---- save the connections ----

	for (turn_itr = data.begin (); turn_itr != data.end (); turn_itr++) {
		dir_ptr = &dir_array [turn_itr->To_Index ()];
		link_ptr = &link_array [dir_ptr->Link ()];

		file.Out_Link (link_ptr->Link ());
		file.Out_Flow (turn_itr->Flow ());
		file.Out_Time (turn_itr->Time ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		count++;
	}
	if (data.size () == 0 && file.Nest () == FLATTEN) {
		file.Blank_Nested_Fields ();
		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
	}
	return (count);
}
