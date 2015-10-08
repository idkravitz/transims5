 //*********************************************************
//	Update_Signals.cpp - adjust traffic signal data
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Update_Signals
//---------------------------------------------------------

void Sim_Method::Update_Signals (void)
{
	int key, num, num1, bar, index;
	Dtime time, green, min_time, dtime, barrier;
	bool flag;

	Sim_Signal_Itr signal_itr;
	Signal_Data *signal_ptr;
	Timing_Data *timing_ptr;
	Timing_Phase *phase_ptr;
	Phasing_Data *phasing_ptr;
	Ring_Itr ring_itr;
	Int_Itr int_itr;
	Movement_Itr move_itr;
	Connect_Data *connect_ptr;

	signal_update_time = MAX_INTEGER;
	phasing_ptr = 0;

	//---- check the phase time of each signal ----

	for (index=0, signal_itr = exe->sim_signal_array.begin (); signal_itr != exe->sim_signal_array.end (); signal_itr++, index++) {

		if (signal_itr->Partition () != partition) continue;

		//---- check the update time ----

		if (signal_itr->Check_Time () > exe->step) {
			if (signal_itr->Check_Time () < signal_update_time) signal_update_time = signal_itr->Check_Time ();
			continue;
		}
		signal_ptr = &exe->signal_array [index];
		timing_ptr = &signal_ptr->timing_plan [signal_itr->Timing_Index ()];

		//---- check for a barrier change ----

		flag = false;
		barrier = exe->step;
		min_time = MAX_INTEGER;

		if (signal_itr->size () > 1) {

			//---- coordinate barrier crossing ----

			for (ring_itr = signal_itr->begin (); ring_itr != signal_itr->end (); ring_itr++) {
				num = ring_itr->Phase_Index ();
				key = ring_itr->at (num);

				if (key < 0) {
					bar = -key;
				} else {
					phase_ptr = &timing_ptr->at (key & 0xFF);
					bar = phase_ptr->Barrier ();

					time = ring_itr->Check ();
					if (ring_itr->Status () == PROTECTED_GREEN) {

						//---- check the extension detectors ----

						if (time == exe->step && timing_ptr->Type () == ACTUATED && phase_ptr->Extension () > 0) {
							green = exe->step - ring_itr->Start ();
							if (green < dtime.Seconds (phase_ptr->Max_Green ())) {
								green += dtime.Seconds (phase_ptr->Extension ());
								if (green > dtime.Seconds (phase_ptr->Max_Green ())) {
									green = dtime;
								}
								phasing_ptr = &signal_ptr->phasing_plan [key >> 8];

								for (int_itr = phasing_ptr->detectors.begin (); int_itr != phasing_ptr->detectors.end (); int_itr++) {
									if (Check_Detector (*int_itr)) {
										time = ring_itr->Start () + green;
										ring_itr->Check (time);
										break;
									}
								}
							}
						}
						time += dtime.Seconds (phase_ptr->Yellow () + phase_ptr->All_Red ());
					} else if (ring_itr->Status () == YELLOW_LIGHT) {
						time += dtime.Seconds (phase_ptr->All_Red ());
					}
					if (time > barrier) barrier = time;
				}
				num++;
				num = num % (int) ring_itr->size ();

				key = ring_itr->at (num);
				if (key < 0) {
					if (bar != -key) {
						flag = true;
					}
				} else {
					phase_ptr = &timing_ptr->at (key & 0xFF);

					if (phase_ptr->Barrier () != bar) {
						if (timing_ptr->Type () == ACTUATED && phase_ptr->Extension () > 0) {
							green = dtime.Seconds (phase_ptr->Min_Green ());
						} else {
							green = dtime.Seconds (MAX (phase_ptr->Min_Green (), phase_ptr->Max_Green ()));
						}
						time = green + dtime.Seconds (phase_ptr->Yellow () + phase_ptr->All_Red ());
						if (time < min_time) min_time = time;
						flag = true;
					}
				}
			}

			//---- adjust the barrier boundary times ----

			if (flag) {
				flag = false;

				for (ring_itr = signal_itr->begin (); ring_itr != signal_itr->end (); ring_itr++) {
					num = ring_itr->Phase_Index ();
					key = ring_itr->at (num);

					if (key < 0) {
						ring_itr->Check (barrier);
						continue;
					}
					if (ring_itr->Check () > exe->step) continue;

					phase_ptr = &timing_ptr->at (key & 0xFF);
					bar = phase_ptr->Barrier ();

					green = barrier - dtime.Seconds (phase_ptr->Yellow () - phase_ptr->All_Red ());
					time = barrier - dtime.Seconds (phase_ptr->All_Red ());

					num++;
					num = num % (int) ring_itr->size ();

					key = ring_itr->at (num);
					if (key < 0) {
						if (bar == -key) continue;
					} else {
						phase_ptr = &timing_ptr->at (key & 0xFF);
						if (phase_ptr->Barrier () == bar) continue;
					}
					if (ring_itr->Status () == PROTECTED_GREEN) {
						if (ring_itr->Check () < green) ring_itr->Check (green);
					} else if (ring_itr->Status () == YELLOW_LIGHT) {
						if (ring_itr->Check () < time) ring_itr->Check (time);
					} else {
						if (ring_itr->Check () < barrier) ring_itr->Check (barrier);
					}
				}
			}
		}

		//---- adjust the timing within the barrier ----

		signal_itr->Check_Time (MAX_INTEGER);

		for (ring_itr = signal_itr->begin (); ring_itr != signal_itr->end (); ring_itr++) {
			if (ring_itr->Check () > exe->step) {
				if (ring_itr->Check () < signal_itr->Check_Time ()) signal_itr->Check_Time (ring_itr->Check ());
				if (ring_itr->Check () < signal_update_time) signal_update_time = ring_itr->Check ();
				continue;
			}
			key = ring_itr->at (ring_itr->Phase_Index ());

			if (key < 0) {
				if (barrier > exe->step) {
					ring_itr->Check (barrier);
					if (barrier < signal_itr->Check_Time ()) signal_itr->Check_Time (barrier);
					if (barrier < signal_update_time) signal_update_time = barrier;
					continue;
				}
				flag = true;
			} else {
				phase_ptr = &timing_ptr->at (key & 0xFF);
				phasing_ptr = &signal_ptr->phasing_plan [key >> 8];

				flag = false;

				if (ring_itr->Status () == PROTECTED_GREEN) {

					//---- check the signal type ----

					if (timing_ptr->Type () == TIMED || phase_ptr->Extension () == 0) {

						//---- fixed timed signal ----

						if (phase_ptr->Yellow () > 0) {
							ring_itr->Start (exe->step);
							ring_itr->Check (exe->step + dtime.Seconds (phase_ptr->Yellow ()));
							ring_itr->Status (YELLOW_LIGHT);
						} else if (phase_ptr->All_Red () > 0) {
							ring_itr->Start (exe->step);
							ring_itr->Check (exe->step + dtime.Seconds (phase_ptr->All_Red ()));
							ring_itr->Status (RED_LIGHT);
						} else {
							flag = true;
						}

					} else {

						//---- check the extension detectors ----

						green = exe->step - ring_itr->Start ();
						if (green < dtime.Seconds (phase_ptr->Max_Green ())) {
							green += dtime.Seconds (phase_ptr->Extension ());
							if (green > dtime.Seconds (phase_ptr->Max_Green ())) {
								green = dtime;
							}
							for (int_itr = phasing_ptr->detectors.begin (); int_itr != phasing_ptr->detectors.end (); int_itr++) {
								if (Check_Detector (*int_itr)) {
									ring_itr->Check (ring_itr->Start () + green);
									flag = true;
									break;
								}
							}
							if (flag) {
								if (ring_itr->Check () < signal_itr->Check_Time ()) signal_itr->Check_Time (ring_itr->Check ());
								if (ring_itr->Check () < signal_update_time) signal_update_time = ring_itr->Check ();
								continue;
							}
						}
						if (phase_ptr->Yellow () > 0) {
							ring_itr->Start (exe->step);
							ring_itr->Check (exe->step + dtime.Seconds (phase_ptr->Yellow ()));
							ring_itr->Status (YELLOW_LIGHT);
						} else if (phase_ptr->All_Red () > 0) {
							ring_itr->Start (exe->step);
							ring_itr->Check (exe->step + dtime.Seconds (phase_ptr->All_Red ()));
							ring_itr->Status (RED_LIGHT);
						} else {
							flag = true;
						}
					}
				} else if (ring_itr->Status () == YELLOW_LIGHT) {
					if (phase_ptr->All_Red () > 0) {
						ring_itr->Start (exe->step);
						ring_itr->Check (exe->step + dtime.Seconds (phase_ptr->All_Red ()));
						ring_itr->Status (RED_LIGHT);
					} else {
						flag = true;
					}
				} else if (ring_itr->Status () == RED_LIGHT) {
					flag = true;
				}

				//---- set the current phase to red lights ----

				if (flag) {
					for (move_itr = phasing_ptr->begin (); move_itr != phasing_ptr->end (); move_itr++) {
						connect_ptr = &exe->connect_array [move_itr->Connection ()];
						connect_ptr->Control (RED_LIGHT);
					}
				}
			}
			if (flag) {

				//---- move to the next phase ---

				num = (ring_itr->Phase_Index () + 1) % (int) ring_itr->size ();
				key = ring_itr->at (num);

				if (key < 0) {
					ring_itr->Start (exe->step);
					ring_itr->Check (exe->step + min_time);
					ring_itr->Status (RED_LIGHT);
					ring_itr->Phase_Index (num);
					if (ring_itr->Check () < signal_itr->Check_Time ()) signal_itr->Check_Time (ring_itr->Check ());
					if (ring_itr->Check () < signal_update_time) signal_update_time = ring_itr->Check ();
					continue;
				}

				//---- check the demand actuated detectors ----

				if (timing_ptr->Type () == ACTUATED) {
					phase_ptr = &timing_ptr->at (key & 0xFF);
					bar = phase_ptr->Barrier ();
					flag = false;

					while (num != ring_itr->Phase_Index ()) {
						phasing_ptr = &signal_ptr->phasing_plan [key >> 8];
						if (phasing_ptr->size () == 0) {
							flag = true;
							break;
						}
						for (int_itr = phasing_ptr->detectors.begin (); int_itr != phasing_ptr->detectors.end (); int_itr++) {
							if (Check_Detector (*int_itr)) {
								flag = true;
								break;
							}
						}
						if (flag) break;
						num1 = (num + 1) % (int) ring_itr->size ();

						key = ring_itr->at (num1);
						if (key >= 0) {
							phase_ptr = &timing_ptr->at (key & 0xFF);
							if (phase_ptr->Barrier () == bar) {
								num = num1;
								continue;
							}
						}
						flag = true;
						break;
					}

					if (!flag) {
						num = ring_itr->Phase_Index ();

						key = ring_itr->at (num);
						if (key >= 0) {
							phasing_ptr = &signal_ptr->phasing_plan [key >> 8];

							for (int_itr = phasing_ptr->detectors.begin (); int_itr != phasing_ptr->detectors.end (); int_itr++) {
								if (Check_Detector (*int_itr)) {
									flag = true;
									break;
								}
							}
						} else {
							flag = true;
						}
						if (!flag) {
							num++;
							num = num % (int) ring_itr->size ();
						}
					}
				}

				//---- initialize the next phase ----

				key = ring_itr->at (num);
				if (key < 0) {
					ring_itr->Start (exe->step);
					ring_itr->Check (exe->step + min_time);
					ring_itr->Status (RED_LIGHT);
					ring_itr->Phase_Index (num);
					if (ring_itr->Check () < signal_itr->Check_Time ()) signal_itr->Check_Time (ring_itr->Check ());
					if (ring_itr->Check () < signal_update_time) signal_update_time = ring_itr->Check ();
					continue;
				}
				
				//---- set the next phase timing ----

				phase_ptr = &timing_ptr->at (key & 0xFF);
				phasing_ptr = &signal_ptr->phasing_plan [key >> 8];
				
				ring_itr->Start (exe->step);
				ring_itr->Phase_Index (num);

				green.Seconds (phase_ptr->Min_Green ());

				if (timing_ptr->Type () == TIMED && dtime.Seconds (phase_ptr->Max_Green ()) > green) {
					green = dtime;
				}
				
				if (green > 0) {
					ring_itr->Check (exe->step + green);
					ring_itr->Status (PROTECTED_GREEN);
				} else if (phase_ptr->Yellow ()) {
					ring_itr->Check (exe->step + dtime.Seconds (phase_ptr->Yellow ()));
					ring_itr->Status (YELLOW_LIGHT);
				} else {
					ring_itr->Check (exe->step + dtime.Seconds (phase_ptr->All_Red ()));
					ring_itr->Status (RED_LIGHT);
				}
			}

			//---- set the connection control type ----

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
			if (ring_itr->Check () < signal_itr->Check_Time ()) signal_itr->Check_Time (ring_itr->Check ());
			if (ring_itr->Check () < signal_update_time) signal_update_time = ring_itr->Check ();
		}
	}
}
