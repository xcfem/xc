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
//SqLiteObject

#include "SqLiteObject.h"
#include "SqLiteDatabase.h"
#include "SqLiteQuery.h"
#include "boost/algorithm/string/trim.hpp"
#include "utility/utils/sqlitepp/StderrLog.h"


SqLiteObject::map_sql_objs SqLiteObject::SqLiteObjs;

//! @brief Borra las consultas.
void SqLiteObject::borra_queries(void)
  {
    for(sql_objs_iterator i= SqLiteObjs.begin();i!=SqLiteObjs.end();i++)
      {
        SqLiteQuery *tmp= dynamic_cast<SqLiteQuery *>((*i).second);
        if(tmp)
          {
            delete (*i).second;
            (*i).second= nullptr;
            SqLiteObjs.erase(i);            
          }
      }
  }

//! @brief Borra las bases de datos.
void SqLiteObject::borra_databases(void)
  {
    borra_queries(); //Primero consultas y luego bases de datos.
    for(sql_objs_iterator i= SqLiteObjs.begin();i!=SqLiteObjs.end();i++)
      {
        SqLiteDatabase *tmp= dynamic_cast<SqLiteDatabase *>((*i).second);
        if(tmp)
          {
            delete (*i).second;
            (*i).second= nullptr;
            SqLiteObjs.erase(i);            
          }
      }
  }

//! @brief Borra todos los objetos.
void SqLiteObject::borra_objetos(void)
  {
    borra_databases(); //Borramos consultas y después bases de datos.
    for(sql_objs_iterator i= SqLiteObjs.begin();i!=SqLiteObjs.end();i++) //Borramos el resto de objetos.
      {
        delete (*i).second;
        (*i).second= nullptr;
      }
    SqLiteObjs.clear();
  }

//! @brief Inerts the object into the container.
void SqLiteObject::insert_object(const std::string &nmb,SqLiteObject *ptr)
  {
    assert(ptr);
    sql_objs_iterator i= SqLiteObjs.find(nmb);
    if(i!= SqLiteObjs.end()) //El objeto ya existe.
      {
        std::cerr << "No se pudo crear el objeto: '" 
                  << nmb << "' ya existe." << std::endl;
        delete ptr; //Liberamos la memoria.
      }
    else
      SqLiteObjs[nmb]= ptr;
  }

//! @brief Return a pointer to the object which name is being passed
//! as parameter.
SqLiteObject *SqLiteObject::find_object(const std::string &nmb)
  {
    SqLiteObject *retval= nullptr;
    sql_objs_iterator i= SqLiteObjs.find(nmb);
    if(i!= SqLiteObjs.end()) //El objeto existe.
      retval= (*i).second;
    return retval;
  }


//! @brief Constructor.
SqLiteObject::SqLiteObject(void)
  {}

//! @brief Destructor.
SqLiteObject::~SqLiteObject(void)
  {}

void SqLiteObject::clearAll(void)
  { borra_objetos(); }

//! @brief Creates the database with the name being passed as parameter.
void SqLiteObject::NuevaDatabase(const std::string &nmb)
  { 
    SqLiteObject *ptr= new SqLiteDatabase(nmb);
    if(ptr)
      insert_object(nmb,ptr);
    else
      std::cerr << "SqLiteObject::NuevoObjetoSql; el puntero al objeto es nulo." << std::endl;
  }

//! @brief Creates a query with the name being passed as parameter.
void SqLiteObject::NuevaQuery(const std::string &nmb,const std::string &nmb_db)
  { 
    SqLiteDatabase *ptr_db= getDatabase(nmb_db);
    if(ptr_db)
      ptr_db->NuevaQuery(nmb);
    else
      std::cerr << "No se encontró la base de datos: " << nmb_db << std::endl;

  }


//! @brief Preprocesa una text string que contiene una sentencia SQL.
std::string SqLiteObject::preprocesa_str_sql(const std::string &str) const
  {
    return boost::trim_copy_if(str,boost::is_any_of("\""));
  }


//! @brief Return the database with the name being passed as parameter.
SqLiteDatabase *SqLiteObject::getDatabase(const std::string &nmb)
  {
    SqLiteDatabase *retval= nullptr;
    sql_objs_iterator i= SqLiteObjs.find(nmb);
    if(i!= SqLiteObjs.end()) //El objeto existe.
      retval= dynamic_cast<SqLiteDatabase *>((*i).second);
    return retval;
  }

//! @brief Return the query with the name being passed as parameter.
SqLiteQuery *SqLiteObject::getQuery(const std::string &nmb)
  {
    SqLiteQuery *retval= nullptr;
    sql_objs_iterator i= SqLiteObjs.find(nmb);
    if(i!= SqLiteObjs.end()) //El objeto existe.
      retval= dynamic_cast<SqLiteQuery *>((*i).second);
    return retval;
  }

//! @brief Removes from container the object which name is being
//! passed as parameter.
void SqLiteObject::BorraObjeto(const std::string &nmb)
  {
    sql_objs_iterator i= SqLiteObjs.find(nmb);
    if(i!= SqLiteObjs.end()) //El objeto existe.
      {
        delete (*i).second;
        (*i).second= nullptr;
        SqLiteObjs.erase(i);
      }
  }

