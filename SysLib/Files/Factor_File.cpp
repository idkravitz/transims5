//*********************************************************
//	Factor_File.cpp - Factor File Input/Output
//*********************************************************

#include "Factor_File.hpp"

//---------------------------------------------------------
//	Factor_File constructor
//---------------------------------------------------------

Factor_File::Factor_File (Access_Type access, Format_Type format, Matrix_Type type, Units_Type od) :
	Matrix_File (access, format, type, od)
{
	Setup ();
}

Factor_File::Factor_File (string filename, Access_Type access, Format_Type format, Matrix_Type type, Units_Type od) : 
	Matrix_File (access, format, type, od)
{
	Setup ();

	Open (filename);
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Factor_File::Setup (void)
{
	Matrix_File::Setup ();

	File_Type ("Adjustment Factor File");
	File_ID ("Factor");
}


