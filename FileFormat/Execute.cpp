//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void FileFormat::Execute (void)
{
	int i, n, num, num_in, num_out;
	Db_Header *fh, *new_fh;

	//---- read the network ----

	for (i=0; i < num_files; i++) {
		fh = file [i];
		if (fh == 0) continue;
		new_fh = new_file [i];

		Show_Message (String ("Reading %s -- Record") % fh->File_Type ());
		Set_Progress ();

		num_in = num_out = 0;

		while (fh->Read (false)) {
			Show_Progress ();
			num_in++;

			new_fh->Copy_Fields (*fh, false);

			if (!script_flag || program.Execute () != 0) {
				new_fh->Write (false);
				num_out++;
			}

			if (fh->Nest_Flag ()) {
				num = fh->Num_Nest ();

				for (n=0; n < num; n++) {
					Show_Progress ();
					num_in++;

					if (!fh->Read (true)) {
						Error ("Reading Nested Record");
					}
					new_fh->Copy_Fields (*fh, true);

					if (!script_flag || program.Execute () != 0) {
						new_fh->Write (true);
						num_out++;
					}
				}
			}
		}
		End_Progress ();
		fh->Close ();
		new_fh->Close ();

		//---- write summary summary ----

		if (i) Print (1);
		Break_Check (2);
		Print (1, String ("Number of %s Records = %d") % fh->File_Type () % num_in);
		Print (1, String ("Number of %s Records = %d") % new_fh->File_Type () % num_out);
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void FileFormat::Page_Header (void)
{
	switch (Header_Number ()) {
		case PRINT_SCRIPT:		//---- Conversion Script ----
			Print (1, "Conversion Script");
			Print (1);
			break;
		case PRINT_STACK:		//---- Conversion Stack ----
			Print (1, "Conversion Stack");
			Print (1);
			break;
		default:
			break;
	}
}
