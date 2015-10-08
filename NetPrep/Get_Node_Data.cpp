//*********************************************************
//	Get_Node_Data.cpp - additonal node processing
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Get_Node_Data
//---------------------------------------------------------

bool NetPrep::Get_Node_Data (Node_File &file, Node_Data &data)
{
	if (Data_Service::Get_Node_Data (file, data)) {
		if (data.Node () >= new_node) new_node = data.Node () + 1;

		if (drop_node_flag && drop_node_range.In_Range (data.Node ())) return (false);

		data.Count (0);
		return (true);
	}
	return (false);
}
