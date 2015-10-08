//*********************************************************
//	Read_Phasing_Plans.cpp - read the phasing plan file
//*********************************************************

#include "Data_Service.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Read_Phasing_Plans
//---------------------------------------------------------

void Data_Service::Read_Phasing_Plans (void)
{
	Phasing_File *file = (Phasing_File *) System_File_Handle (PHASING_PLAN);

	int i, num, count, signal;
	bool keep;
	Signal_Data *signal_ptr;
	Phasing_Data phasing_rec;
	Signal_Itr signal_itr;
	Phasing_Itr phasing_itr;
	Timing_Itr timing_itr;
	Timing_Phase_Itr phase_itr;
	Signal_Time_Itr time_itr;

	//---- store the phasing plan data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Phasing_Plans (*file);
	count = 0;

	while (file->Read (false)) {
		Show_Progress ();

		phasing_rec.Clear ();

		signal = Get_Phasing_Data (*file, phasing_rec);

		num = file->Num_Nest ();
		if (num > 0) phasing_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error (String ("Number of Movement Records for Signal %d Plan %d") % 
					file->Signal () % file->Phasing ());
			}
			Show_Progress ();

			Get_Phasing_Data (*file, phasing_rec);
		}

		//---- save the results ----

		if (signal >= 0) {
			if (file->Version () <= 40) {
				Timing40_Map_Itr timing40_itr;
				Timing40_Data *timing40_ptr;

				timing40_itr = timing40_map.find (phasing_rec.Phasing ());

				if (timing40_itr == timing40_map.end ()) {
					Warning (String ("Phasing Plan %d was Not Found in the Signal file") % phasing_rec.Phasing ());
					continue;
				}
				timing40_ptr = (Timing40_Data *) &(timing40_itr->second);

				signal = timing40_ptr->Signal ();
				phasing_rec.Phasing (timing40_ptr->Timing ());
			}
			signal_ptr = &signal_array [signal];

			keep = true;

			for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
				if (phasing_itr->Phasing () == phasing_rec.Phasing () && phasing_itr->Phase () == phasing_rec.Phase ()) {
					if (file->Version () <= 40) {

						//---- add new detectors ----

						bool flag;
						Int_Itr itr1, itr2;

						for (itr1 = phasing_rec.detectors.begin (); itr1 != phasing_rec.detectors.end (); itr1++) {
							flag = true;

							for (itr2 = phasing_itr->detectors.begin (); itr2 != phasing_itr->detectors.end (); itr2++) {
								if (*itr1 == *itr2) {
									flag = false;
									break;
								}
							}
							if (flag) {
								phasing_itr->detectors.push_back (*itr1);
							}
						}
						Movement_Itr move_itr = phasing_rec.begin ();
						phasing_itr->push_back (*move_itr);
					} else {
						Warning (String ("Duplicate Phasing Plan %d-%d") % phasing_rec.Phasing () % phasing_rec.Phase ());
					}
					keep = false;
					break;
				}
			}
			if (keep) {
				signal_ptr->phasing_plan.push_back (phasing_rec);
				count += (int) phasing_rec.size () + 1;
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % count);
	}
	if (count > 0) System_Data_True (PHASING_PLAN);

	//---- check timing/phasing consistency ----

	for (signal_itr = signal_array.begin (); signal_itr != signal_array.end (); signal_itr++) {
		for (time_itr = signal_itr->begin (); time_itr != signal_itr->end (); time_itr++) {
			for (timing_itr = signal_itr->timing_plan.begin (); timing_itr != signal_itr->timing_plan.end (); timing_itr++) {
				if (timing_itr->Timing () == time_itr->Timing ()) break;
			}
			if (timing_itr == signal_itr->timing_plan.end ()) {
				Warning (String ("Signal %d Timing Plan %d was Not Found") % signal_itr->Signal () % time_itr->Timing ());
				continue;
			}
			for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
				for (phasing_itr = signal_itr->phasing_plan.begin (); phasing_itr != signal_itr->phasing_plan.end (); phasing_itr++) {
					if (phasing_itr->Phasing () == time_itr->Phasing () && phasing_itr->Phase () == phase_itr->Phase ()) break;
				}
				if (phasing_itr == signal_itr->phasing_plan.end ()) {
					Warning (String ("No Movements for Signal %d Phasing Plan %d Phase %d") % signal_itr->Signal () % time_itr->Phasing () % phase_itr->Phase ());
					continue;
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Initialize_Phasing_Plans
//---------------------------------------------------------

void Data_Service::Initialize_Phasing_Plans (Phasing_File &file)
{
	Required_File_Check (file, SIGNAL);
	Required_File_Check (file, TIMING_PLAN);
	Required_File_Check (file, LINK);
	Required_File_Check (file, CONNECTION);
}

//---------------------------------------------------------
//	Get_Phasing_Data
//---------------------------------------------------------

int Data_Service::Get_Phasing_Data (Phasing_File &file, Phasing_Data &phasing_rec)
{
	int signal, link, dir, node, dir_index, to_index, num, detector;
	String svalue;
	
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	signal = 1;

	//---- process the header record ----
	
	if (!file.Nested ()) {
		signal = file.Signal ();
		if (signal == 0) return (-1);

		map_itr = signal_map.find (signal);
		if (map_itr == signal_map.end ()) {
			Warning (String ("Phasing Signal %d was Not Found") % signal);
			return (-1);
		}
		signal = map_itr->second;

		phasing_rec.Phasing (file.Phasing ());
		phasing_rec.Phase (file.Phase ());

		//---- process the detectors ----

		svalue = file.Detectors ();

		if (!svalue.empty ()) {
			Strings detect;
			Str_Itr itr;

			num = svalue.Parse (detect, "|+/");

			for (itr = detect.begin (); itr != detect.end (); itr++) {
				detector = itr->Integer ();
				if (detector <= 0) break;

				map_itr = detector_map.find (detector);
				if (map_itr == detector_map.end ()) {
					Warning (String ("Signal %d Detector %d was Not Found") % file.Signal () % detector);
					break;
				}
				phasing_rec.detectors.push_back (map_itr->second);
			}
		}
		if (file.Version () > 40) return (signal);
	}
	Movement_Data move_rec;

	move_rec.Movement (file.Movement ());

	//---- convert the link direction ----
	
	link = file.Link ();
	dir = file.Dir ();

	link_ptr = Set_Link_Direction (file, link, dir);

	if (link_ptr == 0) return (-1);

	if (dir) {
		dir_index = link_ptr->BA_Dir ();
		node = link_ptr->Anode ();
	} else {
		dir_index = link_ptr->AB_Dir ();
		node = link_ptr->Bnode ();
	}
	if (dir_index < 0) {
		Warning (String ("Phasing Plan %d Link %d Direction %s was Not Found") % Progress_Count () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (-1);
	}

	//---- convert the to-link number ----

	link = file.To_Link ();

	map_itr = link_map.find (link);
	if (map_itr == link_map.end ()) {
		Warning (String ("%s Link %d was Not Found in the Link file") % file.File_ID () % link);
		return (-1);
	}
	link = map_itr->second;

	link_ptr = &link_array [link];

	if (link_ptr->Anode () == node) {
		to_index = link_ptr->AB_Dir ();
	} else if (link_ptr->Bnode () == node) {
		to_index = link_ptr->BA_Dir ();
	} else {
		Node_Data *ptr = &node_array [node];
		Warning (String ("%s Node %d is Not on Link %d") % file.File_ID () % ptr->Node () % link_ptr->Link ());
		return (-1);
	}

	//---- check the link connection ----

	map2_itr = connect_map.find (Int2_Key (dir_index, to_index));

	if (map2_itr == connect_map.end ()) {
		to_index = link_ptr->Link ();

		dir_ptr = &dir_array [dir_index];
		link_ptr = &link_array [dir_ptr->Link ()];
		dir_index = link_ptr->Link ();

		if (file.Version () > 40) {
			Warning (String ("No Link Connection between %d and %d") % dir_index % to_index);
		}
		return (-1);
	}

	//---- save the data ----

	move_rec.Connection (map2_itr->second);

	move_rec.Protection (file.Protection ());

	if (file.Version () <= 40 && move_rec.Movement () == 0) {
		Connect_Data *connect_ptr;
		const char *direction;

		dir_ptr = &dir_array [dir_index];
		direction = compass.Point_Text (dir_ptr->In_Bearing (), 4);

		if (*direction == 'N') {
			dir = NB_TH;
		} else if (*direction == 'S') {
			dir = SB_TH;
		} else if (*direction == 'E') {
			dir = EB_TH;
		} else {
			dir = WB_TH;
		}
		connect_ptr = &connect_array [map2_itr->second];
		num = connect_ptr->Type ();
		
		if (num == RIGHT || num == R_SPLIT) {
			dir += NB_RT - NB_TH;
		} else if (num == LEFT || num == L_SPLIT) {
			dir += NB_LT - NB_TH;
		} else if (num == UTURN) {
			dir += NB_UT - NB_TH;
		}
		move_rec.Movement (dir);
	}
	phasing_rec.push_back (move_rec);

	return (signal);
}
