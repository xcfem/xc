//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//SqLiteDatabase

#include "SqLiteDatabase.h"
#include "SqLiteQuery.h"
Database::Mutex SqLiteDatabase::mutex;
StderrLog SqLiteDatabase::log;
sqlite3_stmt *SqLiteDatabase::tmpRes= nullptr;

//! @brief Constructor.
SqLiteDatabase::SqLiteDatabase(const std::string &nmb)
  : SqLiteObject(), db(mutex,nmb,&log), defaultQuery(nullptr) 
  { defaultQuery= NuevaQuery("defaultQuery"); }

//! @brief Return true if the table with the argument name exists.
bool SqLiteDatabase::existeTabla(const std::string &nmbTabla) const
  {
    SqLiteQuery tmp(const_cast<Database &>(db));
    tmpRes= tmp.get_result("select count(*) from sqlite_master where type='table' and name= '" + nmbTabla + "';");
    long int sz= 0;
    if(tmp.fetch_row())
      sz= tmp.getLongInt();
    tmp.free_result();
    return (sz>0);
  }

//! @brief Borra la tabla si existe.
bool SqLiteDatabase::borraTabla(const std::string &nmbTabla)
  { 
    if(!defaultQuery)
      defaultQuery= NuevaQuery("defaultQuery");
    return defaultQuery->execute("drop table if exists '" + nmbTabla + "';");
  }

//! @brief Return the default query.
SqLiteQuery *SqLiteDatabase::getDefaultQuery(void)
  { return defaultQuery; }

//! @brief Create the quefy with the name being passed as parameter.
SqLiteQuery *SqLiteDatabase::NuevaQuery(const std::string &nmb)
  {
    SqLiteObject *ptr= find_object(nmb);
    SqLiteQuery *retval= dynamic_cast<SqLiteQuery *>(ptr);
    if(!retval) //No existe
      {
        retval= new SqLiteQuery(db);
        if(retval)
          insert_object(nmb,retval);
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; null pointer to object." << std::endl;
      }
    else
      if(verbosity>1)
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; cann't create the query with name: '"
                  << nmb << "' object already exists."
		  << std::endl;
    return retval;
  }

