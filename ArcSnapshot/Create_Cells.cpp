//*********************************************************
//	Create_Cells.cpp - allocate cells for each link
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Create_Cells
//---------------------------------------------------------

void ArcSnapshot::Create_Cells (void)
{
	int i, dir, lanes, cells, size;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Cell_Data *cell_ptr, cell_rec;

	cell_array.assign (dir_array.size (), cell_rec);

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		if (link_itr->Use () == 0) continue;

		cells = (int) (UnRound (link_itr->Length ()) / cell_size + 0.5);

		for (i=0; i < 2; i++) {
			if (i == 0) {
				dir = link_itr->AB_Dir ();
			} else {
				dir = link_itr->BA_Dir ();
			}
			if (dir == 0) continue;

			dir_ptr = &dir_array [dir];
			if (dir_ptr == 0) continue;

			cell_ptr = &cell_array [dir];
			if (cell_ptr == 0) continue;

			cell_ptr->lanes = lanes = dir_ptr->Lanes () + dir_ptr->Right () + dir_ptr->Left ();
			cell_ptr->cells = cells;
			cell_ptr->total = 0;

			size = lanes * cells;

			cell_ptr->data.assign (size, 0);

			if (method == MAXIMUM) {
				cell_ptr->max_total = 0;
				cell_ptr->maximum.assign (size, 0);
			}
		}
	}
}
