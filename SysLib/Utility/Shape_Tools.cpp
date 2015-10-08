//*********************************************************
//	Data_Shapes.cpp - link and turn shape point methods
//*********************************************************

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Link_Shape
//---------------------------------------------------------

bool Link_Shape (Link_Data *link_ptr, int dir, Points &points, double offset, double length, double side)
{
	int i, j, num_pts, node;
	double xa, ya, za, xb, yb, zb, factor, link_len, x1, y1, z1, x2, y2, z2;
	double dx, dy, dz, dx1, dy1, dz1, dx2, dy2, dz2, end_offset;
	bool point_flag, flip_flag;

	XYZ *pt_ptr;
	XYZ_Point point;
	Points_Itr point_itr, output_itr;
	Node_Data *node_ptr;
	Shape_Data *shape_ptr;

	point_flag = (length == 0.0);
	flip_flag = (dir == 1);
	x1 = y1 = z1 = x2 = y2 = z2 = dx2 = dy2 = dz2 = 0.0;

	//---- get the link length and node coordinates ----

	link_len = dat->UnRound (link_ptr->Length ());
	factor = 1.0;

	//---- check the offsets ----

	if (offset < 0.0) offset = 0.0;
	if (length < 0.0) length = link_len;

	end_offset = offset + length;

	if (offset < 0.0) {
		offset = 0.0;
	} else if (offset > link_len) {
		offset = link_len;
	}
	if (end_offset < 0.0) {
		end_offset = 0.0;
	} else if (end_offset > link_len) {
		end_offset = link_len;
	}

	//---- get the end nodes ----

	node = (flip_flag) ? link_ptr->Bnode () : link_ptr->Anode ();

	node_ptr = &dat->node_array [node];

	xa = dat->UnRound (node_ptr->X ());
	ya = dat->UnRound (node_ptr->Y ());
	za = dat->UnRound (node_ptr->Z ());

	node = (flip_flag) ? link_ptr->Anode () : link_ptr->Bnode ();

	node_ptr = &dat->node_array [node];

	xb = dat->UnRound (node_ptr->X ());
	yb = dat->UnRound (node_ptr->Y ());
	zb = dat->UnRound (node_ptr->Z ());

	dx = xb - xa;
	dy = yb - ya;
	dz = zb - za;

	length = sqrt (dx * dx + dy * dy + dz * dz);
	if (length == 0.0) length = 0.01;

	dx /= length;
	dy /= length;
	dz /= length;

	//---- get the shape record ----

	if (dat->System_File_Flag (SHAPE) && link_ptr->Shape () >= 0) {
		shape_ptr = &dat->shape_array [link_ptr->Shape ()];
	} else {
		shape_ptr = 0;
	}
	points.clear ();

	//---- process offsets for a simple link ----

	if (shape_ptr == 0) {
		i = (point_flag) ? 1 : 2;
		points.reserve (i);

		if (link_len > 0.0) {
			factor = length / link_len;
		}
		offset *= factor;

		point.x = xa + offset * dx + side * dy;
		point.y = ya + offset * dy - side * dx;
		point.z = za + offset * dz;

		points.push_back (point);

		if (point_flag) return (true);

		end_offset *= factor;

		point.x = xa + end_offset * dx + side * dy;
		point.y = ya + end_offset * dy - side * dx;
		point.z = za + end_offset * dz;

		points.push_back (point);

		return (true);
	}

	//---- create the link vector ----

	num_pts = (int) shape_ptr->size () + 2;

	points.reserve (num_pts);

	j = (flip_flag) ? (int) shape_ptr->size () : 1;

	if (side == 0.0) {

		for (i=1; i <= num_pts; i++) {
			if (i == 1) {
				point.x = xa;
				point.y = ya;
				point.z = za;
			} else if (i == num_pts) {
				point.x = xb;
				point.y = yb;
				point.z = zb;
			} else {
				pt_ptr = &(shape_ptr->at (j-1));

				point.x = dat->UnRound (pt_ptr->x);
				point.y = dat->UnRound (pt_ptr->y);
				point.z = dat->UnRound (pt_ptr->z);

				j += (flip_flag) ? -1 : 1;
			}
			points.push_back (point);
		}

	} else {
		dx1 = dy1 = dz1 = 0.0;

		for (i=1; i <= num_pts; i++) {
			if (i == 1) {
				x2 = xa;
				y2 = ya;
				z2 = za;
				dx2 = dy2 = dz2 = 0.0;
			} else {
				if (i == num_pts) {
					x2 = xb;
					y2 = yb;
					z2 = zb;
				} else {
					pt_ptr = &(shape_ptr->at (j-1));

					x2 = dat->UnRound (pt_ptr->x);
					y2 = dat->UnRound (pt_ptr->y);
					z2 = dat->UnRound (pt_ptr->z);

					j += (flip_flag) ? -1 : 1;
				}
				dx2 = x2 - x1;
				dy2 = y2 - y1;
				dz2 = z2 - z1;

				length = sqrt (dx2 * dx2 + dy2 * dy2 + dz2 * dz2);

				if (length == 0.0) {
					dx2 = dx;
					dy2 = dy;
					dz2 = dz;
				} else {
					dx2 /= length;
					dy2 /= length;
					dz2 /= length;
				}
				if (i == 2) {
					dx1 = dx2;
					dy1 = dy2;
					dz1 = dz2;
				} else {
					dx1 = (dx1 + dx2) / 2.0;
					dy1 = (dy1 + dy2) / 2.0;
					dz1 = (dz1 + dz2) / 2.0;
				}
				point.x = x1 + side * dy1;
				point.y = y1 - side * dx1;
				point.z = z1;

				points.push_back (point);

				if (i == num_pts) {
					point.x = x2 + side * dy2;
					point.y = y2 - side * dx2;
					point.z = z2;

					points.push_back (point);
				}
			}
			x1 = x2;
			y1 = y2;
			z1 = z2;
			dx1 = dx2;
			dy1 = dy2;
			dz1 = dz2;
		}
	}

	//---- extract a subset of points ----

	if (offset != 0.0 || end_offset != link_len) {
		return (Sub_Shape (points, offset, end_offset - offset, link_len));
	} else {
		return (true);
	}
}


//---------------------------------------------------------
//	Turn_Shape
//---------------------------------------------------------

bool Turn_Shape (int dir_in, int dir_out, Points &points, double setback, bool curve_flag, double side_in, double side_out, double sub_off, double sub_len)
{
	int dir;
	double length, offset, max_len, off1, off2;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Points pts;
	Points_Itr pt_itr;

	points.clear ();
	max_len = 2.0 * setback;

	//---- approach leg ----

	dir_ptr = &dat->dir_array [dir_in];
	dir = dir_ptr->Dir ();

	link_ptr = &dat->link_array [dir_ptr->Link ()];

	if (dir == 0) {
		off1 = dat->UnRound (link_ptr->Boffset ());
		off2 = dat->UnRound (link_ptr->Aoffset ());
	} else {
		off1 = dat->UnRound (link_ptr->Aoffset ());
		off2 = dat->UnRound (link_ptr->Boffset ());
	}
	max_len += off1;

	length = dat->UnRound (link_ptr->Length ()) - off1;
	offset = length - setback;
	if (offset < off2) offset = off2;
	length -= offset;
	if (length < 0.0) length = 0.0;

	Link_Shape (link_ptr, dir, pts, offset, length, side_in);

	for (pt_itr = pts.begin (); pt_itr != pts.end (); pt_itr++) {
		points.push_back (*pt_itr);
	}

	//---- departure leg ----

	dir_ptr = &dat->dir_array [dir_out];
	dir = dir_ptr->Dir ();

	link_ptr = &dat->link_array [dir_ptr->Link ()];

	length = dat->UnRound (link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ());
	if (length < 0.0) length = 0.0;

	if (dir == 0) {
		off1 = dat->UnRound (link_ptr->Aoffset ());
	} else {
		off1 = dat->UnRound (link_ptr->Boffset ());
	}
	max_len += off1;

	offset = off1;
	if (length > setback) length = setback;

	Link_Shape (link_ptr, dir, pts, offset, length, side_out);

	//---- curve connection ----

	if (curve_flag) {
		Connection_Curve (points, pts);
	} else {
		for (pt_itr = pts.begin (); pt_itr != pts.end (); pt_itr++) {
			points.push_back (*pt_itr);
		}
	}

	//---- extract a subset of points ----

	if (sub_off < 0.0) sub_off = 0.0;
	if (sub_len < 0.0) sub_len = 0.0;

	if (sub_off != 0.0 || sub_len != 0.0) {
		return (Sub_Shape (points, sub_off, sub_len, max_len));
	} else {
		return (true);
	}
}

//---------------------------------------------------------
//	Sub_Shape
//---------------------------------------------------------

bool Sub_Shape (Points &points, double offset, double length, double max_len)
{
	double factor, x1, y1, z1, x2, y2, z2, dx, dy, dz, end_offset;
	bool start_flag, first_flag, point_flag;

	XYZ_Point point;
	Points_Itr point_itr, output_itr;

	x1 = y1 = z1 = x2 = y2 = z2 = 0.0;
	point_flag = (length == 0.0);
	end_offset = offset + length;

	//---- calculate shape length ----

	length = 0.0;
	first_flag = true;

	for (point_itr = points.begin (); point_itr != points.end (); point_itr++) {

		x2 = point_itr->x;
		y2 = point_itr->y;
		z2 = point_itr->z;

		if (first_flag) {
			first_flag = false;
		} else {
			dx = x2 - x1;
			dy = y2 - y1;
			dz = z2 - z1;

			length += sqrt (dx * dx + dy * dy + dz * dz);
		}
		x1 = x2;
		y1 = y2;
		z1 = z2;
	}

	//---- process offsets for a shape link ----

	if (length == 0.0) length = 0.01;
	if (max_len <= 0.0) max_len = length;

	if (offset < 0.0) {
		offset = 0.0;
	} else if (offset > max_len) {
		offset = max_len;
	}
	if (end_offset < 0.0) {
		end_offset = 0.0;
	} else if (end_offset > max_len) {
		end_offset = max_len;
	}
	factor = length / max_len;
	offset *= factor;
	end_offset *= factor;

	length = 0.0;
	start_flag = false;
	first_flag = true;

	for (output_itr = point_itr = points.begin (); point_itr != points.end (); point_itr++) {

		x2 = point_itr->x;
		y2 = point_itr->y;
		z2 = point_itr->z;

		if (first_flag) {
			first_flag = false;

			if (offset == 0.0) {
				start_flag = true;
				output_itr++;
			}
		} else {
			dx = x2 - x1;
			dy = y2 - y1;
			dz = z2 - z1;

			length += max_len = sqrt (dx * dx + dy * dy + dz * dz);

			if (!start_flag) {
				if (length >= offset) {
					if (length > offset) {
						if (max_len > 0.0) {
							factor = (offset + max_len - length) / max_len;
						} else {
							factor = 1.0;
						}
						point.x = x1 + dx * factor;
						point.y = y1 + dy * factor;
						point.z = z1 + dz * factor;
						*output_itr++ = point;
					} else {
						*output_itr++ = *point_itr;
					}
					start_flag = true;
					if (point_flag) break;
				}
			}
			if (start_flag) {
				if (length <= end_offset) {
					if (length == offset) break;
					*output_itr++ = *point_itr;
					if (length == end_offset) break;
				} else {
					if (max_len > 0.0) {
						factor = (end_offset + max_len - length) / max_len;
					} else {
						factor = 1.0;
					}
					point.x = x1 + dx * factor;
					point.y = y1 + dy * factor;
					point.z = z1 + dz * factor;
					*output_itr++ = point;
					break;
				}
			}
		}
		x1 = x2;
		y1 = y2;
		z1 = z2;
	}
	if (output_itr < points.end ()) {
		points.erase (output_itr, points.end ());
	}
	return (points.size () > 0);
}

//---------------------------------------------------------
//	Connection_Curve 
//---------------------------------------------------------

bool Connection_Curve (Points &in_pts, Points &out_pts)
{
	int i, dir, max_diff;
	XYZ_Point pt, pt2;
	Points_Itr pt_itr, p1, p2, p3, p4;
	double dx1, dy1, dz1, dx2, dy2, dz2, sum1, sum2, len1, len2, fac1, fac2, inc1, inc2, max_len;

	if (in_pts.size () < 2 || out_pts.size () < 2) goto output;

	//---- inbound slope ----

	p1 = in_pts.begin ();
	p2 = --in_pts.end ();

	dx1 = p2->x - p1->x;
	dy1 = p2->y - p1->y;

	if (dx1 == 0.0) dx1 = 0.0001;
	fac1 = dy1 / dx1;

	//---- outbound slope ----

	p3 = out_pts.begin ();
	p4 = --out_pts.end ();

	dx2 = p4->x - p3->x;
	dy2 = p4->y - p3->y;

	if (dx2 == 0.0) dx2 = 0.0001;
	fac2 = dy2 / dx2;

	//---- slope intercept point ----

	sum1 = fac1 - fac2;
	if (fabs (sum1) < 0.0001) goto output;

	pt.x = (p3->y - p2->y + p2->x * fac1 - p3->x * fac2) / sum1;
	pt.y = p2->y + (pt.x - p2->x) * fac1;
	pt.z = (p2->z + p3->z) / 2.0;

	max_len = 20.0 * dat->turn_shape_setback;
	max_diff = dat->compass.Num_Points () / 16;

	//---- inbound bearing and length to intercept ----

	dir = dat->compass.Direction (dx1, dy1);

	dx1 = pt.x - p2->x;
	dy1 = pt.y - p2->y;
	dz1 = pt.z - p2->z;

	if (dat->compass.Difference (dir, dat->compass.Direction (dx1, dy1)) > max_diff) goto output;
	
	len1 = sqrt (dx1 * dx1 + dy1 * dy1);
	if (len1 == 0.0 || len1 > max_len) goto output;

	//---- outbound bearing and length to intercept ----

	dir = dat->compass.Direction (dx2, dy2);

	dx2 = p3->x - pt.x;
	dy2 = p3->y - pt.y;
	dz2 = p3->z - pt.z;

	if (dat->compass.Difference (dir, dat->compass.Direction (dx2, dy2)) > max_diff) goto output;

	len2 = sqrt (dx2 * dx2 + dy2 * dy2);
	if (len2 == 0.0 || len2 > max_len) goto output;

	//---- calculate the increment factors ----

	sum1 = 0.0;

	for (i=1; i <= NUM_CURVE_POINTS; i++) {
		sum1 += i;
	}
	if (sum1 == 0.0) goto output;

	inc1 = len1 / sum1;
	inc2 = len2 / sum1;

	sum1 = sum2 = 0.0;
	pt2 = *p2;

	for (i=1; i < NUM_CURVE_POINTS; i++) {
		sum1 += (NUM_CURVE_POINTS + 1 - i) * inc1;
		sum2 += i * inc2;

		fac1 = sum1 / len1;
		fac2 = sum2 / len2;

		pt.x = pt2.x + dx1 * fac1 + dx2 * fac2;
		pt.y = pt2.y + dy1 * fac1 + dy2 * fac2;
		pt.z = pt2.z + dz1 * fac1 + dz2 * fac2;

		in_pts.push_back (pt);
	}

output:
	for (pt_itr = out_pts.begin (); pt_itr != out_pts.end (); pt_itr++) {
		in_pts.push_back (*pt_itr);
	}
	return (in_pts.size () > 0);
}

//---------------------------------------------------------
//	Smooth_Shape
//---------------------------------------------------------

bool Smooth_Shape (Points &points, int max_angle, int min_length)
{
	if (points.size () < 3) return (false);
	
	XYZ_Point pt, pt2;
	Points_Itr pt_itr, last_itr, p_itr;

	int loop, dir1, dir2, diff, max_angle15;
	double x1, y1, z1, x2, y2, z2, dx, dy, dz, len1, len2, x0, y0, z0, factor, min_len, min_len2;
	bool first_flag, second_flag, loop_flag, change_flag;

	Points current;

	Compass_Points compass (360);

	//---- calculate shape length ----

	if (max_angle < 5) max_angle = 5;
	if (min_length < 1) min_length = 1;

	min_len = min_length;
	min_length = dat->Round (min_len);

	max_angle15 = (int) (max_angle * 1.5);
	min_len2 = min_length * 2.0;

	change_flag = false;
	loop_flag = true;
	x1 = y1 = z1 = len1 = 0.0;
	dir1 = 0;

	for (loop=0; loop < 100 && loop_flag; loop++) {
		if (points.size () < 3) break;

		//---- copy the existing points ----

		current.clear ();

		current.swap (points);

		first_flag = second_flag = true;
		loop_flag = false;

		last_itr = --current.end ();

		for (pt_itr = current.begin (); pt_itr <= last_itr; pt_itr++) {

			x2 = pt_itr->x;
			y2 = pt_itr->y;
			z2 = pt_itr->z;

			if (first_flag) {
				first_flag = false;
				len2 = 0.0;
				dir2 = 0;
			} else {
				dx = x2 - x1;
				dy = y2 - y1;
				dz = z2 - z1;

				dir2 = compass.Direction (dx, dy);
				len2 = sqrt (dx * dx + dy * dy + dz * dz);

				//---- discard very short shapes ----

				if (len2 < min_len && pt_itr < last_itr) {

					continue;
				}

				if (second_flag) {
					second_flag = false;
				} else {
					diff = compass.Difference (dir1, dir2);

					//---- adjust the difference for short distances ----

					if (len2 < min_length) {
						if (len2 > 0.0) {
							diff = (int) (diff * min_length / len2);
						} else {
							diff = 360;
						}
					}

					//---- smooth large direction changes ----

					if (diff > max_angle) {

						loop_flag = change_flag = true;

						if (len2 < min_length && pt_itr < last_itr) {

							if (diff <= max_angle15) {

								//---- move the last point ----

								p_itr = --points.end ();

								p_itr->x = (x1 + x2) / 2.0;
								p_itr->y = y2 = (y1 + y2) / 2.0;
								p_itr->z = z2 = (z1 + z2) / 2.0;

								p_itr--;

								dx = x2 - p_itr->x;
								dy = y2 - p_itr->y;
								dz = z2 - p_itr->z;

								dir1 = compass.Direction (dx, dy);
								len1 = sqrt (dx * dx + dy * dy + dz * dz);
								x1 = x2;
								y1 = y2;
								z1 = z2;
							}
							continue;

						} else if (len1 < min_len2 || len2 < min_len2) {

							if (diff >= max_angle15) {

								//---- discard the last point ----

								p_itr = --points.end ();

								p_itr->x = pt_itr->x;
								p_itr->y = pt_itr->y;
								p_itr->z = pt_itr->z;

								p_itr--;

								dx = x2 - p_itr->x;
								dy = y2 - p_itr->y;
								dz = z2 - p_itr->z;

								dir1 = compass.Direction (dx, dy);
								len1 = sqrt (dx * dx + dy * dy + dz * dz);
								x1 = x2;
								y1 = y2;
								z1 = z2;
								continue;

							} else if (len1 >= min_len2) {

								//---- move the last point ----

								p_itr = points.end () - 2;

								x0 = p_itr->x;
								y0 = p_itr->y;
								z0 = p_itr->z;

								factor = min_length / len1;

								p_itr++;

								p_itr->x = x1 = x1 + (x0 - x1) * factor;
								p_itr->y = y1 = y1 + (y0 - y1) * factor;
								p_itr->z = z1 = z1 + (z0 - z1) * factor;

								dx = x2 - x1;
								dy = y2 - y1;
								dz = z2 - z1;

								dir2 = compass.Direction (dx, dy);
								len2 = sqrt (dx * dx + dy * dy + dz * dz);

							} else if (len2 >= min_len2) {

								//---- add a shape point ----

								factor = min_length / len2;
								len2 = len2 - min_length;

								pt.x = x1 + (x2 - x1) * factor;
								pt.y = y1 + (y2 - y1) * factor;
								pt.z = z1 + (z2 - z1) * factor;

								points.push_back (pt);

							} else {

								//---- move the last point ----

								p_itr = points.end () - 2;

								x0 = p_itr->x;
								y0 = p_itr->y;
								z0 = p_itr->z;

								p_itr = --points.end ();

								p_itr->x = x1 = (x0 + x2 + x1 + x1) / 4.0;
								p_itr->y = y1 = (y0 + y2 + y1 + y1) / 4.0;
								p_itr->z = z1 = (z0 + z2 + z1 + z1) / 4.0;

								dx = x2 - x1;
								dy = y2 - y1;
								dz = z2 - z1;

								dir2 = compass.Direction (dx, dy);
								len2 = sqrt (dx * dx + dy * dy + dz * dz);
							}

						} else {

							//---- round the shape ----

							p_itr = points.end () - 2;

							x0 = p_itr->x;
							y0 = p_itr->y;
							z0 = p_itr->z;

							factor = min_length / len1;

							p_itr++;

							p_itr->x = x1 + (x0 - x1) * factor;
							p_itr->y = y1 + (y0 - y1) * factor;
							p_itr->z = z1 + (z0 - z1) * factor;

							factor = min_length / len2;
							len2 = len2 - min_length;

							pt.x = x1 + (x2 - x1) * factor;
							pt.y = y1 + (y2 - y1) * factor;
							pt.z = z1 + (z2 - z1) * factor;

							points.push_back (pt);
						}
					}
				}
			}
			points.push_back (*pt_itr);

			x1 = x2;
			y1 = y2;
			z1 = z2;
			len1 = len2;
			dir1 = dir2;
		}
	}
	return (change_flag);
}

//---------------------------------------------------------
//	Link_Bearings
//---------------------------------------------------------

void Link_Bearings (Link_Data &link_rec, int &bearing_in, int &bearing_out)
{
	int xa, ya, xb, yb;
	double dx, dy;

	Node_Data *node_ptr;

	node_ptr = &dat->node_array [link_rec.Anode ()];

	xa = node_ptr->X ();
	ya = node_ptr->Y ();

	node_ptr = &dat->node_array [link_rec.Bnode ()];

	xb = node_ptr->X ();
	yb = node_ptr->Y ();

	dx = dy = 0;

	if (link_rec.Shape () >= 0 && 
		(dat->Bearing_Offset () + MAX (link_rec.Aoffset (), link_rec.Boffset ())) < link_rec.Length ()) {

		int i, num, offset, len1, len2, xy_len, x1, y1;

		XYZ *point;
		Shape_Data *shape_ptr;

		shape_ptr = &dat->shape_array [link_rec.Shape ()];
		num = (int) shape_ptr->size ();

		//---- from the A node ---

		offset = dat->Bearing_Offset () + link_rec.Aoffset ();

		len1 = xy_len = 0;
		x1 = xa;
		y1 = ya;

		for (i=0; i < num; i++) {
			point = &((*shape_ptr) [i]);

			dx = point->x - x1;
			dy = point->y - y1;

			xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
			len2 = len1 + xy_len;

			if (len2 > offset) break;

			len1 = len2;
			x1 = point->x;
			y1 = point->y;
		}
		if (i == num) {
			dx = xb - x1;
			dy = yb - y1;

			xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
			len2 = len1 + xy_len;
		}
		offset -= len1;
		if (xy_len == 0) xy_len = 1;

		dx = x1 + (dx * offset / xy_len) - xa;
		dy = y1 + (dy * offset / xy_len) - ya;

		bearing_in = dat->compass.Direction (dx, dy);
		
		//---- from the B node ----

		offset = dat->Bearing_Offset () + link_rec.Boffset ();

		len1 = xy_len = 0;
		x1 = xb;
		y1 = yb;

		for (i=num-1; i >= 0; i--) {
			point = &((*shape_ptr) [i]);

			dx = point->x - x1;
			dy = point->y - y1;

			xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
			len2 = len1 + xy_len;

			if (len2 > offset) break;

			len1 = len2;
			x1 = point->x;
			y1 = point->y;
		}
		if (i < 0) {
			dx = xa - x1;
			dy = ya - y1;

			xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
			len2 = len1 + xy_len;
		}
		offset -= len1;
		if (xy_len == 0) xy_len = 1;

		dx = x1 + (dx * offset / xy_len) - xb;
		dy = y1 + (dy * offset / xy_len) - yb;

		bearing_out = dat->compass.Direction (-dx, -dy);

	} else {
		dx = xb - xa;
		dy = yb - ya;

		bearing_in = bearing_out = dat->compass.Direction (dx, dy);
	}
}

//---------------------------------------------------------
//	X_Sort_Function
//---------------------------------------------------------

int X_Sort_Function (const void *rec1, const void *rec2)
{
	double r1, r2;

	r1 = *((double *) rec1);
	r2 = *((double *) rec2);

	if (r1 > r2) return (1);
	if (r1 < r2) return (-1);
	return (0);
}

//---------------------------------------------------------
//	In_Polygon
//---------------------------------------------------------

bool In_Polygon (Points &points, double x, double y)
{
	int num;
	double x1, y1, x2, y2, xlist [100];
	bool first, close_flag;

	Points_Itr point_itr, end_itr;
	
	if (points.size () < 3) return (false);

	num = 0;
	first = true;
	close_flag = false;
	end_itr = --points.end ();
	x1 = y1 = 0.0;

	for (point_itr = points.begin (); num < 100; point_itr++) {
		if (point_itr != points.end ()) {
			x2 = point_itr->x;
			y2 = point_itr->y;
		} else {
			if (!close_flag) break;
			x2 = end_itr->x;
			y2 = end_itr->y;
			close_flag = false;
		}
		if (first) {
			first = false;
			close_flag = !(end_itr->x == x2 && end_itr->y == y2);
			end_itr = point_itr;
		} else {
			if (y1 == y2) {
				if (y == y1) {
					if (x1 < x2) {
						if (x1 <= x && x <= x2) return (true);
					} else {
						if (x2 <= x && x <= x1) return (true);
					}
				}
			} else if (y1 < y2) {
				if (y1 <= y && y < y2) {
					xlist [num++] = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
				}
			} else {
				if (y2 < y && y <= y1) {
					xlist [num++] = x2 + (y - y2) * (x1 - x2) / (y1 - y2);
				}
			}
		}
		x1 = x2;
		y1 = y2;
		if (point_itr == points.end ()) break;
	}
	if (num < 2) return (false);

	if (num == 2) {
		if (xlist [0] < xlist [1]) {
			if (xlist [0] <= x && x <= xlist [1]) return (true);
		} else {
			if (xlist [1] <= x && x <= xlist [0]) return (true);
		}
	} else {
		qsort (xlist, num, sizeof (double), X_Sort_Function);

		for (int i=0; i < num; i+=2) {
			if (xlist [i] <= x && x <= xlist [i+1]) return (true);
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Vehicle_Shape
//---------------------------------------------------------

bool Vehicle_Shape (XYZ_Point p1, XYZ_Point p2, double width, Points &points, bool front_flag)
{
	double x0, y0, z0, dx, dy, dz, length;

	XYZ_Point p;

	width /= 2.0;

	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	dz = p2.z - p1.z;

	length = sqrt (dx * dx + dy * dy + dz * dz);
	if (length == 0.0) length = 0.01;

	dx /= length;
	dy /= length;
	dz /= length;

	if (front_flag) {
		x0 = p1.x + 1.5 * dx;
		y0 = p1.y + 1.5 * dy;
		z0 = p1.z + 1.5 * dz;
	} else {
		x0 = p1.x;
		y0 = p1.y;
		z0 = p1.z;
	}
	points.clear ();
	points.push_back (p1);

	p.x = x0 - width * dy;
	p.y = y0 + width * dx;
	p.z = z0;

	points.push_back (p);

	p.x = p2.x - width * dy;
	p.y = p2.y + width * dx;
	p.z = p2.z;

	points.push_back (p);

	p.x = p2.x + width * dy;
	p.y = p2.y - width * dx;
	p.z = p2.z;

	points.push_back (p);

	p.x = x0 + width * dy;
	p.y = y0 - width * dx;
	p.z = z0;

	points.push_back (p);
	points.push_back (p1);

	return (points.size () == 6);
}

//---------------------------------------------------------
//	Vehicle_Shape
//---------------------------------------------------------

bool Vehicle_Shape (Points &pts, double width, Points &points, bool front_flag)
{
	if (pts.size () == 2) {
		return (Vehicle_Shape (pts [0], pts [1], width, points));
	}
	int i, num_pts;
	double x0, y0, z0, length, dx1, dy1, dx2, dy2;

	XYZ_Point p, p1, p2;

	num_pts = (int) pts.size ();
	width /= 2.0;

	points.clear ();
	p1 = pts [0];
	p2 = pts [1];

	points.push_back (p1);

	dx1 = p2.x - p1.x;
	dy1 = p2.y - p1.y;

	length = sqrt (dx1 * dx1 + dy1 * dy1);

	if (length > 0) {
		dx1 /= length;
		dy1 /= length;
	}
	if (front_flag) {
		x0 = p1.x + 1.5 * dx1;
		y0 = p1.y + 1.5 * dy1;
		z0 = p1.z;
	} else {
		x0 = p1.x;
		y0 = p1.y;
		z0 = p1.z;
	}
	p.x = x0 - width * dy1;
	p.y = y0 + width * dx1;
	p.z = z0;

	points.push_back (p);

	x0 += width * dy1;
	y0 -= width * dx1;
	dx2 = dy2 = 0;

	for (i=2; i < num_pts; i++) {
		p1 = p2;
		p2 = pts [i];

		dx2 = p2.x - p1.x;
		dy2 = p2.y - p1.y;

		length = sqrt (dx2 * dx2 + dy2 * dy2);

		if (length > 0) {
			dx2 /= length;
			dy2 /= length;
		}
		dx1 = (dx1 + dx2) / 2.0;
		dy1 = (dy1 + dy2) / 2.0;

		p.x = p1.x - width * dy1;
		p.y = p1.y + width * dx1;
		p.z = p1.z;
			
		points.push_back (p);

		dx1 = dx2;
		dy1 = dy2;
	}
	p.x = p2.x - width * dy2;
	p.y = p2.y + width * dx2;
	p.z = p2.z;

	points.push_back (p);

	p.x = p2.x + width * dy2;
	p.y = p2.y - width * dx2;
	p.z = p2.z;

	points.push_back (p);

	p2 = pts [num_pts - 2];
	dx1 = -dx2;
	dy1 = -dy2;

	for (i=num_pts - 3; i >= 0; i--) {
		p1 = p2;
		p2 = pts [i];

		dx2 = p2.x - p1.x;
		dy2 = p2.y - p1.y;

		length = sqrt (dx2 * dx2 + dy2 * dy2);

		if (length > 0) {
			dx2 /= length;
			dy2 /= length;
		}
		dx1 = (dx1 + dx2) / 2.0;
		dy1 = (dy1 + dy2) / 2.0;

		p.x = p1.x - width * dy1;
		p.y = p1.y + width * dx1;
		p.z = p1.z;
			
		points.push_back (p);

		dx1 = dx2;
		dy1 = dy2;
	}
	p.x = x0;
	p.y = y0;
	p.z = z0;

	points.push_back (p);
	points.push_back (pts [0]);

	return ((int) points.size () == (num_pts * 2 + 2));
}

//---------------------------------------------------------
//	Shift_Shape
//---------------------------------------------------------

bool Shift_Shape (Points &points, double side, int dir)
{
	int i, j, num_pts;
	double x1, y1, z1, x2, y2, z2, dx, dy, dz, dx1, dy1, dz1, dx2, dy2, dz2, length;
	bool flip_flag;

	XYZ_Point point, *pt_ptr, *first_ptr, *last_ptr;
	Points pts;

	flip_flag = (dir == 1);
	x1 = y1 = z1 = x2 = y2 = z2 = dx1 = dy1 = dz1 = dx2 = dy2 = dz2 = 0.0;

	//---- identify the overall orientation of the link ----

	num_pts = (int) points.size () - 1;
	if (num_pts < 1) return (false);

	if (flip_flag) {
		first_ptr = &points [num_pts];
		last_ptr = &points [0];
	} else {
		first_ptr = &points [0];
		last_ptr = &points [num_pts];
	}
	dx = last_ptr->x - first_ptr->x;
	dy = last_ptr->y - first_ptr->y;
	dz = last_ptr->z - first_ptr->z;

	length = sqrt (dx * dx + dy * dy + dz * dz);
	if (length == 0.0) length = 0.01;

	dx /= length;
	dy /= length;
	dz /= length;

	//---- shift a simple line ----

	if (num_pts == 1) {
		point.x = first_ptr->x + side * dy;
		point.y = first_ptr->y - side * dx;
		point.z = first_ptr->z;

		pts.push_back (point);

		point.x = last_ptr->x + side * dy;
		point.y = last_ptr->y - side * dx;
		point.z = last_ptr->z;

		pts.push_back (point);

	} else {

		//---- shift a series of points ----

		j = (flip_flag) ? num_pts : 0;

		for (i=0; i <= num_pts; i++) {
			pt_ptr = &points [j];

			x2 = pt_ptr->x;
			y2 = pt_ptr->y;
			z2 = pt_ptr->z;

			if (i == 0) {
				dx2 = dy2 = dz2 = 0.0;
			} else {
				dx2 = x2 - x1;
				dy2 = y2 - y1;
				dz2 = z2 - z1;

				length = sqrt (dx2 * dx2 + dy2 * dy2 + dz2 * dz2);

				if (length == 0.0) {
					dx2 = dx;
					dy2 = dy;
					dz2 = dz;
				} else {
					dx2 /= length;
					dy2 /= length;
					dz2 /= length;
				}
				if (i == 1) {
					dx1 = dx2;
					dy1 = dy2;
					dz1 = dz2;
				} else {
					dx1 = (dx1 + dx2) / 2.0;
					dy1 = (dy1 + dy2) / 2.0;
					dz1 = (dz1 + dz2) / 2.0;
				}
				point.x = x1 + side * dy1;
				point.y = y1 - side * dx1;
				point.z = z1;

				pts.push_back (point);

				if (i == num_pts) {
					point.x = x2 + side * dy2;
					point.y = y2 - side * dx2;
					point.z = z2;

					pts.push_back (point);
				}
			}
			x1 = x2;
			y1 = y2;
			z1 = z2;
			dx1 = dx2;
			dy1 = dy2;
			dz1 = dz2;

			j += (flip_flag) ? -1 : 1;
		}
	}

	//---- copy the points to the output list ----

	points.swap (pts);
	return (true);
}
