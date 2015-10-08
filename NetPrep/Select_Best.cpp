//*********************************************************
//	Select_Best.cpp - select the best of two cross streets
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Select_Best
//---------------------------------------------------------

bool NetPrep::Select_Best (Crossing_Data *s1, Crossing_Data *s2, Crossing_Data *o1, Crossing_Data *o2, bool set_flag)
{
	//---- select the best of 1 and 2 ----

	if (s1->flag && !s2->flag) return (true);
	if (s2->flag && !s1->flag) return (false);
	if (s1->end_flag && !s2->end_flag) return (true);
	if (s2->end_flag && !s1->end_flag) return (false);
	if (s1->length > 2 * s2->length) return (true);
	if (s2->length > 2 * s1->length) return (false);

	if (o1 != 0 && o2 != 0) {
		if (s1->node == o1->node && 
			s2->node == o2->node) {

			if (o1->end_flag && !o2->end_flag) {
				if (set_flag) o1->flag = true;
				return (true);
			}
			if (o2->end_flag && !o1->end_flag) {
				if (set_flag) o2->flag = true;
				return (false);
			}
			if ((s1->length + o1->length) > 
				(s2->length + o2->length)) {
				if (set_flag) o1->flag = true;
				return (true);
			}
			if (set_flag) o2->flag = true;
			return (false);
		}
		if (s1->node == o1->node) {
			if (set_flag) o1->flag = true;
			return (true);
		}
		if (s2->node == o2->node) {
			if (set_flag) o2->flag = true;
			return (false);
		}
	}
	if (s1->length > s2->length) return (true);
	return (false);
}
