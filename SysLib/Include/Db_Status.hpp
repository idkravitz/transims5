//********************************************************* 
//	Db_Status.hpp - database status code
//*********************************************************

#ifndef DB_STATUS_HPP
#define DB_STATUS_HPP

#include "APIDefs.hpp"
#include "System_Defines.hpp"
#include "Static_Service.hpp"
#include "Execution_Service.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Db_Status Class definition
//---------------------------------------------------------

class SYSLIB_API Db_Status : public Static_Service
{
public:
	Db_Status (void);

	bool    Status (void)                    { return (status == OK); }
	bool    Status (Status_Code stat, bool message = true);

	void    File_ID (string id)              { file_id = id; }
	String& File_ID (void)                   { return ((!file_id.empty ()) ? file_id : default_id); }

	void    File_Type (string label)         { file_type = label; }
	String& File_Type (void)                 { return ((!file_type.empty ()) ? file_type : default_type); }

	Status_Code Get_Status (void)            { return (status); }

	const char * Status_Message (void)       { return (Db_File_Code (Get_Status ())); }

private:
	Status_Code status;

	String file_type;
	String file_id;

	static String default_type;
	static String default_id;
};
#endif
