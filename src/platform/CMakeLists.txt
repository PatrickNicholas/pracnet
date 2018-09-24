include_directories(. )

#aux_source_directory(. SOURCE_FILES)
set(SOURCE_FILES 
	Epoll.cpp
	Selector.cpp
	SignalManager.cpp
)

add_library(platform ${SOURCE_FILES})
