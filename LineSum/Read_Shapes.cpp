//*********************************************************
//	Read_Shapes.cpp - read the link shapes
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Read_Shapes
//---------------------------------------------------------

void LineSum::Read_Shapes (void)
{
	int anode, bnode;

	//---- read the arcview shape file ----

	Show_Message (String ("Reading %s -- Record") % link_shape.File_Type ());
	Set_Progress ();

	//---- read each record ----

	while (link_shape.Read_Record ()) {
		Show_Progress ();

		anode = link_shape.Get_Integer (anode_field);
		bnode = link_shape.Get_Integer (bnode_field);

		points_map.insert (I2_Points_Map_Data (Int2_Key (anode, bnode), link_shape));
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % link_shape.File_Type () % points_map.size ());
	
	link_shape.Close ();
}
