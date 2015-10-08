//*********************************************************
//	Build_Path - Build a Path and Plan records
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Build_Path
//---------------------------------------------------------

int Path_Builder::Build_Path (int lot)
{
	walk_flag = (param.max_walk != 0);
	bike_flag = (param.max_bike != 0);
	wait_flag = (param.max_wait != 0);
	rail_bias_flag = (param.rail_bias != 100 || param.rail_const != 0);
	bus_bias_flag = (param.bus_bias != 100 || param.bus_const != 0);
	turn_flag = (param.left_imped > 0 || param.right_imped > 0 || param.uturn_imped > 0);
	random_flag = (param.random_imped > 0);

	unsigned id = (unsigned) plan_ptr->Household () + plan_ptr->Person () + plan_ptr->Tour () + plan_ptr->Trip ();
	param.random.Seed ((unsigned) exe->Random_Seed () + id);

	if (parking_duration == 0) {
		parking_duration = param.parking_duration;
	}
	if (plan_flag && !param.ignore_time) {
		if (forward_flag) {
			time_limit = plan_ptr->End () + param.end_time;
		} else {
			time_limit = plan_ptr->Start () - param.end_time;
			if (time_limit < min_time_limit) time_limit = min_time_limit;
		}
	}
	lane_use_delay.clear ();

	//---- build the path ----

	max_imp = MAX_IMPEDANCE;
	imp_diff = 0;

	time_flag = dist_flag = length_flag = zero_flag = wait_time_flag = use_flag = false;
	transfer_flag = local_acc_flag = reset_veh_flag = walk_acc_flag = park_flag = access_flag = false;

	switch (param.mode) {
		case WALK_MODE:		//---- walk ----
			param.use = WALK;
			return (Node_Plan ());
		case BIKE_MODE:		//---- bike ----
			param.use = BIKE;
			return (Node_Plan ());
		case DRIVE_MODE:		//---- drive ----
			return (Drive_Plan (lot));
		case RIDE_MODE:			//---- ride ----
			return (Magic_Move ());
		case TRANSIT_MODE:		//---- walk to transit ----
			return (Transit_Plan ());
		case PNR_OUT_MODE:		//---- drive to transit ----
		case PNR_IN_MODE:		//---- transit to drive ----
		case KNR_OUT_MODE:		//---- ride to transit ----
		case KNR_IN_MODE:		//---- transit to ride ----
			return (Drive_Transit_Plan (lot));
		case TAXI_MODE:			//---- magic move ----
			param.use = HOV2;
			return (Drive_Plan (-1));
		case OTHER_MODE:		//---- school bus ----
			return (Magic_Move ());
		case HOV2_MODE:			//---- carpool 2+ ----
			param.use = HOV2;
			return (Drive_Plan (lot));
		case HOV3_MODE:			//---- carpool 3+ ----
			param.use = HOV3;
			return (Drive_Plan (lot));
		case HOV4_MODE:			//---- carpool 4+ ----
			param.use = HOV4;
			return (Drive_Plan (lot));
		default:
			return (MODE_PROBLEM);
	}
}
