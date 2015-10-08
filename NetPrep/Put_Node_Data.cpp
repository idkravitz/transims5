//*********************************************************
//	Put_Node_Data.cpp - write a new node file
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Put_Node_Data
//---------------------------------------------------------

int NetPrep::Put_Node_Data (Node_File &file, Node_Data &data)
{
	if (data.Count () > 0) {
		return (Data_Service::Put_Node_Data (file, data));
	} else {
		return (0);
	}
}
