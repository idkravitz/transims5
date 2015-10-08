//*********************************************************
//	LinkData.cpp - Convert Link Data to Link ID
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	LinkData constructor
//---------------------------------------------------------

LinkData::LinkData (void) : Data_Service ()
{
	Program ("LinkData");
	Version (2);
	Title ("Convert Link Data to Link ID");

	int data_service_keys [] = {
		SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ LINK_NODE_LIST_FILE, "LINK_NODE_LIST_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DIRECTIONAL_DATA_FILE, "DIRECTIONAL_DATA_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DIRECTIONAL_DATA_FORMAT, "DIRECTIONAL_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "CSV_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ FROM_NODE_FIELD_NAME, "FROM_NODE_FIELD_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "ANODE", ANODE_FIELD_RANGE, NO_HELP },
		{ TO_NODE_FIELD_NAME, "TO_NODE_FIELD_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "BNODE", BNODE_FIELD_RANGE, NO_HELP },
		{ VOLUME_DATA_FIELD_NAME, "VOLUME_DATA_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ SPEED_DATA_FIELD_NAME, "SPEED_DATA_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ AB_VOLUME_FIELD_NAME, "AB_VOLUME_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ BA_VOLUME_FIELD_NAME, "BA_VOLUME_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ BA_SPEED_FIELD_NAME, "AB_SPEED_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ AB_SPEED_FIELD_NAME, "BA_SPEED_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ NEW_LINK_DATA_FILE, "NEW_LINK_DATA_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FORMAT, "NEW_LINK_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_AB_VOLUME_FIELD_NAME, "NEW_AB_VOLUME_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "AB_VOLUME", "", NO_HELP },
		{ NEW_BA_VOLUME_FIELD_NAME, "NEW_BA_VOLUME_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "BA_VOLUME", "", NO_HELP },
		{ NEW_AB_SPEED_FIELD_NAME, "NEW_AB_SPEED_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "AB_SPEED", "", NO_HELP },
		{ NEW_BA_SPEED_FIELD_NAME, "NEW_BA_SPEED_FIELD_NAME", LEVEL1, OPT_KEY, LIST_KEY, "BA_SPEED", "", NO_HELP },
		{ NEW_DIRECTIONAL_DATA_FILE, "NEW_DIRECTIONAL_DATA_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_DIRECTIONAL_DATA_FORMAT, "NEW_DIRECTIONAL_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		END_CONTROL
	};
	Data_Service_Keys (data_service_keys);

	Key_List (keys);

	volume_flag = speed_flag = two_way_flag = data_flag = custom_flag = output_flag = false;
	nequiv = nab = ndir = nlink = num_fields = 0;

	from_field = to_field = -1;
}

//---------------------------------------------------------
//	LinkData destructor
//---------------------------------------------------------

LinkData::~LinkData (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LinkData *exe = new LinkData ();

	return (exe->Start_Execution (commands, control));
}
