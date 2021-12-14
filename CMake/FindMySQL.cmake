# ----------------------------------------------------------------------------
#   XC program; finite element analysis code
#   for structural analysis and design.
# 
#   Copyright (C)  Luis C. Pérez Tato
# 
#   Except for the restrictions that may arise from the copyright
#   of the original program (see copyright below if any)
#   XC is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or 
#   (at your option) any later version.
# 
#   This software is distributed in the hope that it will be useful, but 
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details. 
# 
# 
#  You should have received a copy of the GNU General Public License 
#  along with this program.
#  If not, see <http:// www.gnu.org/licenses/>.
# ----------------------------------------------------------------------------
#
# Find the MySQL client includes and library
# 

# This module defines
# MYSQL_INCLUDE_DIRECTORIES, where to find mysql.h
# MYSQL_LIBRARIES, the libraries to link against to connect to MySQL
# MYSQL_FOUND, If false, you cannot build anything that requires MySQL.

# also defined, but not for general use are
# MYSQL_LIBRARY, where to find the MySQL library.

# 
# XXX NOTE: This is not yet for general use.  I'm pretty sure there
# are other libraries I have to link against at the same time.
#

SET( MYSQL_FOUND 0 )

find_path(MYSQL_INCLUDE_DIRECTORIES mysql.h
  /usr/include
  /usr/include/mysql
  /usr/local/include
  /usr/local/include/mysql
  /usr/local/mysql/include
  "C:/Program Files/MySQL/include"
  "C:/Program Files/MySQL/MySQL Server 5.0/include"
  "C:/MySQL/include"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.0;Location]/include"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.0;Location]/include"
  DOC "Specify the directory containing mysql.h."
)

FIND_LIBRARY( MYSQL_LIBRARY 
  NAMES mysql libmysql mysqlclient
  PATHS
  /usr/lib
  /usr/lib/mysql
  /usr/local/lib
  /usr/local/lib/mysql
  /usr/local/mysql/lib
  "C:/Program Files/MySQL/lib"
  "C:/Program Files/MySQL/MySQL Server 5.0/lib/opt"
  "C:/MySQL/lib/debug"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server 5.0;Location]/lib/opt"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server 5.0;Location]/lib/opt"
  DOC "Specify the mysql library here."
)

# On Windows you typically don't need to include any extra libraries
# to build MYSQL stuff.

IF (NOT WIN32)
  FIND_LIBRARY( MYSQL_EXTRA_LIBRARIES
                NAMES z zlib
                PATHS /usr/lib /usr/local/lib 
                DOC "If more libraries are necessary to link in a MySQL client (typically zlib), specify them here.")
ELSE (NOT WIN32)
  SET( MYSQL_EXTRA_LIBRARIES "" )
ENDIF (NOT WIN32)


IF (MYSQL_LIBRARY)
  IF (MYSQL_INCLUDE_DIRECTORIES)
    SET( MYSQL_FOUND 1 )
  ENDIF (MYSQL_INCLUDE_DIRECTORIES)
ENDIF (MYSQL_LIBRARY)

MARK_AS_ADVANCED( MYSQL_FOUND MYSQL_LIBRARY MYSQL_EXTRA_LIBRARIES MYSQL_INCLUDE_DIRECTORIES )
