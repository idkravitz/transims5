//*********************************************************
//	TcadMatrix.cpp - TransCAD Matrix DLL Interface
//*********************************************************

#include "TcadMatrix.hpp"

#include "MTXERR.H"

//---------------------------------------------------------
//	TransCAD_Matrix constructors
//---------------------------------------------------------

TransCAD_Matrix::TransCAD_Matrix (Access_Type access) : Db_Matrix (access, TRANSCAD)
{
	fh = 0;
}

//---------------------------------------------------------
//	Load_DLL
//---------------------------------------------------------

bool TransCAD_Matrix::Load_DLL (void)
{
	// disable transcad
	// tc_status = TC_OKAY;
	
	// InitMatDLL (&tc_status);

	return 0;//(tc_status == TC_OKAY);
}

//---------------------------------------------------------
//	Db_Open
//---------------------------------------------------------

bool TransCAD_Matrix::Db_Open (string filename)
{
	return (Status (NO_TRANSCAD));
	// if (!Load_DLL ()) 

	// if (File_Access () == CREATE) {
	// 	int i, len;
	// 	char **core_names;
	// 	Db_Field *fld_ptr;

	// 	if (Zones () == 0 || Tables () == 0) return (false);

	// 	core_names = new char * [Tables ()];

	// 	for (i=0; i < Tables (); i++) {
	// 		fld_ptr = Field (i);
	// 		len = (int) fld_ptr->Name ().length () + 1;
	// 		core_names [i] = new char [len];
	// 		strncpy (core_names [i], fld_ptr->Name ().c_str (), len);
	// 	}
	// 	fh = MATRIX_New ((char *) filename.c_str (), (char *) File_Type ().c_str (), 
	// 		Zones (), 0, Zones (), 0, Tables (), core_names, DOUBLE_TYPE, 1);
		
	// 	for (i=0; i < Tables (); i++) {
	// 		delete [] core_names [i];
	// 	}
	// 	delete [] core_names;

	// } else {
	// 	fh = MATRIX_LoadFromFile ((char *) filename.c_str (), CONTROL_AUTOMATIC);

	// 	if (tc_status != TC_OKAY || fh == 0) return (false);

	// 	Zones (MATRIX_GetNCols (fh));
	// 	Tables (MATRIX_GetNCores (fh));
	// }
	// return (true);
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool TransCAD_Matrix::Close (void)
{
	// if (fh != 0) {
		// MATRIX_Done (fh);
	// }
	return (Db_Matrix::Close ());
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool TransCAD_Matrix::Read (int org, int table, double *data)
{
	// MATRIX_SetCore (fh, (short) table);
	// MATRIX_GetBaseVector (fh, org, MATRIX_ROW, DOUBLE_TYPE, data);
	return 0;//(tc_status == TC_OKAY);
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool TransCAD_Matrix::Write (int org, int table, double *data)
{
	// MATRIX_SetCore (fh, (short) table);
	// MATRIX_SetBaseVector (fh, org, MATRIX_ROW, DOUBLE_TYPE, data);
	return 0;//(tc_status == TC_OKAY);
}

//---------------------------------------------------------
//	Read_Record
//---------------------------------------------------------

bool TransCAD_Matrix::Read_Record (int org)
{
	return (false);
	// if (Data () == 0) {
		// if (!Allocate_Data ()) return (false);
	// }
	// for (int i=0; i < Tables (); i++) {
		// MATRIX_SetCore (fh, (short) i);
		// MATRIX_GetBaseVector (fh, org, MATRIX_ROW, DOUBLE_TYPE, Data ()[i]);
		// if (tc_status != TC_OKAY) return (false);
	// }
	// return (true);
}

//---------------------------------------------------------
//	Write_Record
//---------------------------------------------------------

bool TransCAD_Matrix::Write_Record (int org)
{
	return (false);
	// if (Data () == 0) return (false);

	// for (int i=0; i < Tables (); i++) {
		// MATRIX_SetCore (fh, (short) i);
		// MATRIX_SetBaseVector (fh, org, MATRIX_ROW, DOUBLE_TYPE, Data ()[i]);
		// if (tc_status != TC_OKAY) return (false);
	// }
	// return (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool TransCAD_Matrix::Read_Header (bool stat)
{
	int i;
	char label [_MAX_FLABEL];
	String text;
	Integers decimals;
	Strings labels;

	//---- extract the table names ----

	// for (i=0; i < Tables (); i++) {
		// MATRIX_GetLabel (fh, (short) i, label);
		// text = label;
		// labels.push_back (text);
		// decimals.push_back (0);
	// }
	return (Table_Names (stat, labels, decimals));
}
