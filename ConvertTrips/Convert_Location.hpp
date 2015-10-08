//*********************************************************
//	Convert_Location.hpp - additional location data
//*********************************************************

#ifndef CONVERT_LOCATION_HPP
#define CONVERT_LOCATION_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Convert_Location class definition
//---------------------------------------------------------

class Convert_Location : public Doubles
{
public:
	Convert_Location (void)               { Clear (); }             

	int    Zone_List (void)               { return (zone_list); }
	int    Org_Parking (void)             { return (org_parking); }
	int    Des_Parking (void)             { return (des_parking); }

	void   Zone_List (int value)          { zone_list = value; }
	void   Org_Parking (int value)        { org_parking = value; }
	void   Des_Parking (int value)        { des_parking = value; }

	void   Clear (void)                   { zone_list = -1 ; org_parking = des_parking = 0; clear (); }

	int    Num_Weight (void)              { return ((int) size ()); }
	void   Num_Weight (int num)           { double w = 0.0; assign (num, w); }

	double Weight (int value)             { return ((value < 0) ? 1.0 : at (value)); }

private:
	int     zone_list;
	int     org_parking;
	int     des_parking;
};

typedef vector <Convert_Location>    Convert_Loc_Array;
typedef Convert_Loc_Array::iterator  Convert_Loc_Itr;

#endif

