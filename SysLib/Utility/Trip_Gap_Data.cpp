//*********************************************************
//	Trip_Gap_Data.cpp - Trip Gap Summary Data
//*********************************************************

#include "Trip_Gap_Data.hpp"
#include "Dtime.hpp"
#include <math.h>

//---------------------------------------------------------
//	Trip_Gap_Data constructor
//---------------------------------------------------------

Trip_Gap_Data::Trip_Gap_Data (void)
{
	gap_report = output_flag = cost_flag = period_flag = active_flag = false;
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Trip_Gap_Data::Clear (void)
{
	gap_array.clear ();
}

//---------------------------------------------------------
//	Report_Flags -- set report flags
//---------------------------------------------------------

bool Trip_Gap_Data::Report_Flags (bool gap)
{
	gap_report = gap;

	Initialize ();

	return (gap_report);
}

//---------------------------------------------------------
//	Initialize -- allocate memory
//---------------------------------------------------------

void Trip_Gap_Data::Initialize (void)
{
	Gap_Data gap_data;

	Clear ();

	active_flag = gap_report || output_flag;
	if (!active_flag) return;

	num_periods = periods.Num_Periods ();

	memset (&gap_data, '\0', sizeof (gap_data));

	gap_array.assign ((num_periods + 1), gap_data);
}

//---------------------------------------------------------
//	Set_Periods -- set time periods 
//---------------------------------------------------------

bool Trip_Gap_Data::Set_Periods (Time_Periods &per)
{
	period_flag = periods.Copy_Periods (per);

	Initialize ();

	return (period_flag);
}

//---------------------------------------------------------
//	Open_Trip_Gap_File -- trip gap file
//---------------------------------------------------------

bool Trip_Gap_Data::Open_Trip_Gap_File (string filename)
{
	if (!filename.empty ()) {
		output_flag = true;

		exe->Print (1);
		gap_file.File_Type (String ("New %s Trip Gap File") % ((cost_flag) ? "Cost" : "Time"));

		if (!gap_file.Create (exe->Project_Filename (filename))) {
			exe->File_Error ("Creating New Trip Gap File", gap_file.Filename ());
			return (false);
		}

		//---- write the file header ----

		fstream &file = gap_file.File ();

		if (cost_flag) {
			file << "PERIOD\tCOUNT\tINPUT_COST\tCOMPARE_COST\tDIFFERENCE\tTRIP_GAP\n";
		} else {
			file << "PERIOD\tCOUNT\tINPUT_TIME\tCOMPARE_TIME\tDIFFERENCE\tTRIP_GAP\n";
		}
		Initialize ();
	}
	return (true);
}

//---------------------------------------------------------
//	Add_Trip_Gap_Data -- add data to the summary periods
//---------------------------------------------------------

void Trip_Gap_Data::Add_Trip_Gap_Data (Dtime tod, double input, double compare)
{
	if (!active_flag || input < 0 || compare < 0) return;

	int period;

	if (period_flag) {
		period = periods.Period (tod);
		if (period < 0) return;
	} else {
		period = num_periods;
	}
	double abs_diff = fabs (input - compare);

	Gap_Data *gap_ptr;

	gap_ptr = &gap_array [num_periods]; 

	gap_ptr->count++;
	gap_ptr->input += input;
	gap_ptr->compare += compare;
	gap_ptr->abs_diff += abs_diff;

	//---- period summary ----

	if (period_flag) {
		gap_ptr = &gap_array [period];

		gap_ptr->count++;
		gap_ptr->input += input;
		gap_ptr->compare += compare;
		gap_ptr->abs_diff += abs_diff;
	}
}

//---------------------------------------------------------
//	Trip_Gap_Report
//---------------------------------------------------------

void Trip_Gap_Data::Trip_Gap_Report (int number)
{
	int period;
	double factor;
	String buffer;

	Gap_Data *gap_ptr;

	//---- print the report ----

	exe->Header_Number (number);

	if (!exe->Break_Check (num_periods + 6)) {
		exe->Print (1);
		Trip_Gap_Header ();
	}
	factor = (cost_flag) ? 100.0 : (double) Dtime (1, MINUTES);

	for (period = 0; period <= num_periods; period++) {

		if (period == num_periods) {
			exe->Print (1);
			buffer = "Total";
		} else {
			buffer = periods.Range_Format (period);
		}
		gap_ptr = &gap_array [period];
		//if (gap_ptr->count == 0 && period < num_periods) continue;

		exe->Print (1, String ("%-12.12s %9d %14.0lf %14.0lf %14.0lf %12.6lf") %
			buffer % gap_ptr->count % (gap_ptr->input / factor) % (gap_ptr->compare / factor) %
			(gap_ptr->abs_diff / factor) % ((gap_ptr->compare > 0) ? (gap_ptr->abs_diff / gap_ptr->compare) : 0.0));
	}
	exe->Header_Number (0);
}

//---------------------------------------------------------
//	Trip Gap Header
//---------------------------------------------------------

void Trip_Gap_Data::Trip_Gap_Header (void)
{
	exe->Print (1, String ("Trip Gap Report (%s)") % ((cost_flag) ? "Dollars" : "Minutes"));
	exe->Print (2, String ("%16cNumber  ---------- Travel %s (%s) ----------") % BLANK %
		((cost_flag) ? "Cost" : "Time") % ((cost_flag) ? "dollars" : "minutes") % FINISH);
	exe->Print (1, "Time Period      Trips          Input        Compare     Difference     Trip Gap");
	exe->Print (1);

}

/*********************************************|***********************************************

	Time Period Summary (Minutes) (Dollars)

	                        ---------- Travel Time (minutes) ---------- 
	                Number  ---------- Travel Cost (dollars) ---------- 
	Time Period     Trips           Input        Compare     Difference     Trip Gap

	dd:dd..dd:dd ddddddddd ffffffffffffff ffffffffffffff ffffffffffffff   fff.ffffff
	
	Total        ddddddddd ffffffffffffff ffffffffffffff ffffffffffffff   fff.ffffff
      
**********************************************|***********************************************/ 

//---------------------------------------------------------
//	Write_Trip_Gap_File
//---------------------------------------------------------

void Trip_Gap_Data::Write_Trip_Gap_File (void)
{
	if (!output_flag) return;

	int period;
	String buffer;
	Gap_Data *gap_ptr;

	exe->Show_Message (String ("Writing %s -- Record") % gap_file.File_Type ());
	exe->Set_Progress ();

	fstream &file = gap_file.File ();
	double factor = (cost_flag) ? 100.0 : (double) Dtime (1, MINUTES);

	for (period=0; period <= num_periods; period++) {
		exe->Show_Progress ();
		if (period == num_periods) {
			buffer = "Total";
		} else {
			buffer = periods.Range_Format (period);
		}
		gap_ptr = &gap_array [period];

		file << (String ("%s\t%d\t%.2lf\t%.2lf\t%.2lf\t%.6lf\n") % buffer % gap_ptr->count % 
			(gap_ptr->input / factor) % (gap_ptr->compare / factor) % (gap_ptr->abs_diff / factor) % 
			((gap_ptr->compare > 0) ? (gap_ptr->abs_diff / gap_ptr->compare) : 0.0));
	}
	exe->End_Progress ();
	gap_file.Close ();

	exe->Print (2, String ("Number of %s Records = %d") % gap_file.File_Type () % exe->Progress_Count ());
}

//---------------------------------------------------------
//	Replicate -- replicated the data structures
//---------------------------------------------------------

void Trip_Gap_Data::Replicate (Trip_Gap_Data &data)
{
	if (data.active_flag) {
		if (data.period_flag) {
			Set_Periods (data.periods);
		}
		output_flag = data.Output_Flag ();
		cost_flag = data.Cost_Flag ();

		Report_Flags (data.gap_report);
	}
}

//---------------------------------------------------------
//	Merge_Data -- copy data structures
//---------------------------------------------------------

void Trip_Gap_Data::Merge_Data (Trip_Gap_Data &data)
{
	if (!active_flag || !data.active_flag) return;

	int i;
	Gap_Itr gap_itr;
	Gap_Data *gap_ptr;

	for (i=0, gap_itr = gap_array.begin (); gap_itr != gap_array.end (); gap_itr++, i++) {
		gap_ptr = &data.gap_array [i];

		gap_itr->count += gap_ptr->count;
		gap_itr->input += gap_ptr->input;
		gap_itr->compare += gap_ptr->compare;
		gap_itr->abs_diff += gap_ptr->abs_diff;
	}
}
