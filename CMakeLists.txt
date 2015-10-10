cmake_minimum_required (VERSION 2.8.4)

project (Transims) # Version 5!
# !!!! TransCAD support is disabled !!!!

set(PROJECT_GCC_FLAGS " -mtune=native -O3") # adjust flags as your soul desires
set(PROJECT_GCC_FLAGS "${PROJECT_GCC_FLAGS} -w") # comment this if you want to see tons of warnings (probably you are not)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PROJECT_GCC_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_GCC_FLAGS}")

foreach(target_with_data_dir IN ITEMS
		AdjustPlans ConvertTours ConvertTrips Emissions EventSum LineSum Microsimulator ModeChoice PopSyn Progression
		Router SmoothData SmoothPlans TransimsNet TransitNet)
	set(${target_with_data_dir}_extra_dirs "Data")
endforeach(target_with_data_dir)

include_directories(SysLib/Include)

foreach(syslib_dir IN ITEMS "Data"
							"Dbfile"
							"Files"
							"Path_Builder"
							"Program"
							"Projection"
							"Read"
							"Service"
							"Sim_Method"
							"Simulator_IO"
							"Utility"
							"Write")
	aux_source_directory(SysLib/${syslib_dir} SOURCES)
endforeach(syslib_dir)
add_library(SysLib STATIC ${SOURCES})

aux_source_directory(sqlite3 SQLITE_SOURCES)
add_library(sqlite3 STATIC ${SQLITE_SOURCES})

aux_source_directory(TcadLib TCADLIB_SOURCES)
add_library(TcadLib STATIC ${TCADLIB_SOURCES})

aux_source_directory(TppLib TPPLIB_SOURCES)
add_library(TppLib STATIC ${TPPLIB_SOURCES})

find_package(Threads REQUIRED)

foreach(target_name IN ITEMS ArcNet ArcPlan ArcSnapshot ConvertTrips EventSum ExportPlans ExportNet FileFormat IntControl LineSum LinkData LinkDelay LinkSum LocationData 
	NetPrep NewFormat PathSkim PlanCompare PlanPrep PlanSelect PlanSum PlanTrips ModeChoice ProblemSelect RandomSelect Relocate Router SimSubareas Simulator 
	TransimsNet TransitAccess TransitDiff TransitNet TripPrep Validate ZoneData)
	unset(SOURCES)
	aux_source_directory(${target_name} SOURCES)
	if (${target_name}_extra_dirs)
		foreach(dirname IN ITEMS ${${target_name}_extra_dirs})
			aux_source_directory(${target_name}/${dirname} SOURCES)
		endforeach(dirname)
	endif()
	add_executable(${target_name} ${SOURCES})
	if(THREADS_HAVE_PTHREAD_ARG)
	  set_property(TARGET ${target_name} PROPERTY COMPILE_OPTIONS "-pthread")
	  set_property(TARGET ${target_name} PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
	endif()
	if(CMAKE_THREAD_LIBS_INIT)
	  target_link_libraries(${target_name} "${CMAKE_THREAD_LIBS_INIT}")
	endif()
	target_link_libraries(${target_name} ${CMAKE_DL_LIBS})
	target_link_libraries(${target_name} SysLib)
	target_link_libraries(${target_name} sqlite3)
	target_link_libraries(${target_name} TcadLib)
	target_link_libraries(${target_name} TppLib)
endforeach(target_name)

