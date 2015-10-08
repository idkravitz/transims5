//*********************************************************
//	Db_Matrix.cpp - General Matrix DLL Interface
//*********************************************************

#include "Db_Matrix.hpp"

//---------------------------------------------------------
//	Db_Matrix constructors
//---------------------------------------------------------

Db_Matrix::Db_Matrix (Access_Type access, Format_Type format) : Db_Header (access, format)
{
	data = 0;
	zones = tables = 0;
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

bool Db_Matrix::Close (void)
{
	if (data != 0) {
		for (int i=0; i < tables; i++) {
			delete [] data [i];
		}
		delete [] data;
		data = 0;
	}
	return (true);
}

//---------------------------------------------------------
//	Read
//---------------------------------------------------------

bool Db_Matrix::Read (int org, int table, double *data)
{
	org = table = 0;
	memset (data, '\0', zones * sizeof (double));
	return (false);
}

//---------------------------------------------------------
//	Write
//---------------------------------------------------------

bool Db_Matrix::Write (int org, int table, double *data)
{
	org = table = (int) data [0];
	return (false);
}

//---------------------------------------------------------
//	Allocate_Data
//---------------------------------------------------------

bool Db_Matrix::Allocate_Data (void)
{
	if (data == 0) {
		data = new double * [tables];
		if (data == 0) return (false);

		for (int i=0; i < tables; i++) {
			data [i] = new double [zones];
			if (data [i] == 0) return (false);
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Zero_Data
//---------------------------------------------------------

void Db_Matrix::Zero_Data (void)
{
	if (data != 0) {
		for (int i=0; i < tables; i++) {
			memset (data [i], '\0', zones * sizeof (double));
		}
	}
}

//---------------------------------------------------------
//	Read_Record
//---------------------------------------------------------

bool Db_Matrix::Read_Record (int org)
{
	org = 0;
	return (false);
}

//---------------------------------------------------------
//	Write_Record
//---------------------------------------------------------

bool Db_Matrix::Write_Record (int org)
{
	org = 0;
	return (false);
}

//---------------------------------------------------------
//	Set_Fields
//---------------------------------------------------------

bool Db_Matrix::Set_Fields (int des)
{
	if (data == 0) return (false);

	double *field = (double *) Record ().Data ();
	des--;

	for (int i=0; i < tables; i++, field++) {
		*field = data [i] [des];
	}
	return (true);
}

//---------------------------------------------------------
//	Get_Fields
//---------------------------------------------------------

bool Db_Matrix::Get_Fields (int des)
{
	if (data == 0) return (false);

	double *field = (double *) Record ().Data ();
	des--;

	for (int i=0; i < tables; i++, field++) {
		data [i] [des] = *field;
	}
	return (true);
}

//-----------------------------------------------------------
//	Table_Names
//-----------------------------------------------------------

bool Db_Matrix::Table_Names (bool stat, Strings &labels, Integers &decimals)
{
	int i;
	double size;

	//---- validate the definition file ----

	if (stat) {
		if (Num_Fields () == tables && Num_Fields () > 0) {
			Db_Field *fld_ptr;

			for (i=0; i < tables; i++) {
				fld_ptr = Field (i);

				if (fld_ptr == 0 || !labels [i].Equals (fld_ptr->Name ())) {
					stat = false;
					break;
				}
			}
		} else {
			stat = false;
		}
	}

	//---- create fields and definition file ----

	if (!stat) {
		Clear_Fields ();

		for (i=0; i < tables; i++) {
			size = (double) sizeof (double) + (decimals [i] / 10.0);

			if (Add_Field (labels [i], DB_DOUBLE, size, NO_UNITS, true) < 0) {
				return (false);
			}
		}
		Write_Def_Header ("");
	}
	return (Record_Number (-1));
}
