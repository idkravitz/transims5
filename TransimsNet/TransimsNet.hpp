//*********************************************************
//	TransimsNet.hpp - Network Conversion Utility
//*********************************************************

#ifndef TRANSIMSNET_HPP
#define TRANSIMSNET_HPP

#include "Data_Service.hpp"
#include "Random.hpp"
#include "Db_Header.hpp"
#include "Data_Range.hpp"
#include "Best_List.hpp"
#include "List_Data.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "TypeDefs.hpp"
#include "Link_Detail_Data.hpp"
#include "Link_Detail_File.hpp"

#include "Pocket_Warrant.hpp"
#include "Control_Warrant.hpp"
#include "Access_Warrant.hpp"
#include "Parking_Details.hpp"
#include "Street_Parking.hpp"

//---------------------------------------------------------
//	TransimsNet - execution class definition
//---------------------------------------------------------

class TransimsNet : public Data_Service
{
public:
	TransimsNet (void);
	virtual ~TransimsNet (void);

	virtual void Execute (void);

	//virtual bool Link_Processing (Db_File *file);
	//virtual bool Zone_Processing (Db_File *file);
	//virtual bool Location_Processing (Db_File *file);
	//virtual bool Parking_Processing (Db_File *file);
	//virtual bool Access_Processing (Db_File *file);
	//virtual bool Pocket_Processing (Db_File *file);
	//virtual bool Connectivity_Processing (Db_File *file);
	//virtual bool Sign_Processing (Db_File *file);
	//virtual bool Signal_Processing (Db_File *file);
	
protected:
	enum TransimsNet_Keys { 
		DEFAULT_LINK_SETBACK = 1, DEFAULT_LOCATION_SETBACK, MAXIMUM_CONNECTION_ANGLE, 
		ADD_UTURN_TO_DEAD_END_LINKS, SIGNAL_ID_AS_NODE_ID, EXTERNAL_ZONE_RANGE, 
		EXTERNAL_STATION_OFFSET, ZONE_BOUNDARY_FILE, ZONE_FIELD_NAME, 
		POCKET_LANE_WARRANT, TRAFFIC_CONTROL_WARRANT, FACILITY_ACCESS_WARRANT, 
		PARKING_DETAILS_WARRANT, STREET_PARKING_WARRANT, 
		UPDATE_NODE_RANGE, UPDATE_LINK_RANGE, UPDATE_NODE_FILE, UPDATE_LINK_FILE, 
		DELETE_NODE_RANGE, DELETE_LINK_RANGE, DELETE_NODE_FILE, DELETE_LINK_FILE,
		LINK_DETAIL_FILE,
	};
	virtual void Program_Control (void);

private:

	bool details_flag, boundary_flag, street_flag, ext_zone_flag, signal_id_flag;
	bool update_flag, delete_flag, connect_flag, replicate_flag, link_detail_flag;
	bool update_node_flag, update_link_flag, delete_node_flag, delete_link_flag;
	bool zone_flag, zout_flag, turn_flag, shape_flag, name_flag, collapse_flag, uturn_flag;
	double max_length_factor;

	int straight_diff, thru_diff, max_angle, uturn_angle, short_length, zone_field;

	int location_id, parking_id;
	int location_base, parking_base;

	int nnode, nlink, nparking, nactivity, nprocess, npocket, nconnect, nsign, nsignal, nuse;
	int mparking, mactivity, mprocess, mpocket, muse;
	int xlink, xnode, xparking, xactivity, xprocess, xpocket, xconnect, xsign, xsignal, xuse;
	int nshort, nlength, nexternal, nzone, nzout, max_splits;
	int nfixed1, nfixed2, nfixed3, nactuated1, nactuated2, nactuated3, nstop, nyield;
	int nshape, nshapes, nturn;
	int min_length, link_setback, loc_setback, external_offset;

	Db_File keep_nodes, turn_nodes, node_string;
	Db_File update_link_file, update_node_file, delete_link_file, delete_node_file;

	Arcview_File boundary_file;
	Projection_Service proj_service;
	Points_Map boundary_map;

	Link_Detail_Map detail_map;
	Link_Detail_File detail_file;

	Pocket_Warrant_Map pocket_warrant_map;
	Pocket_Warrant_Array pocket_warrants;

	Control_Warrant_Map control_warrant_map;
	Control_Warrant_Array control_warrants;

	Access_Warrant_Map access_warrant_map;
	Access_Warrant_Array access_warrants;

	Park_Detail_Array parking_details;

	Int2_Map  street_park_map;
	Street_Park_Array  street_parking;

	List_Array node_list;
	List_Array turn_list;

	Data_Range ext_zone_range;
	Data_Range update_node_range, update_link_range, delete_node_range, delete_link_range;

	//---- methods ----

	void Read_Details (void);
	void Read_Boundary (void);
	void Link_Access (void);
	void Connections (void);
	void Pocket_Lanes (void);
	void Traffic_Controls (void);
	void Lane_Ranges (void);
	void Lane_Use (void);
};
#endif


