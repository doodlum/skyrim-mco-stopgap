cmake_minimum_required(VERSION 3.21)

project(
	MCOBlockRecovery
	VERSION 1.0.1
	LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(XSEPlugin)