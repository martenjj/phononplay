#########################################################################
#  Project:		Phonon Command Line Player			#
#  Edit:		17-Oct-18					#
#########################################################################
#									#
#									#
#########################################################################

project(phononplay)
set(VERSION "0.0.1")
message(STATUS "Configuring for PhononPlay version ${VERSION}")

#########################################################################
#									#
#  ECM paths and definitions						#
#									#
#########################################################################

set(QT_MIN_VERSION "5.8.0")
set(KF5_MIN_VERSION "5.40.0")
set(ECM_MIN_VERSION "1.6.0")

find_package(ECM ${ECM_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${KDE_INSTALL_FULL_DATAROOTDIR}/marble/cmake)

#########################################################################
#									#
#  Subdirectories							#
#									#
#########################################################################

add_subdirectory(play)

#########################################################################
#									#
#  Information at end of cmake run					#
#									#
#########################################################################

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
