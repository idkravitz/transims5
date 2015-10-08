//*********************************************************
//	Best_Lane_Use.cpp - calculate lane use impedance
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Best_Lane_Use
//---------------------------------------------------------

bool Path_Builder::Best_Lane_Use (int index, Dtime time, Dtime &ttime, Dtime &delay, int &cost, int &group)
{
	int i, period, num, tt, cst, imp, lanes, type, grp, use_type [2], costs [2], best [2];
	Dtime ttimes [2], delays [2];
	double factor;

	Dir_Data *dir_ptr;
	Lane_Use_Data *use_ptr;
	Int_Map_Stat map_stat;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *use_index;
	Flow_Time_Array *link_delay_ptr;

	dir_ptr = &exe->dir_array [index];

	if (param.delay_flag) {
		link_delay_ptr = exe->link_delay_array.Period_Ptr (time);

		if (link_delay_ptr != 0) {
			ttimes [0] = link_delay_ptr->Time (index);

			if (dir_ptr->Flow_Index () >= 0) {
				ttimes [1] = link_delay_ptr->Time (dir_ptr->Flow_Index ());
			}
		}
	}
	if (ttimes [0] == 0) ttimes [0] = dir_ptr->Time0 ();
	if (ttimes [1] == 0) ttimes [1] = dir_ptr->Time0 ();

	ttime = ttimes [0];
	delay = 0;
	cost = group = 0;

	//---- find the time period ----

	period = dir_ptr->First_Lane_Use ();
	if (period < 0) return (true);

	for (period_ptr = &exe->use_period_array [period]; ; period_ptr = &exe->use_period_array [++period]) {
		if (period_ptr->Start () <= time && time < period_ptr->End ()) break;
		if (period_ptr->Periods () == 0) return (true);
	}

	//---- set the lane group data ----

	best [0] = best [1] = 0;
	delays [0] = delays [1] = 0;
	costs [0] = costs [1] = 0;

	use_type [0] = (period_ptr->Lanes (0) == 0) ? PROHIBIT : APPLY;
	use_type [1] = (period_ptr->Lanes (1) == 0) ? PROHIBIT : APPLY;

	type = param.traveler_type;

	num = period_ptr->Records ();
	index = period_ptr->Index ();

	for (i=0; i < num; i++, index++) {
		use_index = &exe->use_period_index [index];
		use_ptr = &exe->lane_use_array [use_index->Link ()];

		grp = use_index->Dir ();
		if (use_type [grp] == PROHIBIT) continue;

		lanes = use_ptr->High_Lane () - use_ptr->Low_Lane () + 1;

		if (lanes >= period_ptr->Lanes (grp)) {
			if (param.veh_type < 0 || use_ptr->Min_Veh_Type () < 0 || 
				(use_ptr->Min_Veh_Type () <= param.veh_type && param.veh_type <= use_ptr->Max_Veh_Type ())) {

				if (type == 0 || use_ptr->Min_Traveler () < 0 || 
					(use_ptr->Min_Traveler () <= type && type <= use_ptr->Max_Traveler ())) {

					if (Use_Permission (use_ptr->Use (), param.use)) {
						if (use_ptr->Type () == APPLY) {
							cst = use_ptr->Toll ();
							tt = use_ptr->Min_Delay ();
							if (use_ptr->Max_Delay () > use_ptr->Min_Delay ()) {
								map_stat = lane_use_delay.insert (Int_Map_Data (use_index->Link_Dir (), 0));
								if (map_stat.second) {
									map_stat.first->second = DTOI ((use_ptr->Max_Delay () - use_ptr->Min_Delay ()) * param.random.Probability ());
								}
								tt += map_stat.first->second;
							}
							imp = Resolve (tt * param.value_time + cst * param.value_cost);
							if (imp < best [grp] || best [grp] == 0) {
								delays [grp] = tt;
								costs [grp] = cst;
								best [grp] = imp;
							}
							use_type [grp] = APPLY;
						} else if (use_ptr->Type () == PROHIBIT) {
							use_type [grp] = PROHIBIT;
						} else if (use_ptr->Type () == REQUIRE) {
							use_type [1-grp] = PROHIBIT;
						}
						continue;
					}
				}
			}
			if (use_ptr->Type () == LIMIT) {
				use_type [grp] = PROHIBIT;
			}
		}
	}

	//---- select the best lane group ----

	if (use_type [0] == PROHIBIT && use_type [1] == PROHIBIT) return (false);

	if (use_type [0] == APPLY && use_type [1] == APPLY) {
		best [0] += Resolve (ttimes [0] * param.value_time);
		best [1] += Resolve (ttimes [1] * param.value_time);

		if (random_flag) {
			factor = 1.0 + param.random_imped * (param.random.Probability () - 0.5) / 100.0;
			best [1] = DTOI (best [1] * factor);
		}
		group = (best [1] < best [0]) ? 1 : 0;
	} else if (use_type [0] == APPLY) {
		group = 0;
	} else {
		group = 1;
	}
	delay = delays [group];
	cost = costs [group];
	ttime = ttimes [group];
	return (true);
}
