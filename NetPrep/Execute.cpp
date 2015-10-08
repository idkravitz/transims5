//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void NetPrep::Execute (void)
{
	//---- read the conversion script ----

	if (convert_flag) {
		if (Report_Flag (SCRIPT_REPORT)) {
			Header_Number (SCRIPT_REPORT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		Db_Base_Array files;

		if (link_shape_flag) {
			files.push_back (&link_shape_file);
		} else if (link_flag) {
			files.push_back (&link_file);
		} else {
			files.push_back (System_File_Base (LINK));
		}
		files.push_back (System_File_Base (NEW_LINK));
		if (detail_flag) {
			files.push_back (&detail_file);
		}
		convert.Initialize (files, random.Seed () + 1);

		//---- permit writing to input fields ----

		convert.Read_Only (0, false);

		if (!convert.Compile (script_file, Report_Flag (SCRIPT_REPORT))) {
			Error ("Compiling Conversion Script");
		}
		if (Report_Flag (STACK_REPORT)) {
			Header_Number (STACK_REPORT);

			convert.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- setup network data ----

	Data_Service::Execute ();

	//---- read the input speed capacity file ----

	if (spdcap_flag) {
		Input_SpdCap ();
	}

	//---- read the input node file ----

	if (node_flag || node_shape_flag) {
		Input_Nodes ();

	}

	//---- read the input zone file ----

	if (zone_flag || zone_shape_flag) {
		Input_Zones ();
	}

	//---- read the input link file ----

	if (link_flag || link_shape_flag) {
		Input_Links ();
	}

	//---- read the input route file ----

	if (route_flag) {
		Input_Routes ();
	}

	if (length_flag) {
		Print (2, "Number of Length Ratio Updates = ") << num_ratio;
	}

	//---- split loops ----

	if (split_flag || num_loops > 0) {
		Print (2, "Number of Short Loops Deleted = ") << num_loops;
	}
	if (loop_flag && shape_flag) {
		Split_Loops ();
	}

	//---- build node list ----

	if (loop_flag || collapse_flag || drop_flag || short_flag || spacing_flag || segment_flag || 
		Report_Flag (CHECK_TYPES) || Report_Flag (NODE_TYPES)) {
		Node_List ();
	}

	//---- collapse nodes ----

	if (collapse_flag) {
		Collapse_Nodes ();
	}

	//---- drop dead end links ----

	if (drop_flag) {
		Drop_Links ();
	}

	//---- drop short links ----

	if (short_flag) {
		if (drop_flag) {
			Node_List ();
		}
		Bearing_Update ();
		Short_Links ();
	}

	//---- collapse nodes again ----

	if (collapse_flag) {
		Node_List ();
		Collapse_Nodes ();
	}

	//---- update the link bearings ----

	Bearing_Update ();

	//---- set the thru movements ----

	if (spacing_flag || segment_flag) {
		Thru_Links ();
	}

	//---- set local thru links ----

	if (segment_flag) {
		Local_Thru_Links ();
	}

	//---- local spacing checks ----

	if (spacing_flag) {
		Local_Spacing ();

		if (collapse_flag) {
			Node_List ();

			if (short_flag) {
				Bearing_Update ();
				Short_Links ();
				Node_List ();
			}
			Collapse_Nodes ();
		}

		//---- update the link bearings ----

		Bearing_Update ();
	}

	//---- write the network files ----

	if (System_File_Flag (NEW_NODE)) {
		Write_Nodes ();
	}
	if (new_zone_flag) {
		Write_Zones ();
	}
	if (shape_flag) {
		Write_Shapes ();
	}
	if (System_File_Flag (NEW_LINK)) {
		Write_Links ();
	}
	if (link_node_flag) {
		Write_Link_Nodes ();
	}
	if (route_flag) {
		Write_Route_Nodes ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case CHECK_TYPES:		//---- Check Intersections ----
				Check_Intersections ();
				break;
			case NODE_TYPES:		//---- intersection types ----
				Node_Types ();
				break;
			default:
				break;
		}
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void NetPrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case SCRIPT_REPORT:		//---- Conversion Script ----
			Print (1, "Conversion Script");
			Print (1);
			break;
		case STACK_REPORT:		//---- Conversion Stack ----
			Print (1, "Conversion Stack");
			Print (1);
			break;
		case CHECK_TYPES:		//---- Check Intersections ----
			Intersection_Header ();
			break;
		case NODE_TYPES:		//---- intersection types ----
			Node_Header ();
			break;
		default:
			break;
	}
}
