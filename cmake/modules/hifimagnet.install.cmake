###  coding: utf-8 ---

#  Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
#       Date: 2013-02-08
#
#  Copyright (C) 2013-2015 Feel++ Consortium
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
#-----------------------------------------------------------------------
# Values whose defaults are relative to DATAROOTDIR.
# Store empty values in the cache and store the defaults in local
# variables if the cache values are not set explicitly. This auto-updates
# the defaults as DATAROOTDIR changes.
#
if(NOT CMAKE_INSTALL_DATADIR)
  set(CMAKE_INSTALL_DATADIR "" CACHE PATH "read-only architecture-independent data (DATAROOTDIR)")
  set(CMAKE_INSTALL_DATADIR "${CMAKE_INSTALL_DATAROOTDIR}")
endif()

if(NOT CMAKE_INSTALL_MANDIR)
  set(CMAKE_INSTALL_MANDIR "" CACHE PATH "man documentation (DATAROOTDIR/man)")
  set(CMAKE_INSTALL_MANDIR "${CMAKE_INSTALL_DATAROOTDIR}/man")
endif()

if(NOT CMAKE_INSTALL_DOCDIR)
  set(CMAKE_INSTALL_DOCDIR "" CACHE PATH "documentation root (DATAROOTDIR/doc/PROJECT_NAME)")
  set(CMAKE_INSTALL_DOCDIR "${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME}")
endif()

mark_as_advanced(
  CMAKE_INSTALL_BINDIR
  CMAKE_INSTALL_LIBDIR
  CMAKE_INSTALL_INCLUDEDIR
  CMAKE_INSTALL_DATAROOTDIR
  CMAKE_INSTALL_DATADIR
  CMAKE_INSTALL_MANDIR
  CMAKE_INSTALL_DOCDIR
  )

set(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(HIFIMAGNET_PREFIX ${CMAKE_INSTALL_PREFIX})
if (NOT HIFIMAGNET_BINDIR )
  set(HIFIMAGNET_BINDIR ${CMAKE_INSTALL_PREFIX}/bin )
endif()
if (NOT HIFIMAGNET_DATADIR )
  set(HIFIMAGNET_DATADIR ${CMAKE_INSTALL_PREFIX}/share/hifimagnet )
endif()

set(_INSTALL_HIFIMAGNET_COMMAND ${CMAKE_COMMAND})

if (NOT _D_ORDER)
  math(EXPR _D_ORDER ${PMAX}-1)
endif()

foreach(D Axi 2D 3D)
  # geo order 1
  foreach(P RANGE 1 ${PMAX} 1)
    add_custom_target(install-hifimagnet${D}_P${P}_N1
        DEPENDS hifimagnet${D}_P${P}_N1
        COMMENT "install HifiMagnet ${D} P${P} N1 main codes"
        COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=Bin${D}_P${P}_N1  -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
        )
  endforeach()
  # iso-parametric
  feelpp_min( MAX_ORDER ${NMAX} ${_D_ORDER})
  foreach( P RANGE 2 ${MAX_ORDER} )
    add_custom_target(install-hifimagnet${D}_P${P}_N${P}
      DEPENDS hifimagnet${D}_P${P}_N${P}
      COMMENT "install HifiMagnet ${D} P${P} N${P} main codes"
      COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=Bin${D}_P${P}_N${P}  -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
      )
  endforeach()
endforeach()

add_custom_target(install-hifimagnet-doc
  COMMENT "install HifiMagnet docs and examples"
  COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=Doc  -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
  )

add_custom_target(install-hifimagnet-base
  DEPENDS
  install-hifimagnet3D_P1_N1
  install-hifimagnet-doc
  COMMENT "install HifiMagnet base apps"
  COMMAND
  "${CMAKE_COMMAND}"
  -DCMAKE_INSTALL_COMPONENT=Bin  -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
  -DCMAKE_INSTALL_COMPONENT=testcases  -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake"
  )
