#########################################################################
#  Project:		Phonon Command Line Player			#
#  Edit:		08-Nov-18					#
#########################################################################

project(phononplay)

#########################################################################
#									#
#  Dependencies								#
#									#
#########################################################################

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS I18n CoreAddons Crash)

find_package(Phonon4Qt5 NO_POLICY_SCOPE)
set_package_properties("Phonon4Qt5" PROPERTIES DESCRIPTION "Multimedia library" URL "http://phonon.kde.org" TYPE REQUIRED PURPOSE "Required for audio/video playback")

#########################################################################
#									#
#  Rigourousness							#
#									#
#########################################################################

add_definitions("-DQT_USE_FAST_CONCATENATION")
add_definitions("-DQT_USE_FAST_OPERATOR_PLUS")
add_definitions("-DQT_NO_CAST_FROM_BYTEARRAY")
add_definitions("-DQT_NO_URL_CAST_FROM_STRING")
add_definitions("-DQT_NO_CAST_TO_ASCII")
add_definitions("-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT")
add_definitions("-DQT_DEPRECATED_WARNINGS")

remove_definitions("-DQT_NO_CAST_FROM_ASCII")
remove_definitions("-DQT_NO_SIGNALS_SLOTS_KEYWORDS")

#########################################################################
#									#
#  Building the target - phononplay					#
#									#
#########################################################################

set(phononplay_SRCS phononplay.cpp)

add_definitions("-DVERSION=\\\"${VERSION}\\\"")

add_executable(phononplay ${phononplay_SRCS})
target_include_directories(phononplay PRIVATE ${PHONON_INCLUDES})
target_include_directories(phononplay PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/..)
target_link_libraries(phononplay Qt5::Core KF5::I18n KF5::CoreAddons KF5::Crash Phonon::phonon4qt5)

#########################################################################
#									#
#  Building the target - vlcplay					#
#									#
#########################################################################

set(vlcplay_SRCS vlcplay.c)

add_executable(vlcplay ${vlcplay_SRCS})
target_include_directories(vlcplay PRIVATE "/usr/include/vlc")
target_compile_options(vlcplay PRIVATE "--std=c99")
target_link_libraries(vlcplay vlc)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

install(TARGETS phononplay ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS vlcplay ${INSTALL_TARGETS_DEFAULT_ARGS})
