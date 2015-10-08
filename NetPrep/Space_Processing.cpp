//*********************************************************
//	Space_Processing.cpp - process a link segment
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Space_Processing
//---------------------------------------------------------

int NetPrep::Space_Processing (Spacing_Data &spacing, bool flag)
{
	int link, at_len12, at_len23, num_drop;
	bool keep1;

	Dir_Data *dir_ptr;
	Int_Itr itr;

	num_drop = 0;

	//---- left side cross streets ----

	if (spacing.left3 != 0) {
		at_len12 = area_spacing.Best (spacing.left_at12);
		at_len23 = area_spacing.Best (spacing.left_at23);

		if (spacing.left_len12 >= at_len12 && spacing.left_len23 >= at_len23) {

			//---- save the local street ----

			for (itr = spacing.left2->links.begin (); itr != spacing.left2->links.end (); itr++) {
				dir_ptr = &dir_array [*itr];
				link = dir_ptr->Link ();

				if (select_links [link] == 0) select_links [link] = next_loop;
			}
			if (spacing.left1 != 0) {
				for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					if (select_links [link] == 0) select_links [link] = next_loop;
				}
			}

			//---- shift down ----

			delete spacing.left1;
			delete spacing.left2;

			spacing.left1 = 0;
			spacing.left2 = spacing.left3;
			spacing.left3 = 0;
			spacing.left_len12 = spacing.left_len23;
			spacing.left_at12 = spacing.left_at23;
			spacing.left_len23 = spacing.left_at23 = 0;

		} else if (spacing.left_len12 >= at_len12) {

			//---- save the end link ----

			if (spacing.left1 != 0) {
				for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					if (select_links [link] == 0) select_links [link] = next_loop;
				}
			}

			//---- shift down ----

			delete spacing.left1;

			spacing.left1 = spacing.left2;
			spacing.left2 = spacing.left3;
			spacing.left3 = 0;
			spacing.left_len12 = spacing.left_len23;
			spacing.left_at12 = spacing.left_at23;
			spacing.left_len23 = spacing.left_at23 = 0;

		} else if (spacing.left_len23 >= at_len23) {

			if (spacing.left1 != 0) {
				keep1 = Select_Best (spacing.left1, spacing.left2, spacing.right1, spacing.right2, true);

				if (keep1) {
					for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						if (select_links [link] == 0) select_links [link] = next_loop;
					}
					delete spacing.left1;
				}
			} else {
				keep1 = true;
			}
			if (keep1) {
				for (itr = spacing.left2->links.begin (); itr != spacing.left2->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					Delete_Link (link);
					select_links [link] = -1;
					num_drop++;
				}
				spacing.left_len23 += spacing.left_len12;
				delete spacing.left2;
			} else {
				for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					Delete_Link (link);
					select_links [link] = -1;
					num_drop++;
				}
				delete spacing.left1;
			}

			//---- shift down ----

			spacing.left1 = 0;
			spacing.left2 = spacing.left3;
			spacing.left3 = 0;
			spacing.left_len12 = spacing.left_len23;
			spacing.left_at12 = spacing.left_at23;
			spacing.left_len23 = spacing.left_at23 = 0;

		} else if (spacing.left1 != 0) {

			//---- delete the worst of 1, 2 and 3 ----

			keep1 = Select_Best (spacing.left1, spacing.left2, spacing.right1, spacing.right2);

			if (keep1) {
				keep1 = Select_Best (spacing.left2, spacing.left3, spacing.right2, spacing.right3);

				if (keep1) {
					for (itr = spacing.left3->links.begin (); itr != spacing.left3->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
					delete spacing.left3;
					spacing.left3 = 0;
				} else {
					for (itr = spacing.left2->links.begin (); itr != spacing.left2->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
					delete spacing.left2;

					spacing.left2 = spacing.left3;
					spacing.left3 = 0;
					spacing.left_len12 += spacing.left_len23;
					spacing.left_at12 = spacing.left_at23;
					spacing.left_len23 = spacing.left_at23 = 0;
				}
			} else {
				keep1 = Select_Best (spacing.left1, spacing.left3, spacing.right1, spacing.right3);

				if (keep1) {
					for (itr = spacing.left3->links.begin (); itr != spacing.left3->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
					delete spacing.left3;
					spacing.left3 = 0;
				} else {
					for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
					delete spacing.left1;

					spacing.left1 = spacing.left2;
					spacing.left2 = spacing.left3;
					spacing.left3 = 0;
					spacing.left_len12 = spacing.left_len23;
					spacing.left_at12 = spacing.left_at23;
					spacing.left_len23 = spacing.left_at23 = 0;
				}
			}

		} else if (spacing.left_len12 > 2 * spacing.left_len23) {

			//---- select the best of 2 and 3 ----

			keep1 = Select_Best (spacing.left2, spacing.left3, spacing.right2, spacing.right3);

			if (keep1) {
				for (itr = spacing.left3->links.begin (); itr != spacing.left3->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					Delete_Link (link);
					select_links [link] = -1;
					num_drop++;
				}
				delete spacing.left3;
				spacing.left3 = 0;
			} else {
				for (itr = spacing.left2->links.begin (); itr != spacing.left2->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					Delete_Link (link);
					select_links [link] = -1;
					num_drop++;
				}
				delete spacing.left2;

				spacing.left2 = spacing.left3;
				spacing.left3 = 0;
				spacing.left_len12 += spacing.left_len23;
				spacing.left_at12 = spacing.left_at23;
				spacing.left_len23 = spacing.left_at23 = 0;
			}

		} else {

			for (itr = spacing.left2->links.begin (); itr != spacing.left2->links.end (); itr++) {
				dir_ptr = &dir_array [*itr];
				link = dir_ptr->Link ();

				Delete_Link (link);
				select_links [link] = -1;
				num_drop++;
			}
			delete spacing.left2;

			spacing.left2 = spacing.left3;
			spacing.left3 = 0;
			spacing.left_len12 += spacing.left_len23;
			spacing.left_at12 = spacing.left_at23;
			spacing.left_len23 = spacing.left_at23 = 0;
		}
	}

	//---- right side cross streets ----

	if (spacing.right3 != 0) {
		at_len12 = area_spacing.Best (spacing.right_at12);
		at_len23 = area_spacing.Best (spacing.right_at23);

		if (spacing.right_len12 >= at_len12 && spacing.right_len23 >= at_len23) {

			//---- save the local street ----

			for (itr = spacing.right2->links.begin (); itr != spacing.right2->links.end (); itr++) {
				dir_ptr = &dir_array [*itr];
				link = dir_ptr->Link ();

				if (select_links [link] == 0) select_links [link] = next_loop;
			}
			if (spacing.right1 != 0) {
				for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					if (select_links [link] == 0) select_links [link] = next_loop;
				}
			}

			//---- shift down ----

			delete spacing.right1;
			delete spacing.right2;

			spacing.right1 = 0;
			spacing.right2 = spacing.right3;
			spacing.right3 = 0;
			spacing.right_len12 = spacing.right_len23;
			spacing.right_at12 = spacing.right_at23;
			spacing.right_len23 = spacing.right_at23 = 0;

		} else if (spacing.right_len12 >= at_len12) {

			//---- save the end link ----

			if (spacing.right1 != 0) {
				for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					if (select_links [link] == 0) select_links [link] = next_loop;
				}
			}

			//---- shift down ----

			delete spacing.right1;

			spacing.right1 = spacing.right2;
			spacing.right2 = spacing.right3;
			spacing.right3 = 0;
			spacing.right_len12 = spacing.right_len23;
			spacing.right_at12 = spacing.right_at23;
			spacing.right_len23 = spacing.right_at23 = 0;

		} else if (spacing.right_len23 >= at_len23) {

			if (spacing.right1 != 0) {
				keep1 = Select_Best (spacing.right1, spacing.right2);

				if (keep1) {
					for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						if (select_links [link] == 0) select_links [link] = next_loop;
					}
					delete spacing.right1;
				}
			} else {
				keep1 = true;
			}
			if (keep1) {
				for (itr = spacing.right2->links.begin (); itr != spacing.right2->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					Delete_Link (link);
					select_links [link] = -1;
					num_drop++;
				}
				spacing.right_len23 += spacing.right_len12;
				delete spacing.right2;
			} else {
				for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					Delete_Link (link);
					select_links [link] = -1;
					num_drop++;
				}
				delete spacing.right1;
			}

			//---- shift down ----

			spacing.right1 = 0;
			spacing.right2 = spacing.right3;
			spacing.right3 = 0;
			spacing.right_len12 = spacing.right_len23;
			spacing.right_at12 = spacing.right_at23;
			spacing.right_len23 = spacing.right_at23 = 0;

		} else if (spacing.right1 != 0) {

			//---- delete the worst of 1, 2 and 3 ----

			keep1 = Select_Best (spacing.right1, spacing.right2);

			if (keep1) {
				keep1 = Select_Best (spacing.right2, spacing.right3);

				if (keep1) {
					for (itr = spacing.right3->links.begin (); itr != spacing.right3->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
					delete spacing.right3;
					spacing.right3 = 0;
				} else {
					for (itr = spacing.right2->links.begin (); itr != spacing.right2->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
					delete spacing.right2;

					spacing.right2 = spacing.right3;
					spacing.right3 = 0;
					spacing.right_len12 += spacing.right_len23;
					spacing.right_at12 = spacing.right_at23;
					spacing.right_len23 = spacing.right_at23 = 0;
				}
			} else {
				keep1 = Select_Best (spacing.right1, spacing.right3);

				if (keep1) {
					for (itr = spacing.right3->links.begin (); itr != spacing.right3->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
					delete spacing.right3;
					spacing.right3 = 0;
				} else {
					for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
					delete spacing.right1;

					spacing.right1 = spacing.right2;
					spacing.right2 = spacing.right3;
					spacing.right3 = 0;
					spacing.right_len12 = spacing.right_len23;
					spacing.right_at12 = spacing.right_at23;
					spacing.right_len23 = spacing.right_at23 = 0;
				}
			}

		} else if (spacing.right_len12 > 2 * spacing.right_len23) {

			//---- select the best of 2 and 3 ----

			keep1 = Select_Best (spacing.right2, spacing.right3);

			if (keep1) {
				for (itr = spacing.right3->links.begin (); itr != spacing.right3->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					Delete_Link (link);
					select_links [link] = -1;
					num_drop++;
				}
				delete spacing.right3;
				spacing.right3 = 0;
			} else {
				for (itr = spacing.right2->links.begin (); itr != spacing.right2->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					Delete_Link (link);
					select_links [link] = -1;
					num_drop++;
				}
				delete spacing.right2;

				spacing.right2 = spacing.right3;
				spacing.right3 = 0;
				spacing.right_len12 += spacing.right_len23;
				spacing.right_at12 = spacing.right_at23;
				spacing.right_len23 = spacing.right_at23 = 0;
			}

		} else {

			for (itr = spacing.right2->links.begin (); itr != spacing.right2->links.end (); itr++) {
				dir_ptr = &dir_array [*itr];
				link = dir_ptr->Link ();

				Delete_Link (link);
				select_links [link] = -1;
				num_drop++;
			}
			delete spacing.right2;

			spacing.right2 = spacing.right3;
			spacing.right3 = 0;
			spacing.right_len12 += spacing.right_len23;
			spacing.right_at12 = spacing.right_at23;
			spacing.right_len23 = spacing.right_at23 = 0;
		}
	}

	//---- process the last link ----

	if (flag) {

		//---- left side ----

		if (spacing.left2 != 0) {
			at_len12 = area_spacing.Best (spacing.left_at12);
			if (spacing.left_len12 >= at_len12) {
				for (itr = spacing.left2->links.begin (); itr != spacing.left2->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					if (select_links [link] == 0) select_links [link] = next_loop;
				}
				if (spacing.left1 != 0) {
					for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						if (select_links [link] == 0) select_links [link] = next_loop;
					}
					delete spacing.left1;
				}
			} else if (spacing.left1 != 0) {
				keep1 = Select_Best (spacing.left1, spacing.left2, spacing.right1, spacing.right2);

				if (keep1) {
					for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						if (select_links [link] == 0) select_links [link] = next_loop;
					}
					for (itr = spacing.left2->links.begin (); itr != spacing.left2->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
				} else {
					for (itr = spacing.left2->links.begin (); itr != spacing.left2->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						if (select_links [link] == 0) select_links [link] = next_loop;
					}
					for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
				}
				delete spacing.left1;
			}
			delete spacing.left2;
		} else if (spacing.left1 != 0) {
			for (itr = spacing.left1->links.begin (); itr != spacing.left1->links.end (); itr++) {
				dir_ptr = &dir_array [*itr];
				link = dir_ptr->Link ();

				if (select_links [link] == 0) select_links [link] = next_loop;
			}
			delete spacing.left1;
		}

		//---- right side ----

		if (spacing.right2 != 0) {
			at_len12 = area_spacing.Best (spacing.right_at12);

			if (spacing.right_len12 >= at_len12) {
				for (itr = spacing.right2->links.begin (); itr != spacing.right2->links.end (); itr++) {
					dir_ptr = &dir_array [*itr];
					link = dir_ptr->Link ();

					if (select_links [link] == 0) select_links [link] = next_loop;
				}
				if (spacing.right1 != 0) {
					for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						if (select_links [link] == 0) select_links [link] = next_loop;
					}
					delete spacing.right1;
				}
			} else if (spacing.right1 != 0) {
				keep1 = Select_Best (spacing.right1, spacing.right2);

				if (keep1) {
					for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						if (select_links [link] == 0) select_links [link] = next_loop;
					}
					for (itr = spacing.right2->links.begin (); itr != spacing.right2->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
				} else {
					for (itr = spacing.right2->links.begin (); itr != spacing.right2->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						if (select_links [link] == 0) select_links [link] = next_loop;
					}
					for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
						dir_ptr = &dir_array [*itr];
						link = dir_ptr->Link ();

						Delete_Link (link);
						select_links [link] = -1;
						num_drop++;
					}
				}
				delete spacing.right1;
			}
			delete spacing.right2;
		} else if (spacing.right1 != 0) {
			for (itr = spacing.right1->links.begin (); itr != spacing.right1->links.end (); itr++) {
				dir_ptr = &dir_array [*itr];
				link = dir_ptr->Link ();

				if (select_links [link] == 0) select_links [link] = next_loop;
			}
			delete spacing.right1;
		}
	}
	return (num_drop);
}
