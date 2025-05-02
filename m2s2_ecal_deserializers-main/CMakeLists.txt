cmake_minimum_required(VERSION 3.10)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
set(XIMEA_LIBRARY m3api)
set(MSGS_INCLUDE_PATH usr/include)

project(m2s2_ecal_deserializers)

find_package(eCAL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PCL 1.3 REQUIRED COMPONENTS common io)

set(PROJECT_NAME m2s2_ecal_deserializers)

# NOTE: build with ->  cmake --build . --target m2s2_ecal_deserializers
set(measurement_read_src src/ximea_measurement_read.cpp)
ecal_add_sample(${PROJECT_NAME} ${measurement_read_src})

# NOTE: build with ->  cmake --build . --target ximea
set(XIMEA_TARGET ximea)
ecal_add_sample(${XIMEA_TARGET} src/deserialize_ximea.cpp)

# NOTE: build with ->  cmake --build . --target boson_raw
set(BOSON_RAW_TARGET boson_raw)
ecal_add_sample(${BOSON_RAW_TARGET} src/deserialize_flir_boson_raw.cpp)

# NOTE: build with ->  cmake --build . --target boson_img
set(BOSON_IMG_TARGET boson_img)
ecal_add_sample(${BOSON_IMG_TARGET} src/deserialize_flir_boson_img.cpp)

# NOTE: build with ->  cmake --build . --target bme280
set(BME280_TARGET bme280)
ecal_add_sample(${BME280_TARGET} src/deserialize_bme280.cpp)

# NOTE: build with ->  cmake --build . --target radar
set(RADAR_TARGET radar)
ecal_add_sample(${RADAR_TARGET} src/deserialize_radar.cpp)

# NOTE: build with ->  cmake --build . --target lidar
set(LIDAR_TARGET lidar)
ecal_add_sample(${LIDAR_TARGET} src/deserialize_livox.cpp)

# NOTE: build with ->  cmake --build . --target realsense
set(RS_TARGET realsense)
ecal_add_sample(${RS_TARGET} src/deserialize_realsense.cpp)

set(INCLUDE_DIRS 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${OpenCV_INCLUDE_DIRS} 
    ${MSGS_INCLUDE_PATH}
    ${PCL_INCLUDE_DIRS}
)

set(LIBS
    eCAL::hdf5
    ${OpenCV_LIBS}
    ${XIMEA_LIBRARY}
    ${PCL_LIBRARIES}
)

set(DEFS
    ${PCL_DEFINITIONS})


target_compile_definitions(${PROJECT_NAME} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")
target_compile_definitions(${XIMEA_TARGET} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")
target_compile_definitions(${BOSON_RAW_TARGET} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")
target_compile_definitions(${BOSON_IMG_TARGET} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")
target_compile_definitions(${BME280_TARGET} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")
target_compile_definitions(${RADAR_TARGET} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")
target_compile_definitions(${LIDAR_TARGET} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")
target_compile_definitions(${RS_TARGET} PRIVATE MEASUREMENT_PATH="${SAMPLE_PERSON_MEASUREMENT_PATH}")


target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_include_directories(${XIMEA_TARGET} PUBLIC ${INCLUDE_DIRS}) 
target_include_directories(${BOSON_RAW_TARGET} PUBLIC ${INCLUDE_DIRS})
target_include_directories(${BOSON_IMG_TARGET} PUBLIC ${INCLUDE_DIRS}) 
target_include_directories(${BME280_TARGET} PUBLIC ${INCLUDE_DIRS}) 
target_include_directories(${RADAR_TARGET} PUBLIC ${INCLUDE_DIRS}) 
target_include_directories(${LIDAR_TARGET} PUBLIC ${INCLUDE_DIRS}) 
target_include_directories(${RS_TARGET} PUBLIC ${INCLUDE_DIRS}) 


target_link_libraries(${PROJECT_NAME} ${LIBS})
target_link_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
#target_compile_definitions(${PROJECT_NAME} ${DEFS})

target_link_libraries(${XIMEA_TARGET} ${LIBS})
target_link_directories(${XIMEA_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${BOSON_RAW_TARGET} ${LIBS})
target_link_directories(${BOSON_RAW_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${BOSON_IMG_TARGET} ${LIBS})
target_link_directories(${BOSON_IMG_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${BME280_TARGET} ${LIBS})
target_link_directories(${BME280_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${RADAR_TARGET} ${LIBS})
target_link_directories(${RADAR_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${LIDAR_TARGET} ${LIBS})
target_link_directories(${LIDAR_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


target_link_libraries(${RS_TARGET} ${LIBS})
target_link_directories(${RS_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)
target_compile_features(${XIMEA_TARGET} PRIVATE cxx_std_14)
target_compile_features(${BOSON_RAW_TARGET} PRIVATE cxx_std_14)
target_compile_features(${BOSON_IMG_TARGET} PRIVATE cxx_std_14)
target_compile_features(${BME280_TARGET} PRIVATE cxx_std_14)
target_compile_features(${RADAR_TARGET} PRIVATE cxx_std_14)
target_compile_features(${LIDAR_TARGET} PRIVATE cxx_std_14)
target_compile_features(${RS_TARGET} PRIVATE cxx_std_14)



ecal_install_sample(${PROJECT_NAME})
ecal_install_sample(${XIMEA_TARGET})
ecal_install_sample(${BOSON_RAW_TARGET})
ecal_install_sample(${BOSON_IMG_TARGET})
ecal_install_sample(${BME280_TARGET})
ecal_install_sample(${RADAR_TARGET})
ecal_install_sample(${LIDAR_TARGET})
ecal_install_sample(${RS_TARGET})





if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)

#if(MSVC)
#    set_property(TARGET ${XIMEA_TARGET} PROPERTY LINK_FLAGS "/ignore:4098,4099")
#endif(MSVC)

if(MSVC)
    set_property(TARGET ${BOSON_RAW_TARGET} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)

if(MSVC)
    set_property(TARGET ${BOSON_IMG_TARGET} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)

if(MSVC)
    set_property(TARGET ${BME280_TARGET} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)

if(MSVC)
    set_property(TARGET ${RADAR_TARGET} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)

if(MSVC)
    set_property(TARGET ${LIDAR_TARGET} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)

if(MSVC)
    set_property(TARGET ${RS_TARGET} PROPERTY LINK_FLAGS "/ignore:4098,4099")
endif(MSVC)
