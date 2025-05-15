cmake_minimum_required(VERSION 3.10)
#Import cmake configuration file
include(arm-linux-setup.cmake)
#Projcet Creation
project(LVGL_UI)

#Configure output directory
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/release)

# Recursive search to match. cpp files in the source code directory and its subdirectories
FILE(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
list(FILTER SRCS EXCLUDE REGEX "src/business/.*")
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_enter_amount.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_enter_pin.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_payment_result.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_qrcode.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_read_card.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_select_paytype.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_setting_backlight.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_setting_net.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_setting_time.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_setting.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_signature.c") 
list(FILTER SRCS EXCLUDE REGEX "src/ui/ui_waters.c") 

# include Catalog
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/lvgl
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/lvgl/porting
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/inc
)

# Link Library Catalog
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_pub
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_pub/ymosplayer
)

#Place the generated lib executable files in their respective directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/lib)

#Add executable targets
add_executable( ${PROJECT_NAME} ${SRCS})


#Link dynamic library
set(FFMPEG_LIB asound avcodec avutil swresample swscale avformat ssl crypto ymosplayer)
set(SSTART_LIB mi_disp  mi_sys mi_common mi_scl cam_os_wrapper cam_fs_wrapper)
set(YMI_LIB pos_sdk YMI LinkSdk z)

# target_link_libraries(${PROJECT_NAME} PUBLIC binder log pos_sdk pthread dl iconv crypto ssl sqlite3 pos_devapi posmui mxml rt xutilmate APHLang freetype Common qUICS UICS)
target_link_libraries(${PROJECT_NAME} PUBLIC binder log pthread dl iconv crypto ssl sqlite3 posmui 
    mxml rt xutilmate freetype APHLang avcodec avutil swscale ${FFMPEG_LIB} ${SSTART_LIB} ${YMI_LIB})