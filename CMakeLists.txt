cmake_minimum_required(VERSION 2.8)
 
PROJECT(PcapAnalysis)
 
find_package(VTK REQUIRED)

#Bring the headers, .h files, into the project
include_directories(include)

#WinPcap
include_directories(WpdPack_4_1_2/WpdPack/Include)

include(${VTK_USE_FILE})
 
#the file(GLOB...) allows for wildcard additions of cpp files:
file(GLOB SOURCES "src/*.cpp" "include/*.h")

#For the static pcap library:
#Can use either of these CMake options
#1
link_directories( ${CMAKE_SOURCE_DIR}/WpdPack_4_1_2/WpdPack/Lib)
set ( PROJECT_LINK_LIBS Packet.lib wpcap.lib )
#or 2
#set ( PROJECT_LINK_LIBS ${CMAKE_SOURCE_DIR}/WpdPack_4_1_2/WpdPack/Lib/Packet.lib ${CMAKE_SOURCE_DIR}/WpdPack_4_1_2/WpdPack/Lib/wpcap.lib )
#Can also use the file command instead of set command for * wildcard additions

add_executable(pcap_analysis MACOSX_BUNDLE ${SOURCES})

#Set pcap_analysis to be default VS project to run in the solution
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT pcap_analysis)

if(VTK_LIBRARIES)
  target_link_libraries(pcap_analysis ${VTK_LIBRARIES})
else()
  target_link_libraries(pcap_analysis vtkHybrid vtkWidgets)
endif()

target_link_libraries(pcap_analysis ${PROJECT_LINK_LIBS} )
