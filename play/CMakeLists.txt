#########################################################################
#  Project:		Phonon Command Line Player			#
#  Edit:		18-Oct-18					#
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
#  Building the target							#
#									#
#########################################################################

set(phononplay_SRCS
  phononplay.cpp
)

add_definitions("-DVERSION=\\\"${VERSION}\\\"")
include_directories(${PHONON_INCLUDES})

add_executable(phononplay ${phononplay_SRCS})
target_link_libraries(phononplay
  Qt5::Core
  KF5::I18n
  KF5::CoreAddons
  KF5::Crash
  Phonon::phonon4qt5)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

install(TARGETS phononplay ${INSTALL_TARGETS_DEFAULT_ARGS})
