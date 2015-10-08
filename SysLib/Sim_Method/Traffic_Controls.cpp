 //*********************************************************
//	Traffic_Controls.cpp - prepare traffic signal data
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Traffic_Controls
//---------------------------------------------------------

void Sim_Method::Traffic_Controls (bool initialize)
{
	int index, ring, key, num, timing;
	int barrier, bar, max_bar, min_bar_time [20], max_bar_time [20];
	Dtime offset, cycle, time, green, end_time;
	bool flag, barrier_flag [20];

	Sim_Signal_Itr signal_itr;
	Signal_Data *signal_ptr;
	Signal_Time_Itr time_itr;
	Timing_Itr timing_itr;
	Timing_Phase_Itr phase_itr;
	Timing_Phase *phase_ptr;
	Phasing_Itr phasing_itr;
	Phasing_Data *phasing_ptr;
	Ring_Data ring_rec;
	Ring_Itr ring_itr;
	Int_Map phase_sort;
	Int_Map_Itr sort_itr;
	Int_Itr int_itr;
	Movement_Itr move_itr;
	Connect_Data *connect_ptr;

	//---- initialize the array ----

	if (initialize) {
		Signal_Itr sig_itr;
		Node_Data *node_ptr;
		Sim_Signal_Data *sim_signal_ptr;

		for (index=0, sig_itr = exe->signal_array.begin (); sig_itr != exe->signal_array.end (); sig_itr++, index++) {
			int_itr = sig_itr->nodes.begin ();
			node_ptr = &exe->node_array [*int_itr];

			if (node_ptr->Partition () == partition) {
				sim_signal_ptr = &exe->sim_signal_array [index];
				sim_signal_ptr->Partition (partition);
			}
		}
	}
	timing_update_time = signal_update_time = MAX_INTEGER;

	//---- check the end time of each signal ----

	for (index=0, signal_itr = exe->sim_signal_array.begin (); signal_itr != exe->sim_signal_array.end (); signal_itr++, index++) {
		if (signal_itr->Partition () != partition) continue;

		if (exe->step < signal_itr->End_Time () && signal_itr->End_Time () < MAX_INTEGER) {
			if (signal_itr->End_Time () < timing_update_time) timing_update_time = signal_itr->End_Time ();
			if (signal_itr->Check_Time () < signal_update_time) signal_update_time = signal_itr->Check_Time ();
			continue;
		}
		signal_ptr = &exe->signal_array [index];

		//---- clear the previous controls ----

		for (ring_itr = signal_itr->begin (); ring_itr != signal_itr->end (); ring_itr++) {
			key = ring_itr->at (ring_itr->Phase_Index ());
			if (key < 0) continue;

			phasing_ptr = &signal_ptr->phasing_plan [key >> 8];

			for (move_itr = phasing_ptr->begin (); move_itr != phasing_ptr->end (); move_itr++) {
				connect_ptr = &exe->connect_array [move_itr->Connection ()];

				connect_ptr->Control (RED_LIGHT);
			}
		}
		signal_itr->clear ();	

		//---- identify the time period ----
	
		for (time_itr = signal_ptr->begin (); time_itr != signal_ptr->end (); time_itr++) {
			if (time_itr->Start () > exe->step || exe->step >= time_itr->End ()) continue;

			signal_itr->End_Time (time_itr->End ());
			if (signal_itr->End_Time () < timing_update_time) timing_update_time = signal_itr->End_Time ();

			for (timing = 0, timing_itr = signal_ptr->timing_plan.begin (); timing_itr != signal_ptr->timing_plan.end (); timing_itr++, timing++) {
				if (timing_itr->Timing () != time_itr->Timing ()) continue;

				signal_itr->Timing_Index (timing);

				//---- sort the phase sequence ----

				phase_sort.clear ();
				memset (barrier_flag, '\0', sizeof (barrier_flag));
				max_bar = 0;

				for (num = 0, phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++, num++) {
					key = phase_itr->Ring () * 32000 + phase_itr->Barrier () * 1600 + phase_itr->Position () * 40 + phase_itr->Phase ();
					phase_sort.insert (Int_Map_Data (key, num));
					barrier_flag [phase_itr->Barrier ()] = true;;
					if (phase_itr->Barrier () > max_bar) max_bar = phase_itr->Barrier ();
				}

				//----- build the ring phase list ----

				num = -1;
				bar = 1;
				ring_rec.Clear ();

				for (sort_itr = phase_sort.begin (); sort_itr != phase_sort.end (); sort_itr++) {
					ring = sort_itr->first / 32000;
					barrier = (sort_itr->first - ring * 32000) / 1600;

					if (ring != num) {
						if (num >= 0) {
							while (bar < max_bar) {
								if (barrier_flag [++bar]) {
									ring_rec.push_back (-bar);
								}
							}
							signal_itr->push_back (ring_rec);
							ring_rec.Clear ();
							bar = 1;
						}
						num = ring;
						for (; bar < barrier; bar++) {
							if (barrier_flag [bar]) {
								ring_rec.push_back (-bar);
							}
						}
					} else if ((bar + 1) < barrier) {
						for (++bar; bar < barrier; bar++) {
							if (barrier_flag [bar]) {
								ring_rec.push_back (-bar);
							}
						}
					}
					bar = barrier;
					ring_rec.push_back (sort_itr->second);
				}
				if (num >= 0) {
					while (bar < max_bar) {
						if (barrier_flag [++bar]) {
							ring_rec.push_back (-bar);
						}
					}
					signal_itr->push_back (ring_rec);
				}

				//---- map the phasing records to each ring phase sequence ----

				memset (min_bar_time, '\0', sizeof (min_bar_time));
				memset (max_bar_time, '\0', sizeof (max_bar_time));
				
				for (ring_itr = signal_itr->begin (); ring_itr != signal_itr->end (); ring_itr++) {
					time = end_time = 0;
					bar = MIN (1, max_bar);

					for (int_itr = ring_itr->begin (); int_itr != ring_itr->end (); int_itr++) {
						if (*int_itr < 0) continue;

						key = *int_itr & 0xFF;
						phase_ptr = &timing_itr->at (key);

						if (phase_ptr->Barrier () != bar) {
							if (end_time < time) end_time = time;
							if (time > min_bar_time [bar]) min_bar_time [bar] = time;
							if (end_time > max_bar_time [bar]) max_bar_time [bar] = end_time;
							time = end_time = 0;
							bar = phase_ptr->Barrier ();
						}
						time += cycle.Seconds (phase_ptr->Min_Green () + phase_ptr->Yellow () + phase_ptr->All_Red ());
						end_time += cycle.Seconds (phase_ptr->Max_Green () + phase_ptr->Yellow () + phase_ptr->All_Red ());

						for (num=0, phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++, num++) {
							if (phasing_itr->Phasing () == time_itr->Phasing () &&
								phasing_itr->Phase () == phase_ptr->Phase ()) {
								*int_itr = (num << 8) + key;
								break;
							}
						}
					}
					if (end_time < time) end_time = time;
					if (time > min_bar_time [bar]) min_bar_time [bar] = time;
					if (end_time > max_bar_time [bar]) max_bar_time [bar] = end_time;
				}

				//---- set the initial barrier offset ----

				cycle.Seconds (timing_itr->Cycle ());
				offset.Seconds (timing_itr->Offset ());

				offset = (exe->step - (time_itr->Start () + offset)) % cycle;
				if (offset < 0) offset += cycle;

				time = exe->step - offset;
				barrier = offset = 0;
				bar = MIN (1, max_bar);

				for (; bar <= max_bar; bar++) {
					end_time = time + max_bar_time [bar];
					if (exe->step >= time && exe->step < end_time) {
						offset = exe->step - time;
						barrier = time;
						break;
					}
					time = end_time;
				}

				//---- set the initial timing offset ----
				
				for (ring_itr = signal_itr->begin (); ring_itr != signal_itr->end (); ring_itr++) {

					time = barrier;	

					//----find the barrier number ----

					for (num=0, int_itr = ring_itr->begin (); int_itr != ring_itr->end (); int_itr++, num++) {

						//---- check if the barrier has phases ----

						if (*int_itr < 0) {
							if (*int_itr != -bar) continue;
							ring_itr->Start (time);
							end_time = time + min_bar_time [bar];

							if (end_time <= exe->step) {
								end_time = time + max_bar_time [bar];
								end_time = (end_time + exe->step) / 2;
							}
							ring_itr->Check (end_time);
							ring_itr->Phase_Index (num);
							ring_itr->Status (RED_LIGHT);
							if (end_time < signal_itr->Check_Time ()) signal_itr->Check_Time (end_time);
							break;
						}
						key = *int_itr;
						phase_ptr = &timing_itr->at (key & 0xFF);

						if (phase_ptr->Barrier () != bar) continue;

						//---- find the position offset within the barrier ----

						ring_itr->Start (time);
						flag = false;

						for (green = phase_ptr->Min_Green (); ; green += phase_ptr->Extension ()) {
							if (green > phase_ptr->Max_Green () && phase_ptr->Max_Green () > 0) green = phase_ptr->Max_Green ();
							end_time = ring_itr->Start () + offset.Seconds (green);

							if (exe->step >= time && exe->step < end_time) {
								ring_itr->Check (end_time);
								ring_itr->Phase_Index (num);
								ring_itr->Status (PROTECTED_GREEN);
								flag = true;
								if (end_time < signal_itr->Check_Time ()) signal_itr->Check_Time (end_time);
								break;
							}
							time = end_time;
							if (green == phase_ptr->Max_Green () ||
								timing_itr->Type () == TIMED ||
								phase_ptr->Extension () == 0) break;
						}
						if (flag) break;

						if (phase_ptr->Yellow () > 0) {
							end_time = time + offset.Seconds (phase_ptr->Yellow ());

							if (exe->step >= time && exe->step < end_time) {
								ring_itr->Start (time);
								ring_itr->Check (end_time);
								ring_itr->Phase_Index (num);
								ring_itr->Status (YELLOW_LIGHT);
								if (end_time < signal_itr->Check_Time ()) signal_itr->Check_Time (end_time);
								break;
							}
							time = end_time;
						}
						if (phase_ptr->All_Red () > 0) {
							end_time = time + offset.Seconds (phase_ptr->All_Red ());

							if (exe->step >= time && exe->step < end_time) {
								ring_itr->Start (time);
								ring_itr->Check (end_time);
								ring_itr->Phase_Index (num);
								ring_itr->Status (RED_LIGHT);
								if (end_time < signal_itr->Check_Time ()) signal_itr->Check_Time (end_time);
								break;
							}
							time = end_time;
						}
					}
					if (ring_itr->Check () < signal_update_time) signal_update_time = ring_itr->Check ();

					//---- set the connection control type ----

					if (int_itr != ring_itr->end () && *int_itr >= 0) {
						phasing_ptr = &signal_ptr->phasing_plan [*int_itr >> 8];

						for (move_itr = phasing_ptr->begin (); move_itr != phasing_ptr->end (); move_itr++) {
							connect_ptr = &exe->connect_array [move_itr->Connection ()];

							if (ring_itr->Status () == PROTECTED_GREEN) {
								if (move_itr->Protection () == PERMITTED) {
									connect_ptr->Control (PERMITTED_GREEN);
								} else if (move_itr->Protection () == STOP_PERMIT) {
									connect_ptr->Control (STOP_GREEN);
								} else {
									connect_ptr->Control (PROTECTED_GREEN);
								}
							} else {
								connect_ptr->Control (ring_itr->Status ());
							}
						}
					}
				}
				break;
			}
			break;
		}
	}
}
