//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------

//FEProblem.cc

#include "FEProblem.h"

//Salida de resultados.
#include "utility/handler/DataOutputHandler.h"
#include "utility/handler/DataOutputFileHandler.h"
#include "utility/handler/DataOutputDatabaseHandler.h"
#include "utility/handler/DataOutputStreamHandler.h"
#include "utility/database/FE_Datastore.h"


#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/database/FileDatastore.h"
#include "utility/database/MySqlDatastore.h"
#include "utility/database/BerkeleyDbDatastore.h"
#include "utility/database/SQLiteDatastore.h"
#include "utility/database/PyDictDatastore.h"

#include "domain/mesh/Mesh.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "utility/utils/misc_utils/colormod.h"

XC::FEM_ObjectBroker XC::FEProblem::theBroker;
XC::Domain *XC::FEProblem::theActiveDomain= nullptr;

//! @brief Default constructor.
XC::FEProblem::FEProblem(void)
  : preprocessor(this,&output_handlers),proc_solu(this), dataBase(nullptr) {}

//! @brief Database definition.
//! @param type: type of the database (File, MySql, BerkeleyDB, SQLite).
//! @param name: name of the database.
XC::FE_Datastore *XC::FEProblem::defineDatabase(const std::string &type, const std::string &name)
  {
    if(dataBase)
      {
        delete dataBase;
        dataBase= nullptr;
      }
    if(type == "File")
      dataBase= new FileDatastore(name, preprocessor, theBroker);
    else if(type == "MySql")
      dataBase= new MySqlDatastore(name, preprocessor, theBroker);
    else if(type == "BerkeleyDB")
      dataBase= new BerkeleyDbDatastore(name, preprocessor, theBroker);
    else if(type == "SQLite")
      dataBase= new SQLiteDatastore(name, preprocessor, theBroker);
    else if(type == "PyDict")
      dataBase= new PyDictDatastore(name, preprocessor, theBroker);
    else
      {  
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; WARNING No database type exists "
		  << "for database of type:" << type
		  << "valid database type File"
	          << Color::def << std::endl;
      }
    if(!dataBase)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; ran out of memory - database File "
		<< name
		<< Color::def << std::endl;
    return dataBase; 
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::FEProblem::getPyDict(void) const
  {
    boost::python::dict retval= CommandEntity::getPyDict();
    boost::python::dict oh_dict;
    DataOutputHandler::map_output_handlers::const_iterator it;
    for(it= output_handlers.begin(); it != output_handlers.end(); it++)
      { oh_dict[it->first]= it->second->getPyDict(); }
    retval["output_handlers"]= oh_dict;
    retval["preprocessor"]= preprocessor.getPyDict();
    retval["sol_proc"]= proc_solu.getPyDict();
    if(dataBase)
      {
        retval["data_base"]= dataBase->getPyDict();
        retval["data_base_name"]= dataBase->getName();
	retval["data_base_type"]= dataBase->getTypeId();
      }
    retval["object_broker"]= theBroker.getPyDict();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::FEProblem::setPyDict(const boost::python::dict &d)
  {
    CommandEntity::setPyDict(d);
    const boost::python::dict &oh_dict= boost::python::extract<boost::python::dict>(d["output_handlers"]);
    boost::python::list items= oh_dict.items();
    const size_t sz= len(items);
    if(sz>0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; reading of output handlers not implemented yet."
	          << Color::def << std::endl;
	for(size_t i=0; i<sz; i++)
	  {
	    const std::string key= boost::python::extract<std::string>(items[i][0]);
	    const boost::python::dict itemDict= boost::python::extract<boost::python::dict>(items[i][1]);
	    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		      << " ouput handler: " << key
		      << " ignored."
		      << Color::def << std::endl;
	  }
      }
    preprocessor.setPyDict(boost::python::extract<boost::python::dict>(d["preprocessor"]));
    proc_solu.setPyDict(boost::python::extract<boost::python::dict>(d["sol_proc"]));
    if(d.has_key("data_base"))
      {
	if(!dataBase)
	  {
	    const std::string dbName= boost::python::extract<std::string>(d["data_base_name"]);
	    const std::string dbType= boost::python::extract<std::string>(d["data_base_type"]);
	    defineDatabase(dbType, dbName);
	  }
        if(dataBase)
	  dataBase->setPyDict(boost::python::extract<boost::python::dict>(d["data_base"]));
      }
  }

//! @brief Destructor.
XC::FEProblem::~FEProblem(void)
  {
    clearAll();
  }

//! @brief Delete all entities in the FE problem
void XC::FEProblem::clearAll(void)
  {
    for(DataOutputHandler::map_output_handlers::iterator i= output_handlers.begin();i!=output_handlers.end();i++)
      {
        DataOutputHandler *tmp= (*i).second;
        if(tmp) delete tmp;
      }
    output_handlers.clear();
    proc_solu.clearAll();
    preprocessor.clearAll();
  }
