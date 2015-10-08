//*********************************************************
//	Shape_Tools.hpp - network shape functions
//*********************************************************

#ifndef SHAPE_TOOLS_HPP
#define SHAPE_TOOLS_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Data_Service.hpp"

bool SYSLIB_API Link_Shape (Link_Data *link_ptr, int dir, Points &points, double offset = -1.0, double length = -1.0, double side = 0.0);
bool SYSLIB_API Turn_Shape (int dir_in, int dir_out, Points &points, double setback = 0.0, bool curve_flag = false, double side_in = 0.0, double side_out = 0.0, double offset = -1.0, double length = -1.0);
bool SYSLIB_API Sub_Shape (Points &points, double offset, double length, double max_len = 0.0);
bool SYSLIB_API Connection_Curve (Points &in_pts, Points &out_pts);
bool SYSLIB_API Smooth_Shape (Points &points, int max_angle = 90, int min_length = 10);
void SYSLIB_API Link_Bearings (Link_Data &link_rec, int &bearing_in, int &bearing_out);
int  SYSLIB_API X_Sort_Function (const void *rec1, const void *rec2);
bool SYSLIB_API In_Polygon (Points &points, double x, double y);
bool SYSLIB_API Vehicle_Shape (XYZ_Point p1, XYZ_Point p2, double width, Points &points, bool front_flag = true);
bool SYSLIB_API Vehicle_Shape (Points &pts, double width, Points &points, bool front_flag = true);
bool SYSLIB_API Shift_Shape (Points &points, double side = 0.0, int dir = 0);

#endif
