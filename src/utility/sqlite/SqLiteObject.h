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
//SqLiteObject.h
//Envoltorio para objetos de SqLite++.

#ifndef SQLITEOBJECT_H
#define SQLITEOBJECT_H

#include "../kernel/CommandEntity.h"

class SqLiteDatabase;
class SqLiteQuery;

//! @defgroup SQLITE Interfaz con la biblioteca SqLite.
//
//! @ingroup SQLITE
//
//!  @brief Clase base de los objetos de base de datos.
class SqLiteObject: public CommandEntity
  {
  public:
    typedef std::map<std::string,SqLiteObject *> map_sql_objs;//!< Contenedor de objetos de base de datos.
    typedef map_sql_objs::iterator sql_objs_iterator;//!< Iterador a objetos de base de datos.
    typedef map_sql_objs::const_iterator sql_objs_const_iterator;//!< Iterador a objetos de base de datos.
  protected:
    static map_sql_objs SqLiteObjs;//!< Contenedor de objetos de SqLite.

    static void borra_queries(void);
    static void borra_databases(void);
    static void borra_objetos(void);
    static void insert_object(const std::string &nmb,SqLiteObject *ptr);
    static SqLiteObject *find_object(const std::string &nmb);

    std::string preprocesa_str_sql(const std::string &str) const;
  public:
    SqLiteObject(void);
    virtual ~SqLiteObject(void);

    static void NuevaDatabase(const std::string &nmb);
    static void NuevaQuery(const std::string &nmb,const std::string &nmb_db);

    static void BorraObjeto(const std::string &nmb);
    static void clearAll(void);

    static SqLiteDatabase *getDatabase(const std::string &nmb);
    static SqLiteQuery *getQuery(const std::string &nmb);
  };
#endif
