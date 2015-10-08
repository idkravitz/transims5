//*********************************************************
//	Cell_Use.cpp - check the cell use restrictions
//*********************************************************

#include "Sim_Method.hpp"

//---------------------------------------------------------
//	Cell_Use
//---------------------------------------------------------

bool Sim_Method::Cell_Use (Sim_Lane_Data *lane_ptr, int lane, int cell, Travel_Step &step, bool use_flag)
{
	int i, index, num, seed;
	Lane_Use_Data *use_ptr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *use_index;

	int type = step.sim_traveler_ptr->Type ();
	int veh_type = step.sim_veh_ptr->Type ();
	Use_Type use = step.veh_type_ptr->Use ();

	step.Delay (0);

	if (use_flag) {
		if (use != BUS && use != HOV2 && use != HOV3 && use != HOV4) return (false);
		if (Use_Permission (lane_ptr->Use (), SOV)) return (false);
	}
	if (!Use_Permission (lane_ptr->Use (), use)) return (false);

	if (veh_type >= 0 && lane_ptr->Min_Veh_Type () >= 0) {
		if (lane_ptr->Type () == PROHIBIT) {
			if (veh_type >= lane_ptr->Min_Veh_Type () && veh_type <= lane_ptr->Max_Veh_Type ()) return (false);
		} else {
			if (veh_type < lane_ptr->Min_Veh_Type () || veh_type > lane_ptr->Max_Veh_Type ()) return (false);
		}
	}
	if (type > 0 && lane_ptr->Min_Traveler () > 0) {
		if (lane_ptr->Type () == PROHIBIT) {
			if (type >= lane_ptr->Min_Traveler () && type <= lane_ptr->Max_Traveler ()) return (false);
		} else {
			if (type < lane_ptr->Min_Traveler () || type > lane_ptr->Max_Traveler ()) return (false);
		}
	}
	index = lane_ptr->First_Use ();
	if (index < 0) return (true);

	period_ptr = &exe->use_period_array [index];

	num = period_ptr->Records ();
	index = period_ptr->Index ();

	for (i=0; i < num; i++, index++) {
		use_index = &exe->use_period_index [index];
		use_ptr = &exe->lane_use_array [use_index->Link ()];

		if (use_ptr->Offset () > 0 || use_ptr->Length () > 0) {
			int offset = cell * param.cell_size;

			if (use_ptr->Offset () > offset || offset > (use_ptr->Offset () + use_ptr->Length ())) continue;
		}
		if (use_ptr->Type () == REQUIRE) {
			if (!Use_Permission (use_ptr->Use (), use)) continue;
			if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
				if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) continue;
			}
			if (type > 0 && use_ptr->Min_Traveler () > 0) {
				if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) continue;
			}
			return (lane >= use_ptr->Low_Lane () && lane <= use_ptr->High_Lane ());
		} else {
			if (lane < use_ptr->Low_Lane () || lane > use_ptr->High_Lane ()) continue;

			if (use_ptr->Type () == PROHIBIT) {
				if (use_flag && !Use_Permission (use_ptr->Use (), SOV)) return (false);
				if (!Use_Permission (use_ptr->Use (), use)) continue;
				if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
					if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) continue;
				}
				if (type > 0 && use_ptr->Min_Traveler () > 0) {
					if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) continue;
				}
				return (false);
			} else if (use_ptr->Type () == LIMIT) {
				if (use_flag && Use_Permission (use_ptr->Use (), SOV)) return (false);
				if (!Use_Permission (use_ptr->Use (), use)) return (false);
				if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
					if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) return (false);
				}
				if (type > 0 && use_ptr->Min_Traveler () > 0) {
					if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) return (false);
				}
				return (true);
			} else if (use_ptr->Type () == APPLY) {
				if (!Use_Permission (use_ptr->Use (), use)) continue;
				if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
					if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) continue;
				}
				if (type >= 0 && use_ptr->Min_Traveler () > 0) {
					if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) continue;
				}
				if (use_ptr->Min_Delay () > 0 || use_ptr->Max_Delay () > 0) {
					if (use_ptr->Offset () > 0 || use_ptr->Length () > 0) {
						if (cell != (use_ptr->Offset () + (use_ptr->Length () / 2)) / param.cell_size) break;
					} else {
						if (cell != (int) (lane_ptr->size () / 2)) break;
					}
					int diff = 0;
					if (use_ptr->Max_Delay () > use_ptr->Min_Delay ()) {
						seed = abs (index + exe->Random_Seed () + step.Vehicle () + lane);
						diff = DTOI ((use_ptr->Max_Delay () - use_ptr->Min_Delay ()) * exe->random.Probability (seed));
					}
					step.Delay (use_ptr->Min_Delay () + diff);
				}
				break;
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Cell_Use
//---------------------------------------------------------

bool Sim_Method::Cell_Use (Travel_Step &step)
{
	int lane, lanes, cell;

	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Lane_Ptr lane_ptr = 0;
	
	Cell_Data cell_rec = step.sim_veh_ptr->front ();

	if (cell_rec.Index () == step.Dir_Index ()) {
		if (step.sim_dir_ptr == 0) {
			step.sim_dir_ptr = &exe->sim_dir_array [cell_rec.Index ()];
		}
		sim_dir_ptr = step.sim_dir_ptr;
	} else {
		sim_dir_ptr = &exe->sim_dir_array [cell_rec.Index ()];
	}
	if (sim_dir_ptr->Method () != MESOSCOPIC) return (true);

	lanes = sim_dir_ptr->Lanes ();
	cell = cell_rec.Cell ();

	for (lane = 0; lane < lanes; lane++) {
		lane_ptr = sim_dir_ptr->Lane (lane);

		if (Cell_Use (lane_ptr, lane, cell, step)) return (true);
	}
	Dtime max_time = exe->step + param.max_wait_time;

	if (use_update_time < max_time) {
		int i, index, num;
		Lane_Use_Data *use_ptr;
		Lane_Use_Period *period_ptr;
		Link_Dir_Data *use_index;

		Use_Type use = step.veh_type_ptr->Use ();
		int veh_type = step.sim_veh_ptr->Type ();
		int type = step.sim_traveler_ptr->Type ();

		Dir_Data *dir_ptr = &exe->dir_array [cell_rec.Index ()];

		index = dir_ptr->First_Lane_Use ();
		if (index < 0) goto use_error;

		for (period_ptr = &exe->use_period_array [index]; ; period_ptr = &exe->use_period_array [++index]) {
			if (period_ptr->Start () >= use_update_time && period_ptr->Start () <= max_time) break;
			if (period_ptr->Periods () == 0) goto use_error;
		}
		num = period_ptr->Records ();
		index = period_ptr->Index ();

		for (i=0; i < num; i++, index++) {
			use_index = &exe->use_period_index [index];
			use_ptr = &exe->lane_use_array [use_index->Link ()];

			if (use_ptr->Offset () > 0 || use_ptr->Length () > 0) {
				int offset = cell * param.cell_size;

				if (use_ptr->Offset () > offset || offset > (use_ptr->Offset () + use_ptr->Length ())) continue;
			}
			if (use_ptr->Type () == PROHIBIT) {
				if (!Use_Permission (use_ptr->Use (), use)) {
					if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
						if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) {
							if (type > 0 && use_ptr->Min_Traveler () > 0) {
								if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) return (true);
							} else {
								return (true);
							}
						}
					} else if (type > 0 && use_ptr->Min_Traveler () > 0) {
						if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) return (true);
					} else {
						return (true);
					}
				}
			} else {
				if (Use_Permission (use_ptr->Use (), use)) {
					if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
						if (veh_type >= use_ptr->Min_Veh_Type () && veh_type <= use_ptr->Max_Veh_Type ()) {
							if (type > 0 && use_ptr->Min_Traveler () > 0) {
								if (type >= use_ptr->Min_Traveler () && type <= use_ptr->Max_Traveler ()) return (true);
							} else {
								return (true);
							}
						}
					} else if (type > 0 && use_ptr->Min_Traveler () > 0) {
						if (type >= use_ptr->Min_Traveler () && type <= use_ptr->Max_Traveler ()) return (true);
					} else {
						return (true);
					}
				}
			}
		}
	}
use_error:
	step.Problem (USE_PROBLEM);
	step.Status (1);
	return (false);
}
