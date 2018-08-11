include_directories(../iothub_client/inc)
include_directories(../iothub_service_client/inc)


add_library(iothub_client_cpp "aioth_client.cpp")
target_link_libraries(iothub_client_cpp iothub_client ${iothub_client_libs})

add_library(iothub_service_client_cpp "aioth_service.cpp")
target_link_libraries(iothub_service_client_cpp iothub_client_cpp iothub_service_client)

enable_testing()
add_subdirectory(tests)