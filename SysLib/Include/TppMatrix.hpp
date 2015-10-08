//*********************************************************
//	TppMatrix.hpp - TPPlus Matrix DLL Interface
//*********************************************************

#ifndef TPPMATRIX_HPP
#define TPPMATRIX_HPP

#include "APIDefs.hpp"
#include "Db_Matrix.hpp"

//---------------------------------------------------------
//	TPPlus_Matrix Class definition
//---------------------------------------------------------

class SYSLIB_API TPPlus_Matrix : public Db_Matrix
{
public:
	TPPlus_Matrix (Access_Type access = READ);

	virtual bool Db_Open (string filename);
	virtual bool Is_Open (void)        { return (fh != 0); }
	virtual bool Close (void);
	
	virtual bool Read_Record (int org = 0);
	virtual bool Write_Record (int org = 0);

	virtual bool Read (int org, int table, double *data);
	virtual bool Write (int org, int table, double *data);

private:
	virtual bool Load_DLL (void);
	
	virtual bool Read_Header (bool stat);

#pragma pack (push, 1)
	typedef struct {
			unsigned short  length;    // can be resized to this length
			unsigned char   type;      // type of file (1-2-3 = TPP, MINUTP, TRANPLAN)
			unsigned char   dummy;     //
			FILE *          ptr;       // pointer
			unsigned char   start;     // required for use with TRANPLAN I/P
			unsigned char   remain;    // required for use with TRANPLAN I/P
			unsigned char   rowStart;  // required for use with TRANPLAN I/P
			unsigned char   rowRemain; // required for use with TRANPLAN I/P
			unsigned short  zones;     // number of zones
			unsigned short  mats;      // number of matrices per zone
			unsigned short  Zones;     // system zones
			unsigned long   row0pos;   // location to position to row 1 for TPP
			unsigned long   rowpos;    // position where this row was located on file
			unsigned short  rowWords;  // header of current row
			unsigned short  rowOrg;    // header of current row
			unsigned short  rowMat;    // header of current row
			double          rowsum;    // for Output
			unsigned long   bufReq;    //
			char *          FileName;  // pointer to filename
			unsigned char * Mspecs;    // pointer to varBytes;
			unsigned char * Mnames;    // pointer to varNames;
			void *          buffer;    // location of required buffer (i/o & work)
	} TppMatrix;
#pragma pack (pop)

	typedef int ( __cdecl *pf_FileInquire)  (char *filename, TppMatrix **fh);
	typedef int ( __cdecl *pf_TppMatOpenIP) (TppMatrix *fh, char *path, int type);
	typedef int ( __cdecl *pf_TppMatOpenOP) (TppMatrix *fh, char *label, char *program, void *time, char *license, int type);
	typedef int ( __cdecl *pf_TppMatMatSet) (TppMatrix **fh, int type, char *name, int zones, int tables);
	typedef int ( __cdecl *pf_TppMatClose)  (TppMatrix *fh);

	typedef int ( __cdecl *pf_TppMatReadSelect)  (TppMatrix *fh, int org, int table, void *data);
	typedef int ( __cdecl *pf_TppMatReadDirect)  (TppMatrix *fh, unsigned long location, void *matrix);
	typedef int ( __cdecl *pf_TppMatReadNext)    (int op_code, TppMatrix *fh, void *data);
	typedef int ( __cdecl *pf_TppMatMatWriteRow) (TppMatrix *fh, int org, int table, int format, void *data);

	typedef int ( __cdecl *pf_TppMatPos)    (TppMatrix *fh, unsigned long offset);
	typedef int ( __cdecl *pf_TppMatGetPos) (TppMatrix *fh);
	typedef int ( __cdecl *pf_TppMatSeek)   (TppMatrix *fh, long offset, int base);

	typedef TppMatrix * ( __cdecl *pf_TppMatMatResize) (TppMatrix **fh);

	static pf_FileInquire       FileInquire;
	static pf_TppMatOpenIP      TppMatOpenIP;
	static pf_TppMatOpenOP      TppMatOpenOP;
	static pf_TppMatMatSet      TppMatMatSet;
	static pf_TppMatClose       TppMatClose;

	static pf_TppMatReadSelect  TppMatReadSelect;
	static pf_TppMatReadNext    TppMatReadNext;
	static pf_TppMatReadDirect  TppMatReadDirect;
	static pf_TppMatMatWriteRow TppMatMatWriteRow;

	static pf_TppMatPos         TppMatPos;
	static pf_TppMatGetPos      TppMatGetPos;
	static pf_TppMatSeek        TppMatSeek;

	static pf_TppMatMatResize   TppMatMatResize;

	TppMatrix *fh;
};
#endif
