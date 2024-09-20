# ##  TEMPLATE.txt.tpl; coding: utf-8 ---

# Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
# Date: 2020-06-03
#
# Copyright (C) 2020 Cemosis
#
# Distributed under the GPL(GNU Public License):
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if(MAGNETTOOLS_FOUND)
    message(STATUS "[MagnetTools] \nincludes: ${MAGNETTOOLS_INCLUDE_DIR} \nlibraries: ${MAGNETTOOLS_LIBRARY}")
    include_directories(${GSL_INCLUDES})
    include_directories(${FREESTEAM_INCLUDES})
    include_directories(${GNUPLOT_IOSTREAM_INCLUDE_DIRS})
    include_directories(${MAGNETTOOLS_INCLUDE_DIR})
endif()

feelpp_add_application(cfpdes_mag SRCS cfpdes_mag.cpp LINK_LIBRARIES Feelpp::feelpp_toolbox_coefficientformpdes_lib)
feelpp_add_application(cfpdes_mag_amap SRCS cfpdes_mag_amap.cpp LINK_LIBRARIES Feelpp::feelpp_toolbox_coefficientformpdes_lib ${FEELPP_LIBRARIES} ${GSL_LIBRARIES} ${FREESTEAM_LIB} ${MAGNETTOOLS_LIBRARIES})

feelpp_add_application(cfpdes_mag_biot_savart SRCS cfpdes_mag_biot_savart.cpp LINK_LIBRARIES Feelpp::feelpp_toolbox_coefficientformpdes_lib)
feelpp_add_application(cfpdes_mag_biot_savart_pdhv0 SRCS cfpdes_mag_biot_savart_pdhv0.cpp LINK_LIBRARIES Feelpp::feelpp_toolbox_coefficientformpdes_lib)
feelpp_add_application(cfpdes_elas SRCS cfpdes_elas.cpp LINK_LIBRARIES Feelpp::feelpp_toolbox_coefficientformpdes_lib)

# if ( FEELPP_TOOLBOXES_FOUND )
# feelpp_add_application(toolbox SRCS toolbox.cpp LINK_LIBRARIES Feelpp::feelpp_toolbox_electric_lib TESTS)
# feelpp_add_testcase(cases)
# endif()