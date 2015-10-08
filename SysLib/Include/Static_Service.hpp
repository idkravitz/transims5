//*********************************************************
//	Static_Service.hpp - static program services
//*********************************************************

#ifndef STATIC_SERVICE_HPP
#define STATIC_SERVICE_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "String.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef BOOST_THREADS
#include <boost/thread/mutex.hpp>
extern  SYSLIB_API boost::mutex   main_mutex;
#define MAIN_LOCK    { boost::mutex::scoped_lock lock (main_mutex);
#else
#define MAIN_LOCK    {
#endif
#define END_LOCK     }

//---------------------------------------------------------
//	Static_Service - static program services
//---------------------------------------------------------

class SYSLIB_API Static_Service
{
public:
	Static_Service (void);

	static Units_Type Time_Code (String text)                    { return ((Units_Type) Type_Code (text, time_codes, "Time Format")); }
	static const char * Time_Code (Units_Type code)              { return (Type_Code (code, time_codes)); }

	static Units_Type Length_Code (String text)                  { return ((Units_Type) Type_Code (text, length_codes, "Length Format")); }
	static const char * Length_Code (Units_Type code)            { return (Type_Code (code, length_codes)); }

	static Measure_Type Measure_Code (String text)               { return ((Measure_Type) Type_Code (text, measure_codes, "Units of Measure")); }
	static const char * Measure_Code (Measure_Type code)         { return (Type_Code (code, measure_codes)); }

	static Units_Type Units_Code (String text)                   { return ((Units_Type) Type_Code (text, units_codes, "Units")); }
	static const char * Units_Code (Units_Type code)             { return (Type_Code (code, units_codes)); }

	static Format_Type Format_Code (String text)                 { return ((Format_Type) Type_Code (text, format_codes, "Format")); }
	static const char * Format_Code (Format_Type code)           { return (Type_Code (code, format_codes)); }

	static Field_Type Field_Code (String text)                   { return ((Field_Type) Type_Code (text, field_codes, "Field")); }
	static const char * Field_Code (Field_Type code)             { return (Type_Code (code, field_codes)); }
	
	static Nest_Type Nest_Code (String text)                     { return ((Nest_Type) Type_Code (text, nest_codes, "Nest")); }
	static const char * Nest_Code (Nest_Type code)               { return (Type_Code (code, nest_codes)); }
	static bool Check_Nest_Code (String text);

	static Facility_Type Facility_Code (String text)             { return ((Facility_Type) Type_Code (text, facility_codes, "Facility", EXTERNAL)); }
	static const char * Facility_Code (Facility_Type code)       { return (Type_Code (code, facility_codes)); }
	
	static Mode_Type Mode_Code (String text)                     { return ((Mode_Type) Type_Code (text, mode_codes, "Mode")); }
	static const char * Mode_Code (Mode_Type code)               { return (Type_Code (code, mode_codes)); }
	static Mode_Type Trip_Mode_Map (int code);

	static Parking_Type Parking_Code (String text)               { return ((Parking_Type) Type_Code (text, parking_codes, "Parking")); }
	static const char * Parking_Code (Parking_Type code)         { return (Type_Code (code, parking_codes)); }

	static Restrict_Type Restrict_Code (String text)             { return ((Restrict_Type) Type_Code (text, restrict_codes, "Restriction")); }
	static const char * Restrict_Code (Restrict_Type code)       { return (Type_Code (code, restrict_codes)); }
	
	static Pocket_Type Pocket_Code (String text)                 { return ((Pocket_Type) Type_Code (text, pocket_codes, "Pocket Lane")); }
	static const char * Pocket_Code (Pocket_Type code)           { return (Type_Code (code, pocket_codes)); }
	
	static Connect_Type Connect_Code (String text)               { return ((Connect_Type) Type_Code (text, connect_codes, "Connection")); }
	static const char * Connect_Code (Connect_Type code)         { return (Type_Code (code, connect_codes)); }

	static Transit_Type Transit_Code (String text)               { return ((Transit_Type) Type_Code (text, transit_codes, "Transit Mode")); }
	static const char * Transit_Code (Transit_Type code)         { return (Type_Code (code, transit_codes)); }
	
	static Class_Type Class_Code (String text)                   { return ((Class_Type) Type_Code (text, class_codes, "Fare Class")); }
	static const char * Class_Code (Class_Type code)             { return (Type_Code (code, class_codes)); }

	static Stop_Type Stop_Code (String text)                     { return ((Stop_Type) Type_Code (text, stop_codes, "Transit Stop")); }
	static const char * Stop_Code (Stop_Type code)               { return (Type_Code (code, stop_codes)); }

	static Use_Type Veh_Use_Code (String text)                   { return ((Use_Type) Type_Code (text, use_codes, "Vehicle Use")); }
	static const char * Veh_Use_Code (Use_Type code)             { return (Type_Code (code, use_codes)); }

	static Use_Type Veh_Type_Map (int code);
	static int Map_Veh_Type (Use_Type code);

	static Use_Type Transit_Use_Map (Transit_Type code);

	static ID_Type ID_Code (String text)                         { return ((ID_Type) Type_Code (text, id_codes, "ID Data")); }
	static const char * ID_Code (ID_Type code)                   { return (Type_Code (code, id_codes)); }

	static Control_Type Control_Code (String text)               { return ((Control_Type) Type_Code (text, control_codes, "Traffic Control")); }
	static const char * Control_Code (Control_Type code)         { return (Type_Code (code, control_codes)); }

	static Signal_Type Signal_Code (String text)                 { return ((Signal_Type) Type_Code (text, signal_codes, "Signal")); }
	static const char * Signal_Code (Signal_Type code)           { return (Type_Code (code, signal_codes)); }

	static Movement_Type Movement_Code (String text)             { return ((Movement_Type) Type_Code (text, movement_codes, "Movement")); }
	static const char * Movement_Code (Movement_Type code)       { return (Type_Code (code, movement_codes)); }

	static Movement_Type Movement_Code (int bearing, int turn);

	static Protection_Type Protection_Code (String text)         { return ((Protection_Type) Type_Code (text, protection_codes, "Signal Protection")); }
	static const char * Protection_Code (Protection_Type code)   { return (Type_Code (code, protection_codes)); }

	static Detector_Type Detector_Code (String text)             { return ((Detector_Type) Type_Code (text, detector_codes, "Signal Detector")); }
	static const char * Detector_Code (Detector_Type code)       { return (Type_Code (code, detector_codes)); }

	static int Use_Code (String text);
	static string Use_Code (int code);

	static bool Use_Permission (int use_code, Use_Type type)     { return ((use_code & use_mask [type]) != 0); }
	
	static System_File_Type System_File_Code (String text)       { return ((System_File_Type) Type_Code (text, system_file_codes, "System File", END_FILE)); }
	static const char * System_File_Code (System_File_Type code) { return (Type_Code (code, system_file_codes)); }
	
	static Problem_Type Problem_Code (String text);
	static const char * Problem_Code (Problem_Type code)         { return (Type_Code (code, problem_codes)); }

	static Event_Type Event_Code (String text)                   { return ((Event_Type) Type_Code (text, event_codes, "Event")); }
	static const char * Event_Code (Event_Type code)             { return (Type_Code (code, event_codes)); }

	static Constraint_Type Constraint_Code (String text)         { return ((Constraint_Type) Type_Code (text, constraint_codes, "Constraint")); }
	static const char * Constraint_Code (Constraint_Type code)   { return (Type_Code (code, constraint_codes)); }

	static Priority_Type Priority_Code (String text)             { return ((Priority_Type) Type_Code (text, priority_codes, "Priority")); }
	static const char * Priority_Code (Priority_Type code)       { return (Type_Code (code, priority_codes)); }

	static Time_Point_Type Time_Point_Code (String text)         { return ((Time_Point_Type) Type_Code (text, time_point_codes, "Time Point")); }
	static const char * Time_Point_Code (Time_Point_Type code)   { return (Type_Code (code, time_point_codes)); }

	static Distance_Type Distance_Code (String text)             { return ((Distance_Type) Type_Code (text, distance_codes, "Distance")); }
	static const char * Distance_Code (Distance_Type code)       { return (Type_Code (code, distance_codes)); }
	
	static Matrix_Type Matrix_Code (String text)                 { return ((Matrix_Type) Type_Code (text, matrix_codes, "Matrix")); }
	static const char * Matrix_Code (Matrix_Type code)           { return (Type_Code (code, matrix_codes)); }

	static Units_Type Flow_Code (String text)                    { return ((Units_Type) Type_Code (text, flow_codes, "Flow", VEHICLES)); }
	static const char * Flow_Code (Units_Type code)              { return (Type_Code (code, flow_codes)); }

	static Performance_Type Performance_Code (String text)       { return ((Performance_Type) Type_Code (text, performance_codes, "Performance")); }
	static const char * Performance_Code (Performance_Type code) { return (Type_Code (code, performance_codes)); }
	
	static Rate_Type Rate_Code (String text)                     { return ((Rate_Type) Type_Code (text, rate_codes, "Emission Rate")); }
	static const char * Rate_Code (Rate_Type code)               { return (Type_Code (code, rate_codes)); }

	static Equation_Type Equation_Code (String text)             { return ((Equation_Type) Type_Code (text, equation_codes, "Equation")); }
	static const char * Equation_Code (Equation_Type code)       { return (Type_Code (code, equation_codes)); }

	static Projection_Type Projection_Code (String text)         { return ((Projection_Type) Type_Code (text, projection_codes, "Projection")); }
	static const char * Projection_Code (Projection_Type code)   { return (Type_Code (code, projection_codes)); }

	static Units_Type Coordinate_Code (String text)              { return ((Units_Type) Type_Code (text, coordinate_codes, "Coordinate")); }
	static const char * Coordinate_Code (Units_Type code)        { return (Type_Code (code, coordinate_codes)); }

	static Loading_Type Loading_Code (String text)               { return ((Loading_Type) Type_Code (text, loading_codes, "Vehicle Loading")); }
	static const char * Loading_Code (Loading_Type code)         { return (Type_Code (code, loading_codes)); }

	static Trip_Sort_Type Trip_Sort_Code (String text)           { return ((Trip_Sort_Type) Type_Code (text, trip_sort_codes, "Trip Sort")); }
	static const char * Trip_Sort_Code (Trip_Sort_Type code)     { return (Type_Code (code, trip_sort_codes)); }

	static Status_Code Db_File_Code (String text)                { return ((Status_Code) Type_Code (text, db_file_codes, "Status Code", DB_ERROR)); }
	static const char * Db_File_Code (Status_Code code)          { return (Type_Code (code, db_file_codes)); }

	static Relate_Type Relate_Code (String text)                 { return ((Relate_Type) Type_Code (text, relate_codes, "Relate")); }
	static const char * Relate_Code (Relate_Type code)           { return (Type_Code (code, relate_codes)); }

	static Gender_Type Gender_Code (String text)                 { return ((Gender_Type) Type_Code (text, gender_codes, "Gender")); }
	static const char * Gender_Code (Gender_Type code)           { return (Type_Code (code, gender_codes)); }

	static Combine_Type Combine_Code (String text)               { return ((Combine_Type) Type_Code (text, combine_codes, "Combine Method")); }
	static const char * Combine_Code (Combine_Type code)         { return (Type_Code (code, combine_codes)); }
	
	static Sel_Method_Type Select_Method_Code (String text)      { return ((Sel_Method_Type) Type_Code (text, select_method_codes, "Selection Method")); }
	static const char * Select_Method_Code (Sel_Method_Type code){ return (Type_Code (code, select_method_codes)); }

	static Loc_Method_Type Loc_Method_Code (String text)         { return ((Loc_Method_Type) Type_Code (text, loc_method_codes, "Location Method")); }
	static const char * Loc_Method_Code (Loc_Method_Type code)   { return (Type_Code (code, loc_method_codes)); }
	
	static Signal_Split_Type Signal_Split_Code (String text)     { return ((Signal_Split_Type) Type_Code (text, signal_split_codes, "Signal Split")); }
	static const char *Signal_Split_Code (Signal_Split_Type code){ return (Type_Code (code, signal_split_codes)); }

	static Units_Type Matrix_OD_Code (String text)               { return ((Units_Type) Type_Code (text, matrix_od_codes, "Matrix OD", ZONE_OD)); }
	static const char * Matrix_OD_Code (Units_Type code)         { return (Type_Code (code, matrix_od_codes)); }

	static int Bool_Code (String text)                           { return (Type_Code (text, bool_codes, "True/False")); }
	static string Bool_Code (int code)                           { return (Type_Code (code, bool_codes)); }

	static int Lane_Range_Code (String text);
	static string Lane_Range_Code (int code);
	
	static int Lane_ID_Code (String text);
	static string Lane_ID_Code (int code);

	static int    Internal_Units (int data, Units_Type *units);
	static int    Internal_Units (int data, Units_Type units = NO_UNITS);
	static double Internal_Units (double data, Units_Type *units);	
	static double Internal_Units (double data, Units_Type units = NO_UNITS);	
	static int    Internal_Units (string &data, Units_Type units = NO_UNITS);

	static int    External_Units (int data, Units_Type units = NO_UNITS);
	static double External_Units (double data, Units_Type units = NO_UNITS);
	static void   External_Units (int code, Units_Type units, String &result);

	static double Convert_Units (double data, Units_Type units);

	static Units_Type Parse_Units (string &text);

	static bool   Type_Range (String &range, Units_Type type, int &low, int &high);

	static struct tm *local_time (time_t *tim);
	static char * c_time (time_t *tim);
	
	static FILE *f_open (string filename, const char *control);
	static char *get_env (const char *var);
	static bool f_exist (string filename);

	static double Scale (double value)    { return (value * 10.0); }
	static int Round (double value)       { return ((value < 0) ? -((int) (-value * 10 + 0.5)) : (int) (value * 10 + 0.5)); }
	static int Round (int value)          { return (value * 10); }
	static double UnRound (int value)     { return ((double) value / 10.0); }
	static double UnRound (double value)  { return (value / 10.0); }
	static int Resolve (int value)        { return ((value < 0) ? -((-value + 5) / 10) : ((value + 5) / 10)); }
	static int Resolve (double value)     { return ((value < 0) ? -((int) ((-value + 5.0) / 10.0)) : (int) ((value + 5.0) / 10.0)); }
	static int Resolve (unsigned value)   { return ((value + 5) / 10); }
	static int DTOI (double value)        { return ((value < 0) ? -((int) (-value + 0.5)) : (int) (value + 0.5)); }

	static char * Clean (char *text);
	
	static Units_Type Performance_Units_Map (Performance_Type code, Units_Type flow_type = VEHICLES);

	static Units_Type  Time_Format (void)           { return (time_format); }
	static void Time_Format (Units_Type format)     { time_format = format; }

	static Format_Type  Default_Format (void)       { return (default_format); }
	static void Default_Format (Format_Type format) { default_format = format; }

	static void Metric_Flag (bool flag)             { metric_flag = flag; }
	static bool Metric_Flag (void)                  { return (metric_flag); }

private:
	static int use_mask [NONE+1];
	static Mode_Type trip_mode_map [];
	static Use_Type veh_type_map [];
	static Use_Type transit_use_map [];

	static bool metric_flag;
	static Units_Type time_format;
	static Format_Type default_format;

	struct Code_Text {
		int code;
		const char * text;
	};
	static Code_Text time_codes [];
	static Code_Text length_codes [];
	static Code_Text measure_codes [];
	static Code_Text units_codes [];
	static Code_Text format_codes [];
	static Code_Text field_codes [];
	static Code_Text nest_codes [];
	static Code_Text facility_codes [];
	static Code_Text mode_codes [];
	static Code_Text parking_codes [];
	static Code_Text restrict_codes [];
	static Code_Text pocket_codes [];
	static Code_Text connect_codes [];
	static Code_Text transit_codes [];
	static Code_Text class_codes [];
	static Code_Text stop_codes [];
	static Code_Text id_codes [];
	static Code_Text control_codes [];
	static Code_Text signal_codes [];
	static Code_Text movement_codes [];
	static Code_Text protection_codes [];
	static Code_Text detector_codes [];
	static Code_Text use_codes [];
	static Code_Text system_file_codes [];
	static Code_Text problem_codes [];
	static Code_Text simulation_codes [];
	static Code_Text event_codes [];
	static Code_Text constraint_codes [];
	static Code_Text priority_codes [];
	static Code_Text time_point_codes [];
	static Code_Text distance_codes [];
	static Code_Text matrix_codes [];
	static Code_Text flow_codes [];
	static Code_Text performance_codes [];
	static Code_Text rate_codes [];
	static Code_Text equation_codes [];
	static Code_Text projection_codes [];
	static Code_Text coordinate_codes [];
	static Code_Text loading_codes [];
	static Code_Text trip_sort_codes [];
	static Code_Text db_file_codes [];
	static Code_Text relate_codes [];
	static Code_Text gender_codes [];
	static Code_Text combine_codes [];
	static Code_Text select_method_codes [];
	static Code_Text loc_method_codes [];
	static Code_Text signal_split_codes [];
	static Code_Text bool_codes [];
	static Code_Text matrix_od_codes [];

	static int Type_Code (String text, Code_Text *codes, const char *label, int error_code = 0);
	static const char * Type_Code (int code, Code_Text *codes);
};
#endif
