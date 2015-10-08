//*********************************************************
//	Report_Service.hpp - general reporting services
//*********************************************************

#ifndef REPORT_SERVICE_HPP
#define REPORT_SERVICE_HPP

#include "Message_Service.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Report_Service - general reporting class
//---------------------------------------------------------

class SYSLIB_API Report_Service : public Message_Service
{
public:
	Report_Service (void);

	String  Control_File (void)               { return (control_file); }
	void    Control_File (string filename)    { control_file = filename; }
	
	String  Title (void)                      { return (title); }
	void    Title (string name)               { title = name; }
	
	bool    XML_Flag (void)                   { return (xml_flag); }
	void    XML_Flag (bool flag)              { xml_flag = flag; }

	bool    XML_Help_Flag (void)              { return (xml_help); }
	void    XML_Help_Flag (bool flag)         { xml_help = flag; }

	bool    Enable_Parameter (void)           { return (parameter_flag); }
	void    Enable_Parameter (bool flag)      { parameter_flag = flag; }

	bool    Enable_Partitions (void)          { return (partition_flag); }
	void    Enable_Partitions (bool flag)     { partition_flag = flag; }

	int     First_Partition (void)            { return (first_part); }
	void    First_Partition (int num)         { first_part = num; }

	int     Last_Partition (void)             { return (last_part); }
	void    Last_Partition (int num)          { last_part = num; }

	int     Partition_Index (int number);
	int     Partition_Number (int index);
	int     Num_Partitions (void)             { return (MAX ((last_part - first_part), 0) + 1); }

	bool    Partition_Range (void)            { return (first_part >= 0 && last_part > first_part); }
	bool    Single_Partition (void)           { return (first_part >= 0 && last_part == first_part); }

	String  Parameter (void)                  { return (parameter); }
	void    Parameter (string param)          { parameter = param; }

	void    Exit_Stat (Exit_Code stat);

	Write_Stream & Write (int num_lines)                    { return (write.Write (num_lines)); }
	Write_Stream & Write (int num_lines, string text)       { return (write.Write (num_lines, text.c_str ())); }
	Write_Stream & Write (int num_lines, const char *text)  { return (write.Write (num_lines, text)); };

	Print_Stream & Print (int num_lines)                    { return (print.Print (num_lines)); }
	Print_Stream & Print (int num_lines, string text)       { return (print.Print (num_lines, text.c_str ())); }
	Print_Stream & Print (int num_lines, const char *text)  { return (print.Print (num_lines, text)); }

	Write_Stream & Warning (string text)                    { return (Warning (text.c_str ())); }
	Write_Stream & Warning (const char *text);
	Write_Stream & Active (bool flag)                       { return (write.Active (flag));  }

	XML_Stream & XML (int num_lines, string text)           { return (xml.XML (num_lines, text.c_str ())); }
	XML_Stream & XML (int num_lines, const char *text)      { return (xml.XML (num_lines, text)); }

	void    Error (string text)                             { Error (text.c_str ()); }
	void    Error (const char *text);

	void    Mem_Error (string type)                         { Mem_Error (type.c_str ()); }
	void    Mem_Error (const char *type);

	void    Print_Filename (int num_lines, string label, string filename);
	void    File_Error (string label, string error);
	
	void    Write_Time (int num_lines, string text)         { Write_Time (num_lines, text.c_str ()); } 
	void    Write_Time (int num_lines, const char *text);

	void    Header_Number (int number)        { header = number; }
	int     Header_Number (void)              { return (header); }
	
	void    Print_Banner (void);

	bool    Break_Check (int num_lines)       { return (print.Break_Check (num_lines)); }
	void    Page_Size (int width, int lines)  { print.Page_Size (width, lines); }
	void    New_Page (void)                   { return (print.New_Page ()); }
	void    Flush (void)                      { print.Flush (); }

	int     Page_Lines (void)                 { return (print.Page_Lines ()); }
	int     Page_Width (void)                 { return (print.Page_Width ()); }

	virtual void Page_Header ()               {}

	string& Fix_Directory (string& dir);
	string  Replace_Directory (string dir, string filename);

	//---- problem services ----

	void Max_Problems (int number)            { max_problems = number; };
	int  Max_Problems (void)                  { return (max_problems); };

	int  Num_Problems (void)                  { return (num_problems [TOTAL_PROBLEM]); };
	void Reset_Problems (void)                {	memset (num_problems, '\0', sizeof (num_problems)); }

	int  Set_Problem (Problem_Type problem);

	void Report_Problems (int total_records = 0, bool show_flag = true);

	void Add_Problems (int *problem_array);
	int * Get_Problems (void)                 { return (num_problems); }

protected:
	void Open_Report (string report_dir, string report_name, bool report_flag);
	bool Open_XML (string report_dir, string report_name);

	bool Page_Check (int new_lines)           { return (print.Page_Check (new_lines)); }

	Int_Set help_set;

private:
	Print_Stream print;
	Write_Stream write;
	XML_Stream xml;

	String title, control_file, parameter;
	int header, first_part, last_part, max_problems, num_problems [MAX_PROBLEM];
	bool partition_flag, parameter_flag, xml_flag, xml_help;
};
#endif
