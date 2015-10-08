//*********************************************************
//	TppMatrix.cpp - TPPlus Matrix DLL Interface
//*********************************************************

#include "TppMatrix.hpp"

// #include <windows.h>

//---- matrix types ----

#define TPP      1
#define MINUTP   2
#define TPLAN    3
#define TRIPS    5

//---- license types ----

#define VIPER    1
#define CUBE     2
#define CUBEBASE 3
#define VOYAGER  4
#define CARGO    5

//---- operation codes ----

#define READ_HEADER  1
#define READ_DATA    2
#define SKIP_DATA    -2
#define READ_BOTH    3

TPPlus_Matrix::pf_FileInquire       TPPlus_Matrix::FileInquire = 0;
TPPlus_Matrix::pf_TppMatOpenIP      TPPlus_Matrix::TppMatOpenIP = 0;
TPPlus_Matrix::pf_TppMatOpenOP      TPPlus_Matrix::TppMatOpenOP = 0;
TPPlus_Matrix::pf_TppMatMatSet      TPPlus_Matrix::TppMatMatSet = 0;
TPPlus_Matrix::pf_TppMatClose       TPPlus_Matrix::TppMatClose = 0;

TPPlus_Matrix::pf_TppMatReadSelect  TPPlus_Matrix::TppMatReadSelect = 0;
TPPlus_Matrix::pf_TppMatReadNext    TPPlus_Matrix::TppMatReadNext = 0;
TPPlus_Matrix::pf_TppMatReadDirect  TPPlus_Matrix::TppMatReadDirect = 0;
TPPlus_Matrix::pf_TppMatMatWriteRow TPPlus_Matrix::TppMatMatWriteRow = 0;

TPPlus_Matrix::pf_TppMatPos         TPPlus_Matrix::TppMatPos = 0;
TPPlus_Matrix::pf_TppMatGetPos      TPPlus_Matrix::TppMatGetPos = 0;
TPPlus_Matrix::pf_TppMatSeek        TPPlus_Matrix::TppMatSeek = 0;

TPPlus_Matrix::pf_TppMatMatResize   TPPlus_Matrix::TppMatMatResize = 0;

//---------------------------------------------------------
//	TPPlus_Matrix constructors
//---------------------------------------------------------

TPPlus_Matrix::TPPlus_Matrix (Access_Type access) : Db_Matrix (access, TPPLUS)
{
	fh = 0;
}

//---------------------------------------------------------
//	Load_DLL
//---------------------------------------------------------

bool TPPlus_Matrix::Load_DLL (void)
{
	if (FileInquire == 0) {
		// HMODULE hMod;

		// hMod = LoadLibrary ("tppdlibx.dll");

		// FileInquire       = (pf_FileInquire) GetProcAddress (hMod, "FileInquire");
		// TppMatOpenIP      = (pf_TppMatOpenIP) GetProcAddress (hMod, "TppMatOpenIP");
		// TppMatOpenOP      = (pf_TppMatOpenOP) GetProcAddress (hMod, "TppMatOpenOP");
		// TppMatMatSet      = (pf_TppMatMatSet) GetProcAddress (hMod, "TppMatSet");
		// TppMatClose       = (pf_TppMatClose) GetProcAddress (hMod, "TppMatClose");

		// TppMatReadSelect  = (pf_TppMatReadSelect) GetProcAddress (hMod, "TppMatReadSelect");
		// TppMatReadNext    = (pf_TppMatReadNext) GetProcAddress (hMod, "TppMatReadNext");
		// TppMatReadDirect  = (pf_TppMatReadDirect) GetProcAddress (hMod, "TppMatReadDirect");
		// TppMatMatWriteRow = (pf_TppMatMatWriteRow) GetProcAddress (hMod, "TppMatWriteRow");
	    // 
		// TppMatPos         = (pf_TppMatPos) GetProcAddress (hMod, "TppMatPos");
		// TppMatGetPos      = (pf_TppMatGetPos) GetProcAddress (hMod, "TppMatGetPos");
		// TppMatSeek        = (pf_TppMatSeek) GetProcAddress (hMod, "TppMatSeek");

		// TppMatMatResize   = (pf_TppMatMatResize) GetProcAddress (hMod, "TppMatResize");

		if (FileInquire == 0) return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Db_Open
//---------------------------------------------------------

bool TPPlus_Matrix::Db_Open (string filename)
{
	int i, type;
    char *license = 0;

	if (!Load_DLL ()) return (Status (NO_TPPLUS));

	if (File_Access () == CREATE) {
		int len;
		unsigned char *header;
		Db_Field *fld_ptr;

		if (Zones () == 0 || Tables () == 0) return (false);

		if (Dbase_Format () == TRANPLAN) {
			type = TPLAN;
		} else {
			type = TPP;
		}
		TppMatMatSet (&fh, type, (char *) filename.c_str (), Zones (), Tables ());

		header = fh->Mnames;

		for (i=0; i < Tables (); i++) {
			fld_ptr = Field (i);
			if (fld_ptr->Decimal () == 0) {
				fh->Mspecs [i] = 'D';
			} else {
				fh->Mspecs [i] = (unsigned char) fld_ptr->Decimal ();
			}
			len = (int) fld_ptr->Name ().length () + 1;
			memcpy (header, fld_ptr->Name ().c_str (), len);
			header += len;
		}
		if ((i = TppMatOpenOP (fh, (char *) File_ID ().c_str (), (char *) exe->Program (), 0, license, CUBE)) <= 0) {
			if (i < -1) {
				return (Status (TPPLUS_LICENSE));
			} else {
				return (false);
			}
		}
		TppMatMatResize (&fh);
	} else {
		if (FileInquire ((char *) filename.c_str (), &fh) <= 0) return (false);

		if ((i = TppMatOpenIP (fh, license, CUBE)) <= 0) {
			if (i < -1) {
				return (Status (TPPLUS_LICENSE));
			} else {
				return (false);
			}
		}
		Zones (fh->zones);
		Tables (fh->mats);
	}
	fh->buffer = (void *) new char [fh->bufReq];

	return (true);
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool TPPlus_Matrix::Close (void)
{
	if (fh != 0) {
		delete [] fh->buffer;

	    TppMatClose (fh);
		fh = 0;
	}
	return (Db_Matrix::Close ());
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool TPPlus_Matrix::Read (int org, int table, double *data)
{
	return (TppMatReadSelect (fh, org, table, data) == 1);
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool TPPlus_Matrix::Write (int org, int table, double *data)
{
	return (TppMatMatWriteRow (fh, org, table, fh->Mspecs [table-1], data) == 1);
}

//---------------------------------------------------------
//	Read_Record
//---------------------------------------------------------

bool TPPlus_Matrix::Read_Record (int org)
{
	if (Data () == 0) {
		if (!Allocate_Data ()) return (false);
	}
	for (int i=0; i < Tables (); i++) {
		if (TppMatReadSelect (fh, org, i+1, Data ()[i]) != 1) return (false);
	}
	return (true);
}

//---------------------------------------------------------
//	Write_Record
//---------------------------------------------------------

bool TPPlus_Matrix::Write_Record (int org)
{
	if (Data () == 0) return (false);

	for (int i=0; i < Tables (); i++) {
		if (TppMatMatWriteRow (fh, org, i+1, fh->Mspecs [i], Data ()[i]) != 1) return (false);
	}
	return (true);
}

//-----------------------------------------------------------
//	Read_Header
//-----------------------------------------------------------

bool TPPlus_Matrix::Read_Header (bool stat)
{
	int i;
	char *header;
	String label;
	Integers decimals;
	Strings labels;

	//---- extract the table names ----

	header = (char *) fh->Mnames;

	for (i=0; i < Tables (); i++) {
		label = header;
		if (label.empty ()) {
			header++;
			label ("%d") % (i + 1);
		} else {
			header += label.size () + 1;
		}
		labels.push_back (label);
		if (fh->Mspecs [i] == 'D' || fh->Mspecs [i] == 'S') {
			decimals.push_back (0);
		} else {
			decimals.push_back ((int) fh->Mspecs [i]);
		}
	}
	return (Table_Names (stat, labels, decimals));
}
