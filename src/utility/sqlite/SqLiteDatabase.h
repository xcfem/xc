// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//SqLiteDatabase.h

#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include "SqLiteObject.h"
#include <sqlite3.h>
#include "utility/utils/sqlitepp/IError.h"
#include "utility/utils/sqlitepp/Database.h"
#include "utility/utils/sqlitepp/StderrLog.h"

//! @ingroup SQLITE
//
//!  @brief Clase para consulta SQL
class SqLiteDatabase: public SqLiteObject
  {
    static Database::Mutex mutex;
    static StderrLog log;
    Database db; //!< Base de datos SqLite.
    SqLiteQuery *defaultQuery;
    static sqlite3_stmt *tmpRes;
  public:
    SqLiteDatabase(const std::string &nmb);
    inline Database &getDB(void)
      { return db; }
    bool existeTabla(const std::string &nmbTabla) const;
    bool borraTabla(const std::string &nmbTabla);

    SqLiteQuery *getDefaultQuery(void);
    SqLiteQuery *NuevaQuery(const std::string &nmb);
  };
#endif
