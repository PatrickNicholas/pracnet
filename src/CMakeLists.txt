include_directories(. )

add_subdirectory(platform)
add_subdirectory(ip)

#aux_source_directory(. SOURCE_FILES)
set(SOURCE_FILES 
	EventBase.cpp
	EventHandler.cpp
	TimerQueue.cpp
)

add_library(network ${SOURCE_FILES} )
target_link_libraries(network ip platform)