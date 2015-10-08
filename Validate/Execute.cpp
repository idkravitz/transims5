//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Validate::Execute (void)
{

	//---- read the network ----

	Data_Service::Execute ();
	
	//----- create the node to zone equivalence -----

	if (zone_flag) {
		int node = 0;
		Node_Itr itr;

		node_zone.assign (node_array.size (), 0);

		for (itr = node_array.begin (); itr != node_array.end (); itr++, node++) {
			int *ptr = &node_zone [node];

			*ptr = Closest_Zone (itr->X (), itr->Y ());
		}
	}

	//---- process link volume data ----

	if (link_flag) {
		if (!delay_flag) {
			Read_Flow (&volume_file, true);
		}

		//---- read the count file ----

		Read_Flow (&count_file, false);
	}

	//---- process turning volume data ----

	if (turn_flag) {
		if (!delay_flag) {
			Read_Turn (&turn_volume, false);
		}

		//---- read the count file ----

		Read_Turn (&turn_count, true);
	}

	//---- write the output volume file ----

	if (output_flag) {
		Write_Volume ();
	}

	//---- write the volume count file ----

	if (vc_flag) {
		Write_Volume_Count ();
	}

	//---- summarize the transit ridership ----

	if (line_flag || stop_flag) {
		//line_array.Summarize_Riders ();
	}

	//---- generate each report ----

	Show_Message ("Writing Performance Reports");	

	New_Page ();

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		group_data.clear ();
		memset (&total, '\0', sizeof (total));

		switch (i) {
			case VOLUME_LEVEL:
                Volume_Stats ();
				break;
			case FACILITY_TYPE:
				Facility_Stats ();
				break;
			case AREA_TYPE:
				Area_Stats ();
				break;
			case ZONE_GROUP:
				Zone_Group_Stats ();
				break;
			case LINK_GROUP:
				Link_Group_Stats ();
				break;
			case GROUP_DETAILS:
				Link_Group_Details ();
				break;
			case TURN_MOVEMENT:
				Turn_Stats ();
				break;
			case TURN_LOS:
				Turn_LOS ();
				break;
			case LINE_GROUP:
				Line_Group_Stats ();
				break;
			case STOP_GROUP:
			case BOARD_GROUP:
			case ALIGHT_GROUP:
				Stop_Group_Stats (i);
				break;
			default:
				break;
		}
	}

	//---- process complete ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Validate::Page_Header (void)
{
	switch (Header_Number ()) {
		case VOLUME_LEVEL:
            Volume_Header ();
			break;
		case FACILITY_TYPE:
			Facility_Header ();
			break;
		case AREA_TYPE:
			Area_Header ();
			break;
		case ZONE_GROUP:
			Zone_Group_Header ();
			break;
		case LINK_GROUP:
			Link_Group_Header ();
			break;
		case GROUP_DETAILS:
			Group_Detail_Header ();
			break;
		case TURN_MOVEMENT:
			Turn_Header ();
			break;
		case TURN_LOS:
			Turn_LOS_Header ();
			break;
		case LINE_GROUP:
			Line_Group_Header ();
			break;
		case STOP_GROUP:
		case BOARD_GROUP:
		case ALIGHT_GROUP:
			Stop_Group_Header (Header_Number ());
			break;
		default:
			break;
	}
}
