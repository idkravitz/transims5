//*********************************************************
//	XML_Routes.cpp - convert transit routes data
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	XML_Routes
//---------------------------------------------------------

void ExportNet::XML_Routes (void)
{
	int index, hour, minute, second;
	String time;

	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Stop_Data *stop_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Node_Data *node_ptr;
	Driver_Itr driver_itr;
	Dtime start, depart, end;

	fstream &fh = xml_file.File ();

	Set_Progress ();

	fh << "<PTLINES>" << endl;

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		line_ptr = &line_array [map_itr->second];

		//---- check if in subarea ----

		for (index=0, stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
			stop_ptr = &stop_array [stop_itr->Stop ()];

			link_ptr = &link_array [stop_ptr->Link ()];
			if (link_ptr->Divided () == 0) continue;
			index++;
		}
		if (index == 0) continue;

		fh << "<PTLINE ID=\"" << line_ptr->Route ();
		fh << "\" NAME=\"" << ((line_ptr->Name () != 0) ? line_ptr->Name () : "");
		fh << "\" VEHTYPEID=\"" << line_ptr->Type () << "\" >" << endl;
		fh << "<PTLINEITEMS>" << endl;

		driver_itr = line_ptr->driver_array.begin ();

		for (index=0, stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
			stop_ptr = &stop_array [stop_itr->Stop ()];

			dir_ptr = &dir_array [*driver_itr];

			while (dir_ptr->Link () != stop_ptr->Link ()) {
				if (index > 0) {
					link_ptr = &link_array [dir_ptr->Link ()];
					if (link_ptr->Divided () != 0) {
						if (dir_ptr->Dir () == 0) {
							node_ptr = &node_array [link_ptr->Anode ()];
						} else {
							node_ptr = &node_array [link_ptr->Bnode ()];
						}
						index++;

						fh << "<PTLINEITEM INDEX=\"" << index;
						fh << "\" NODENO=\"" << node_ptr->Node ();
						fh << "\" />" << endl;
					}
				}
				driver_itr++;
				dir_ptr = &dir_array [*driver_itr];
			}
			link_ptr = &link_array [stop_ptr->Link ()];
			if (link_ptr->Divided () == 0) continue;
			index++;

			fh << "<PTLINEITEM INDEX=\"" << index;
			fh << "\" STOPPOINTID=\"" << stop_ptr->Stop ();
			run_itr = stop_itr->begin ();
			if (index == 1) {
				start = run_itr->Schedule ();
			} 
			depart = run_itr->Schedule () - start;

			fh << "\" DEP=\"PT" << (int) depart.Seconds () << "S\" STOPTIME=\"PT" << 5 << "S\" />" << endl;
		}
		fh << "</PTLINEITEMS>" << endl;

		if (sum_periods.Num_Periods () > 0) {
			start = sum_periods.Start ();
			end = sum_periods.End ();

		} else {
			start = 00;
			end.Hours (24);
		}

		stop_itr = line_ptr->begin ();
		fh << "<PTVEHJOURNEYS>" << endl;

		for (index=1, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, index++) {
			depart = run_itr->Schedule ();
			if (depart < start || depart >= end) continue;

			fh << "<PTVEHJOURNEY NO = \"" << index;

			depart = depart - start;

			second = DTOI (depart.Seconds ());
			hour = second / 3600;
			second = second - hour * 3600;
			minute = second / 60;

			time ("%dH%02dM") % hour % minute;

			fh << "\" DEPARTURE = \"PT" << time << "\" />" << endl;
		}
		fh << "</PTVEHJOURNEYS>" << endl;
		fh << "</PTLINE>" << endl;
	}
	fh << "</PTLINES>" << endl;
}
//
//<PTLINES>
//      <PTLINE ID = "B21;01;&gt;;01;1" NAME = "B21;01;&gt;;01;1" VEHTYPEID = "600">
//        <PTLINEITEMS>
//          <PTLINEITEM INDEX = "1" STOPPOINTID = "329WA" DEP = "PT0S"  STOPTIME = "PT0S" />          
//          <PTLINEITEM INDEX = "2" NODENO = "10940" />  
//          <PTLINEITEM INDEX = "3" NODENO = "11179" />          
//          <PTLINEITEM INDEX = "4" NODENO = "10945" />          
//          <PTLINEITEM INDEX = "5" NODENO = "712" />          
//          <PTLINEITEM INDEX = "6" NODENO = "10110" />          
//          <PTLINEITEM INDEX = "7" NODENO = "11151" />          
//          <PTLINEITEM INDEX = "8" NODENO = "719" />          
//          <PTLINEITEM INDEX = "9" NODENO = "51" />          
//          <PTLINEITEM INDEX = "10" STOPPOINTID = "51WA" DEP = "PT7M" STOPTIME = "PT0S" />          
//          <PTLINEITEM INDEX = "11" NODENO = "616" />          
//          <PTLINEITEM INDEX = "12" NODENO = "718" />          
//          <PTLINEITEM INDEX = "13" NODENO = "10988" />          
//          <PTLINEITEM INDEX = "14" NODENO = "10986" />          
//          <PTLINEITEM INDEX = "15" NODENO = "313" />          
//          <PTLINEITEM INDEX = "16" NODENO = "309" />          
//          <PTLINEITEM INDEX = "17" STOPPOINTID = "309WA" DEP = "PT9M" STOPTIME = "PT0S" />          
//		</PTLINEITEMS>
//      <PTVEHJOURNEYS>                                                                                                                                (Information 
//          <PTVEHJOURNEY NO = "14" DEPARTURE = "PT9H20M" /> 
//          <PTVEHJOURNEY NO = "15" DEPARTURE = "PT9H50M" />          
//          <PTVEHJOURNEY NO = "16" DEPARTURE = "PT10H20M" />          
//          <PTVEHJOURNEY NO = "17" DEPARTURE = "PT10H50M" />          
//          <PTVEHJOURNEY NO = "18" DEPARTURE = "PT11H20M" />          
//          <PTVEHJOURNEY NO = "19" DEPARTURE = "PT11H50M" />          
//      </PTVEHJOURNEYS>
//    </PTLINE>
//</PTLINES>
