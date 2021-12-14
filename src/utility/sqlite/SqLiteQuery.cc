//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//SqLiteQuery

#include "SqLiteQuery.h"
#include "SqLiteDatabase.h"
#include "utility/utils/sqlitepp/Database.h"

//! @brief Constructor.
SqLiteQuery::SqLiteQuery(Database &db)
  : SqLiteObject(), q(db) {}

//! @brief Constructor.
SqLiteQuery::SqLiteQuery(SqLiteDatabase &db)
  : SqLiteObject(), q(db.getDB()) {}

//! @brief Constructor.
SqLiteQuery::SqLiteQuery(Database &db,const std::string &sql)
  : SqLiteObject(), q(db,sql) {}

int SqLiteQuery::GetErrno(void)
  { return q.GetErrno(); }

std::string SqLiteQuery::getError(void) const
  { return q.getError(); }

bool SqLiteQuery::execute(const std::string &sql)
  { return q.execute(sql); }

bool SqLiteQuery::insert_blob1(const std::string &sql,const void *blobData,const size_t &numBytes)
  { return q.insert_blob1(sql,blobData,numBytes); }


sqlite3_stmt *SqLiteQuery::get_result(const std::string &sql)
  { return q.get_result(sql); }

void SqLiteQuery::free_result(void)
  { q.free_result(); }

bool SqLiteQuery::fetch_row(void) const
  { return const_cast<Query *>(&q)->fetch_row(); }

long SqLiteQuery::num_rows(void) const
  { return const_cast<Query *>(&q)->num_rows(); }

//! @brief Return el valor del campo denominado fieldName como BLOB.
const void *SqLiteQuery::getBlob(const std::string &fieldName) const
  { return const_cast<Query *>(&q)->getblob(fieldName); }

//! @brief Return el valor del campo denominado fieldName como text string.
std::string SqLiteQuery::getStr(const std::string &fieldName) const
  { return std::string(const_cast<Query *>(&q)->getstr(fieldName)); }

//! @brief Return el valor del campo denominado fieldName como entero.
int SqLiteQuery::getInt(const std::string &fieldName) const
  { return static_cast<int>(getLongInt(fieldName)); }

//! @brief Return el valor del campo denominado fieldName como entero sin signo.
size_t SqLiteQuery::getSizeT(const std::string &fieldName) const
  { return static_cast<size_t>(getLongInt(fieldName)); }

//! @brief Return el valor del campo denominado fieldName como entero largo.
long int SqLiteQuery::getLongInt(const std::string &fieldName) const
  { return const_cast<Query *>(&q)->getval(fieldName); }

//! @brief Return el valor del campo denominado fieldName como entero largo sin signo.
unsigned long int SqLiteQuery::getULongInt(const std::string &fieldName) const
  { return const_cast<Query *>(&q)->getuval(fieldName); }

//! @brief Return el valor del campo denominado fieldName como double.
double SqLiteQuery::getDouble(const std::string &fieldName) const
  { return const_cast<Query *>(&q)->getnum(fieldName); }


//! @brief Return el valor del campo de índice 'index' como BLOB.
const void *SqLiteQuery::getBlob(const int &index) const
  { return const_cast<Query *>(&q)->getblob(index); }

//! @brief Return el valor del campo de índice 'index' como text string.
std::string SqLiteQuery::getStr(const int &index) const
  { return std::string(const_cast<Query *>(&q)->getstr(index)); }

//! @brief Return el valor del campo de índice 'index' como entero.
int SqLiteQuery::getInt(const int &index) const
  { return static_cast<int>(getLongInt(index)); }

//! @brief Return el valor del campo de índice 'index' como entero sin signo.
size_t SqLiteQuery::getSizeT(const int &index) const
  { return static_cast<size_t>(getLongInt(index)); }

//! @brief Return el valor del campo de índice 'index' como entero largo.
long int SqLiteQuery::getLongInt(const int &index) const
  { return const_cast<Query *>(&q)->getval(index); }

//! @brief Return el valor del campo de índice 'index' como entero largo sin signo.
unsigned long int SqLiteQuery::getULongInt(const int &index) const
  { return const_cast<Query *>(&q)->getuval(index); }

//! @brief Return el valor del campo de índice 'index' como double.
double SqLiteQuery::getDouble(const int &index) const
  { return const_cast<Query *>(&q)->getnum(index); }

//! @brief Return el valor del siguiente campo como text string.
std::string SqLiteQuery::getStr(void) const
  { return std::string(const_cast<Query *>(&q)->getstr()); }

//! @brief Return el valor del siguiente campo como entero.
int SqLiteQuery::getInt(void) const
  { return static_cast<int>(getLongInt()); }

//! @brief Return el valor del siguiente campo como entero sin signo.
size_t SqLiteQuery::getSizeT(void) const
  { return static_cast<size_t>(getLongInt()); }

//! @brief Return el valor del siguiente campo como entero largo.
long int SqLiteQuery::getLongInt(void) const
  { return const_cast<Query *>(&q)->getval(); }

//! @brief Return el valor del siguiente campo como entero largo sin signo.
unsigned long int SqLiteQuery::getULongInt(void) const
  { return const_cast<Query *>(&q)->getuval(); }

//! @brief Return el valor del siguiente campo como double.
double SqLiteQuery::getDouble(void) const
  { return const_cast<Query *>(&q)->getnum(); }


