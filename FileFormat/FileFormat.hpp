//*********************************************************
//	FileFormat.hpp - Convert files to a new format
//*********************************************************

#ifndef FILEFORMAT_HPP
#define FILEFORMAT_HPP

#include "Execution_Service.hpp"
#include "Db_Header.hpp"
#include "User_Program.hpp"
#include "Db_Array.hpp"

//---------------------------------------------------------
//	FileFormat - execution class definition
//---------------------------------------------------------

class SYSLIB_API FileFormat : public Execution_Service
{
public:
	FileFormat (void);
	virtual ~FileFormat (void);

	virtual void Execute (void);

protected:
	enum FileFormat_Keys { 
		DATA_FILE = 1, DATA_FORMAT, NEW_DATA_FILE, NEW_DATA_FORMAT,
		COPY_EXISTING_FIELDS, NEW_FILE_HEADER, NEW_DATA_FIELD, CONVERSION_SCRIPT,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum FileFormat_Reports { PRINT_SCRIPT = 1, PRINT_STACK };

	int num_files;
	Db_Header **file, **new_file;
	bool script_flag;

	Db_File program_file;
	User_Program program;
	Db_Base_Array data_rec;
};
#endif
