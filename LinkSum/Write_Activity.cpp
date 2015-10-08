//*********************************************************
//	Write_Activity.cpp - Write the Link Activity File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Activity
//---------------------------------------------------------

void LinkSum::Write_Activity (void)
{
	Show_Message ("Writing Link Activity File -- Record");
	Set_Progress ();

	Db_Sort_Itr itr;

	link_db.Rewind ();

	for (itr = link_db.begin (); itr != link_db.end (); itr++) {
		Show_Progress ();

		link_db.Record (itr->second);

		activity_file.Copy_Fields (link_db);

		if (!activity_file.Write ()) {
			Error ("Writing the Link Activity File");
		}
	}
	End_Progress ();

	Print (2, "Number of Link Activity Records = ") << Progress_Count ();

	activity_file.Close ();
}
