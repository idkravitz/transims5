//*********************************************************
//	Link_Offset.cpp - link shape offset point
//*********************************************************

#include "Relocate.hpp"

#include <math.h>

//---------------------------------------------------------
//	Link_Offset
//---------------------------------------------------------

XYZ Relocate::Link_Offset (Link_Data *link_ptr, int dir, double offset, double side)
{
	int i, j, num_pts, node;
	double xa, ya, za, xb, yb, zb, factor, link_len, x1, y1, z1, x2, y2, z2;
	double dx, dy, dz, dx1, dy1, dz1, dx2, dy2, dz2, length;

	XYZ xyz, *pt_ptr;
	XYZ_Point point;
	Points_Itr point_itr;
	Node_Data *node_ptr;
	Shape_Data *shape_ptr;

	x1 = y1 = z1 = x2 = y2 = z2 = dx2 = dy2 = dz2 = 0.0;

	//---- get the link length and node coordinates ----

	link_len = UnRound (link_ptr->Length ());
	factor = 1.0;

	//---- check the offsets ----

	if (offset < 0.0) {
		offset = 0.0;
	} else if (offset > link_len) {
		offset = link_len;
	}

	//---- get the end nodes ----

	node = (dir == 1) ? link_ptr->Bnode () : link_ptr->Anode ();

	node_ptr = &old_node_array [node];

	xa = dat->UnRound (node_ptr->X ());
	ya = dat->UnRound (node_ptr->Y ());
	za = dat->UnRound (node_ptr->Z ());

	node = (dir == 1) ? link_ptr->Anode () : link_ptr->Bnode ();

	node_ptr = &old_node_array [node];

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

	//---- process offsets for a simple link ----

	if (link_ptr->Shape () < 0) {
		if (link_len > 0.0) {
			factor = length / link_len;
		}
		offset *= factor;

		xyz.x = Round (xa + offset * dx + side * dy);
		xyz.y = Round (ya + offset * dy - side * dx);
		xyz.z = Round (za + offset * dz);

		return (xyz);
	}

	//---- get the shape record ----

	points.clear ();	
	shape_ptr = &old_shape_array [link_ptr->Shape ()];
	
	//---- create the link vector ----

	num_pts = (int) shape_ptr->size () + 2;

	points.reserve (num_pts);

	j = (dir == 1) ? (int) shape_ptr->size () : 1;

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

				j += (dir == 1) ? -1 : 1;
			}
			points.push_back (point);
		}

	} else {
		dx1 = dy1 = dz1 = 0.0;

		for (i=1; i <= num_pts; i++) {
			if (i == 1) {
				x2 = xa;
				y2 = ya;
				z2 = z1;
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

					j += (dir == 1) ? -1 : 1;
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

	//---- find the offset point ----

	length = 0.0;

	for (i=0, point_itr = points.begin (); point_itr != points.end (); point_itr++, i++) {

		x2 = point_itr->x;
		y2 = point_itr->y;
		z2 = point_itr->z;

		if (i > 0) {
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

	factor = length / link_len;
	offset *= factor;
	length = 0.0;

	for (i=0, point_itr = points.begin (); point_itr != points.end (); point_itr++, i++) {

		x2 = point_itr->x;
		y2 = point_itr->y;
		z2 = point_itr->z;

		if (i > 0) {
			dx = x2 - x1;
			dy = y2 - y1;
			dz = z2 - z1;

			length += link_len = sqrt (dx * dx + dy * dy + dz * dz);

			if (length == offset) break;

			if (length > offset) {
				if (link_len > 0.0) {
					factor = (offset + link_len - length) / link_len;
				} else {
					factor = 1.0;
				}
				xyz.x = Round (x1 + dx * factor);
				xyz.y = Round (y1 + dy * factor);
				xyz.z = Round (z1 + dz * factor);
				return (xyz);
			}
		}
		x1 = x2;
		y1 = y2;
		z1 = z2;
	}
	xyz.x = Round (x2);
	xyz.y = Round (y2);
	xyz.z = Round (z2);
	return (xyz);
}
