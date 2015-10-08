//*********************************************************
//	Put_Link_Data.cpp - write a new link file
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Put_Link_Data
//---------------------------------------------------------

int NetPrep::Put_Link_Data (Link_File &file, Link_Data &data)
{
	if (!drop_flag || data.Length () > 0) {
		return (Data_Service::Put_Link_Data (file, data));
	} else {
		return (0);
	}
}
