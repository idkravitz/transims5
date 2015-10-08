//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LineSum::Execute (void)
{
	Line_Report_Itr line_report_itr;
	Link_Report_Itr link_report_itr;
	On_Off_Report_Itr on_off_report_itr;
	Access_Report_Itr access_report_itr;
	Stop_Report_Itr stop_report_itr;
	Total_Report_Itr total_report_itr;
	Link_Rider_Itr link_rider_itr;

	//---- read the stop names ----

	if (station_flag) {
		Stop_Names ();
	}

	//---- read the service file ----

	if (service_flag) {
		Read_Service ();
	}

	//---- combine peak files ----

	if (peak_list.size () > 0) {
		Combine (0, peak_list, new_peak_file);
	}

	//---- combine offpeak files ----

	if (offpeak_list.size () > 0) {
		Combine (1, offpeak_list, new_offpeak_file);
	}

	//---- create the total file -----

	if (new_total_file.Is_Open ()) {
		int i = 0;
		File_List total_list;
		File_Itr list_itr;
		Db_Header *fh;

		total_list.Initialize ((int) (peak_list.size () + offpeak_list.size ())); 

		for (list_itr = peak_list.begin (); list_itr != peak_list.end (); list_itr++) {
			fh = *list_itr;
			fh->Open (0);
			total_list [i++] = fh;
		}
		for (list_itr = offpeak_list.begin (); list_itr != offpeak_list.end (); list_itr++) {
			fh = *list_itr;
			fh->Open (0);
			total_list [i++] = fh;
		}
		Combine (2, total_list, new_total_file);
	}

	//---- line reports ----

	if (!Report_Flag (LINE_REPORT)) {
		for (line_report_itr = line_report_array.begin (); line_report_itr != line_report_array.end (); line_report_itr++) {
			Line_Report (*line_report_itr);
		}
	}

	//---- link reports ----

	if (!Report_Flag (LINK_REPORT)) {	
		for (link_report_itr = link_report_array.begin (); link_report_itr != link_report_array.end (); link_report_itr++) {
			Link_Report (*link_report_itr);
		}
	}
	
	//---- on-off reports ----
	
	if (!Report_Flag (ON_OFF_REPORT)) {
		for (on_off_report_itr = on_off_report_array.begin (); on_off_report_itr != on_off_report_array.end (); on_off_report_itr++) {
			On_Off_Report (*on_off_report_itr);
		}
	}

	//---- access reports ----
	
	if (!Report_Flag (ACCESS_REPORT)) {
		for (access_report_itr = access_report_array.begin (); access_report_itr != access_report_array.end (); access_report_itr++) {
			Access_Report (*access_report_itr);
		}
	}
	
	//---- stop reports ----
	
	if (!Report_Flag (STOP_REPORT)) {
		for (stop_report_itr = stop_report_array.begin (); stop_report_itr != stop_report_array.end (); stop_report_itr++) {
			Stop_Report (*stop_report_itr);
		}
	}

	//---- total reports ----
	
	if (!Report_Flag (TOTAL_REPORT)) {
		for (total_report_itr = total_report_array.begin (); total_report_itr != total_report_array.end (); total_report_itr++) {
			Total_Report (*total_report_itr);
		}
	}

	//---- read the link shapes ----

	if (shape_flag) {
		Read_Shapes ();
	}

	//---- link rider file ----

	for (link_rider_itr = link_rider_array.begin (); link_rider_itr != link_rider_array.end (); link_rider_itr++) {
		Link_Rider (*link_rider_itr);
	}

	//---- route difference report ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case LINE_REPORT:		//---- line report ----
				for (line_report_itr = line_report_array.begin (); line_report_itr != line_report_array.end (); line_report_itr++) {
					Line_Report (*line_report_itr);
				}				
				break;
			case LINK_REPORT:		//---- link reports ----
				for (link_report_itr = link_report_array.begin (); link_report_itr != link_report_array.end (); link_report_itr++) {
					Link_Report (*link_report_itr);
				}
				break;
			case ON_OFF_REPORT:		//---- on-off reports ----
				for (on_off_report_itr = on_off_report_array.begin (); on_off_report_itr != on_off_report_array.end (); on_off_report_itr++) {
					On_Off_Report (*on_off_report_itr);
				}
				break;
			case ACCESS_REPORT:		//---- access reports ----
				for (access_report_itr = access_report_array.begin (); access_report_itr != access_report_array.end (); access_report_itr++) {
					Access_Report (*access_report_itr);
				}
				break;
			case STOP_REPORT:		//---- stop reports ----
				for (stop_report_itr = stop_report_array.begin (); stop_report_itr != stop_report_array.end (); stop_report_itr++) {
					Stop_Report (*stop_report_itr);
				}
				break;
			case TOTAL_REPORT:		//---- total reports ----
				for (total_report_itr = total_report_array.begin (); total_report_itr != total_report_array.end (); total_report_itr++) {
					Total_Report (*total_report_itr);
				}
				break;
			case DIFFERENCE_REPORT:	//---- difference report ----
				Compare ();
				break;
			default:
				break;
		}
	}
	if (Report_Flag (DIFFERENCE_REPORT)) {
		Compare ();
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void LineSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case LINE_REPORT:		//---- Line Report ----
			Line_Header ();
			break;
		case LINK_REPORT:		//---- Link Report ----
			Link_Header ();
			break;
		case ACCESS_REPORT:		//---- Access Report ----
			Access_Header ();
			break;
		case STOP_REPORT:		//---- Stop Report ----
			Stop_Header ();
			break;
		case TOTAL_REPORT:		//---- Total Report ----
			Total_Header ();
			break;
		default:
			break;
	}
}
