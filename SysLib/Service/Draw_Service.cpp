//*********************************************************
//	Draw_Service.cpp - shape drawing service keys and data
//*********************************************************

#include "Draw_Service.hpp"

//---------------------------------------------------------
//	Draw_Service constructor
//---------------------------------------------------------

Draw_Service::Draw_Service (void)
{
	lanes_flag = center_flag = direction_flag = overlap_flag = arrow_flag = curve_flag = shape_flag = false;

	link_offset = pocket_side = parking_side = location_side = sign_side = sign_setback = 0.0;
	stop_side = route_offset = lane_width = arrow_length = arrow_side = 0.0;
	width_factor = min_value = min_width = max_width = 0.0;
}

//---------------------------------------------------------
//	Draw_Service_Keys
//---------------------------------------------------------

void Draw_Service::Draw_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DRAW_NETWORK_LANES, "DRAW_NETWORK_LANES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ LANE_WIDTH, "LANE_WIDTH", LEVEL0, OPT_KEY, FLOAT_KEY, "3.5 meters", "0..40 meters", NO_HELP },
		{ CENTER_ONEWAY_LINKS, "CENTER_ONEWAY_LINKS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ LINK_DIRECTION_OFFSET, "LINK_DIRECTION_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ DRAW_AB_DIRECTION, "DRAW_AB_DIRECTION", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ POCKET_SIDE_OFFSET, "POCKET_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "2.0 meters", "0..82 meters", NO_HELP },
		{ PARKING_SIDE_OFFSET, "PARKING_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "3.0 meters", "0..164 meters", NO_HELP },
		{ LOCATION_SIDE_OFFSET, "LOCATION_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0 meters", "0..328 meters", NO_HELP },
		{ SIGN_SIDE_OFFSET, "SIGN_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "2.0 meters", "0..164 meters", NO_HELP },
		{ SIGN_SETBACK, "SIGN_SETBACK", LEVEL0, OPT_KEY, FLOAT_KEY, "2.0 meters", "0..328 meters", NO_HELP },
		{ TRANSIT_STOP_SIDE_OFFSET, "TRANSIT_STOP_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "2.0 meters", "0..164 meters", NO_HELP },
		{ TRANSIT_DIRECTION_OFFSET, "TRANSIT_DIRECTION_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0..50 meters", NO_HELP },
		{ TRANSIT_OVERLAP_FLAG, "TRANSIT_OVERLAP_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ DRAW_ONEWAY_ARROWS, "DRAW_ONEWAY_ARROWS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ONEWAY_ARROW_LENGTH, "ONEWAY_ARROW_LENGTH", LEVEL0, OPT_KEY, FLOAT_KEY, "7.0 meters", "0.1..656 meters", NO_HELP },
		{ ONEWAY_ARROW_SIDE_OFFSET, "ONEWAY_ARROW_SIDE_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "1.75 meters", "0.1..164 meters", NO_HELP },
		{ CURVED_CONNECTION_FLAG, "CURVED_CONNECTION_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ DRAW_VEHICLE_SHAPES, "DRAW_VEHICLE_SHAPES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ BANDWIDTH_SCALING_FACTOR, "BANDWIDTH_SCALING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0 units/meter", "0.01..100000 units/meter", NO_HELP },
		{ MINIMUM_BANDWIDTH_VALUE, "MINIMUM_BANDWIDTH_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0", "0..100000", NO_HELP },
		{ MINIMUM_BANDWIDTH_SIZE, "MINIMUM_BANDWIDTH_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0 meters", "0.001..10 meters", NO_HELP },
		{ MAXIMUM_BANDWIDTH_SIZE, "MAXIMUM_BANDWIDTH_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "1000.0 meters", "1..10000 meters", NO_HELP },
		END_CONTROL
	};

	if (keys == 0) {
		exe->Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					exe->Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				exe->Error (String ("Draw Service Key %d was Not Found") % keys [i]);
			}
		}
	}
}

//---------------------------------------------------------
//	Read_Draw_Keys
//---------------------------------------------------------

void Draw_Service::Read_Draw_Keys (void)
{
	exe->Print (2, "Draw Service Controls:");

	if (dat->System_File_Flag (LINK)) {

		//---- draw lanes ----

		if (exe->Control_Key_Status (DRAW_NETWORK_LANES)) {
			lanes_flag = exe->Get_Control_Flag (DRAW_NETWORK_LANES);
		}

		//---- lane width ----

		if ((lanes_flag || dat->System_File_Flag (CONNECTION)) && exe->Control_Key_Status (LANE_WIDTH)) {
			lane_width = exe->Get_Control_Double (LANE_WIDTH);

			if (lanes_flag && lane_width == 0.0) {
				exe->Error ("Lane Width must be > 0.0 to Draw Lanes");
			}
		}

		//---- center oneway links ----

		if (exe->Control_Key_Status (CENTER_ONEWAY_LINKS)) {
			center_flag = exe->Get_Control_Flag (CENTER_ONEWAY_LINKS);
		}

		if (!lanes_flag) {

			//---- link direction offset ----

			if (exe->Control_Key_Status (LINK_DIRECTION_OFFSET)) {
				link_offset = exe->Get_Control_Double (LINK_DIRECTION_OFFSET);

				if (link_offset == 0.0 && exe->Control_Key_Status (DRAW_AB_DIRECTION)) {
					direction_flag = exe->Get_Control_Flag (DRAW_AB_DIRECTION);
				}
			}

			//---- pocket side offset ----

			if (dat->System_File_Flag (POCKET) && exe->Control_Key_Status (POCKET_SIDE_OFFSET)) {
				pocket_side = exe->Get_Control_Double (POCKET_SIDE_OFFSET);
			}
		}
	}

	//---- activity location side offset ----

	if (dat->System_File_Flag (LOCATION) && exe->Control_Key_Status (LOCATION_SIDE_OFFSET)) {
		location_side = exe->Get_Control_Double (LOCATION_SIDE_OFFSET);
	}

	//---- parking side offset ----

	if (dat->System_File_Flag (PARKING) && exe->Control_Key_Status (PARKING_SIDE_OFFSET)) {
		parking_side = exe->Get_Control_Double (PARKING_SIDE_OFFSET);
	}

	//---- sign side offsets ----

	if (dat->System_File_Flag (SIGN)) {
		if (exe->Control_Key_Status (SIGN_SIDE_OFFSET)) {
			sign_side = exe->Get_Control_Double (SIGN_SIDE_OFFSET);
		}
		if (exe->Control_Key_Status (SIGN_SETBACK)) {
			sign_setback = exe->Get_Control_Double (SIGN_SETBACK);
		}
	}

	//---- transit stop offset ----

	if (dat->System_File_Flag (TRANSIT_STOP) && exe->Control_Key_Status (TRANSIT_STOP_SIDE_OFFSET)) {
		stop_side = exe->Get_Control_Double (TRANSIT_STOP_SIDE_OFFSET);
	}

	//---- transit direction offset ----

	if (dat->System_File_Flag (TRANSIT_ROUTE) || dat->System_File_Flag (TRANSIT_DRIVER)) {

		if (exe->Control_Key_Status (TRANSIT_DIRECTION_OFFSET)) {
			route_offset = exe->Get_Control_Double (TRANSIT_DIRECTION_OFFSET);
		}

		//---- transit overlap ----

		if (exe->Control_Key_Status (TRANSIT_OVERLAP_FLAG)) {
			overlap_flag = exe->Get_Control_Flag (TRANSIT_OVERLAP_FLAG);

			if (!overlap_flag) {
				if (route_offset == 0.0) {
					exe->Error ("Transit Direction Offset is Required for Overlap Processing");
				}
				if (dat->System_File_Flag (TRANSIT_ROUTE) && !dat->System_File_Flag (TRANSIT_DRIVER)) {
					exe->Error ("Transit Driver file is Required for Overlap Processing");
				}
			}
		}
	}

	//---- oneway arrow ----

	if (dat->System_File_Flag (LINK) && exe->Control_Key_Status (DRAW_ONEWAY_ARROWS) &&
		exe->Check_Control_Key (DRAW_ONEWAY_ARROWS)) {
		exe->Print (1);
		arrow_flag = exe->Get_Control_Flag (DRAW_ONEWAY_ARROWS);
		
		if (exe->Control_Key_Status (ONEWAY_ARROW_LENGTH)) {
			if (exe->Check_Control_Key (ONEWAY_ARROW_LENGTH) || lane_width == 0.0) {
				arrow_length = exe->Get_Control_Double (ONEWAY_ARROW_LENGTH);
			} else {
				arrow_length = lane_width * 2.0;
			}
		}
		if (exe->Control_Key_Status (ONEWAY_ARROW_SIDE_OFFSET)) {
			if (exe->Check_Control_Key (ONEWAY_ARROW_SIDE_OFFSET) || lane_width == 0.0) {
				arrow_side = exe->Get_Control_Double (ONEWAY_ARROW_SIDE_OFFSET);
			} else {
				arrow_side = lane_width / 2.0;
			}
		}
	}

	//---- curved connections ----

	if (exe->Control_Key_Status (CURVED_CONNECTION_FLAG)) {
		if ((dat->System_File_Flag (CONNECTION) || dat->System_File_Flag (PHASING_PLAN)) && 
			exe->Check_Control_Key (CURVED_CONNECTION_FLAG)) {
			exe->Print (1);
			curve_flag = exe->Get_Control_Flag (CURVED_CONNECTION_FLAG);
		}
	}

	//---- draw vehicle shapes ----

	if (exe->Control_Key_Status (DRAW_VEHICLE_SHAPES)) {
		shape_flag = exe->Get_Control_Flag (DRAW_VEHICLE_SHAPES);
	}

	//---- bandwidth scaling factor ----

	if (exe->Control_Key_Status (BANDWIDTH_SCALING_FACTOR)) {
		width_factor= exe->Get_Control_Double (BANDWIDTH_SCALING_FACTOR);

		//---- minimum bandwidth value ----

		min_value = exe->Get_Control_Double (MINIMUM_BANDWIDTH_VALUE);

		//---- minimum bandwidth size ----

		min_width = exe->Get_Control_Double (MINIMUM_BANDWIDTH_SIZE);

		//---- maximum bandwidth size ----

		max_width = exe->Get_Control_Double (MAXIMUM_BANDWIDTH_SIZE);
	}
}

