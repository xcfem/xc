// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//SqLiteQuery.h

#ifndef SQLITEQUERY_H
#define SQLITEQUERY_H

#include "SqLiteObject.h"
#include <sqlite3.h>
#include "utility/utils/sqlitepp/IError.h"
#include "utility/utils/sqlitepp/Database.h"
#include "utility/utils/sqlitepp/Query.h"

//! @ingroup SQLITE
//
//!  @brief Clase para consulta SQL
class SqLiteQuery: public SqLiteObject
  {
    Query q; //!< Consulta SqLite.

  public:
    SqLiteQuery(Database &db);
    SqLiteQuery(SqLiteDatabase &db);
    SqLiteQuery(Database &db,const std::string &sql);

    int GetErrno(void);
    std::string getError(void) const;
    bool execute(const std::string &sql);
    sqlite3_stmt *get_result(const std::string& sql);
    void free_result();
    bool fetch_row(void) const;
    long num_rows(void) const;
    bool insert_blob1(const std::string &sql,const void *blobData,const size_t &numBytes);

    const void *getBlob(const std::string &fieldName) const;
    std::string getStr(const std::string &fieldName) const;
    int getInt(const std::string &fieldName) const;
    size_t getSizeT(const std::string &fieldName) const;
    long int getLongInt(const std::string &fieldName) const;
    unsigned long int getULongInt(const std::string &fieldName) const;
    double getDouble(const std::string &fieldName) const;

    const void *getBlob(const int &index) const;
    std::string getStr(const int &index) const;
    int getInt(const int &index) const;
    size_t getSizeT(const int &index) const;
    long int getLongInt(const int &index) const;
    unsigned long int getULongInt(const int &index) const;
    double getDouble(const int &index) const;

    std::string getStr(void) const;
    int getInt(void) const;
    size_t getSizeT(void) const;
    long int getLongInt(void) const;
    unsigned long int getULongInt(void) const;
    double getDouble(void) const;

  };
#endif
