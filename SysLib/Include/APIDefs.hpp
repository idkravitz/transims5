/*********************************************************
	APIDefs.hpp - define API options
**********************************************************/

#ifndef APIDEFS_HPP
#define APIDEFS_HPP

#ifdef _MSC_EXTENSIONS
# ifdef DLL_IMPORTS
#  define SQLITE_API __declspec(dllimport)
#  define SYSLIB_API __declspec(dllimport)
#  pragma warning(disable: 4251)
# endif
# ifdef DLL_EXPORTS
#  define SQLITE_API __declspec (dllexport)
#  define SYSLIB_API __declspec (dllexport)
#  pragma warning(disable: 4251)
# endif
#  define HASH_MAP
#  define _DEFINE_DEPRECATED_HASH_CLASSES  0
#else
# ifdef DLL_IMPORTS
#  define SQLITE_API __attribute__ ((visibility("default")))
#  define SYSLIB_API __attribute__ ((visibility("default")))
# endif
# ifdef DLL_EXPORTS
#  define SQLITE_API __attribute__ ((visibility("default")))
#  define SYSLIB_API __attribute__ ((visibility("default")))
# endif
#endif

#ifdef MAKE_MPI
# define MPI_EXE
#else
# ifdef BOOST_MPI
#  define MPI_EXE
# endif
#endif

#ifdef DEBUG
# define DEBUG_KEYS
#endif

#ifndef SYSLIB_API
# define SYSLIB_API
#endif
#endif
