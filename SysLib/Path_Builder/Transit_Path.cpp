//*********************************************************
//	Transit_Path.cpp - Build a Transit Path between locations
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Transit_Path
//---------------------------------------------------------

int Path_Builder::Transit_Path (Path_End_Array *from_ptr, Path_End_Array *to_ptr, bool best_flag)
{
	Transit_Path_Index path_index;
	Transit_Sort_Itr sort_itr;

	//---- initialize the path tree ----

	Transit_Access (from_ptr, to_ptr, best_flag);

	//---- build the path legs ----

	for (;;) {
		
		//---- remove the approach data from the processing queue ----

		if (param.sort_method) {
			if (transit_sort.empty ()) break;
			sort_itr = transit_sort.begin ();
			path_index = sort_itr->second;
			transit_sort.erase (sort_itr);
		} else {
			if (next_index.empty ()) break;
			path_index = next_index.front ();
			next_index.pop_front ();
		}
		switch (path_index.Type ()) {
			case NODE_ID:
				Walk_Access (path_index, to_ptr, best_flag);
				break;
			case STOP_ID:
				Stop_Board (path_index, to_ptr, best_flag);
				break;
			case ROUTE_ID:
				Stop_Alight (path_index, to_ptr, best_flag);
				break;
		}
	}
	return (0);
}
