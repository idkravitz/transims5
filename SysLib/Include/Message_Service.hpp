//*********************************************************
//	Message_Service.hpp - general message services
//*********************************************************

#ifndef MESSAGE_SERVICE_HPP
#define MESSAGE_SERVICE_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Streams.hpp"

#ifdef MPI_EXE	
# include "Data_Buffer.hpp"
# ifdef BOOST_MPI
#   include <boost/serialization/vector.hpp>
#   include <boost/mpi.hpp>
#   include <boost/mpi/datatype.hpp>
    namespace mpi = boost::mpi;
# else
#   include "mpi.h"
# endif
#endif
//---------------------------------------------------------
//	Message_Service - general message class
//---------------------------------------------------------

class SYSLIB_API Message_Service : public Static_Service
{
public:
	Message_Service (void);
	virtual ~Message_Service (void) {}

	enum Exit_Code {DONE, FATAL, WARNING, CANCEL, RUNNING};

	Exit_Code Program_Status (void)                  { return (status); }
	void Program_Status (Exit_Code stat)             { status = stat; }

	bool Quiet (void)                                { return (message.Quiet ()); }
	void Quiet (bool flag)                           { message.Quiet (flag); }

	const char * Program (void)                      { return (program); }
	void Program (const char *p)                     { program = p; }

	int Version (void)                               { return (version); }
	void Version (int number)                        { version = number; }

	const char * CopyRight (void)                    { return (copyright); }
	void CopyRight (const char *c)                   { copyright = c; }

	time_t StartTime (void)                          { return (starttime); }
	void StartTime (time_t dt)                       { starttime = dt; }
	
	bool Send_Messages (void)                        { return (send_messages); }
	void Send_Messages (bool send)                   { send_messages = send; }

	bool Master (void)                               { return (mpi_rank == 0); }
	bool Slave (void)                                { return (mpi_rank > 0); }

	int  MPI_Rank (void)                             { return (mpi_rank); }
	void MPI_Rank (int rank)                         { mpi_rank = rank; }

	int  MPI_Size (void)                             { return (mpi_size); }
	void MPI_Size (int size)                         { mpi_size = size; }

	void Show_Banner (void);

	Message_Stream & Show_Message (int num_lines)    { return (message.Message (num_lines)); }
	Message_Stream & Show_Message (string text)      { return (message.Message (text.c_str ())); }
	Message_Stream & Show_Message (int num_lines, string text)	
	                                                 { return (message.Message (num_lines, text.c_str ())); }

	Message_Stream & Show_Message (const char *text) { return (message.Message (text)); }
	Message_Stream & Show_Message (int num_lines, const char *text)
	                                                 { return (message.Message (num_lines, text)); }

	bool Show_Question (string text)                 { return (Show_Question (text.c_str ())); }  
	bool Show_Question (const char *text);

	void Show_Error (string text)                    { Show_Error (text.c_str ()); }
	void Show_Error (const char *text);

	Message_Stream & Show_Warning (string text)      { return (Show_Warning (text.c_str ())); }
	Message_Stream & Show_Warning (const char *text);

	void Set_Progress (void);
	void Show_Dot (void);
	
	void Show_Progress (int value = 0);
	void Show_Progress (string text)                 { Show_Progress (text.c_str ()); }
	void Show_Progress (const char *text);          
	
	void End_Progress (void);
	void End_Progress (string text)                  { End_Progress (text.c_str ()); }
	void End_Progress (const char *text);

	int  Progress_Count (void)                       { return (progress); }

	void Pause_Process (void);

	String Get_Control_File (void);

	int Return_Code (void);

	virtual void Exit_Stat (Exit_Code stat);

#ifdef MPI_EXE	
	Data_Buffer mpi_buffer;
	Data_Buffer mpi_text;

	boost::mutex  mpi_mutex;

	void Send_MPI_Message (String message);
	int  Show_MPI_Message (void);

	void Send_MPI_Buffer (int rank = 0)              { Send_MPI_Buffer (mpi_buffer, 0, rank); }
	int  Get_MPI_Buffer (int rank = -1)              { return (Get_MPI_Buffer (mpi_buffer, 0, rank)); }
	
	void Send_MPI_Buffer (Data_Buffer &data, int tag = 0, int rank = 0);
	int  Get_MPI_Buffer (Data_Buffer &data, int tag = 0, int rank = -1) ;

	void Send_MPI_Array (int *data, int num, int rank = 0);
	int  Get_MPI_Array (int *data, int num, int rank = -1);
#endif

protected:
	void Banner (ofstream *file = 0);

	int  Warnings (void)                             { return (warnings); }
	void Warnings (int value)                        { warnings = value; }

	int  Max_Warnings (void)                         { return (max_warnings); }
	void Max_Warnings (int value)                    { max_warnings = value; }

	bool Warning_Flag (void)                         { return (warning_flag); }
	void Warning_Flag (bool flag)                    { warning_flag = flag; }

	bool Pause (void)                                { return (pause); }
	void Pause (bool flag)                           { pause = flag; }

	bool No_Pause (void)                             { return (no_pause); }
	void No_Pause (bool flag)                        { no_pause = flag; }

	bool Detail (void)                               { return (detail); }
	void Detail (bool flag)                          { detail = flag; }

	Message_Stream message;

#ifdef BOOST_MPI
	mpi::environment env;
	mpi::communicator world;
#endif

private:
	int version, progress, warnings, max_warnings, mpi_rank, mpi_size;
	bool pause, no_pause, detail, send_messages, warning_flag;
	const char *program, *copyright;
	time_t starttime;
	clock_t last;

	Exit_Code status;
	void Exit_Program (void);
};
#endif
