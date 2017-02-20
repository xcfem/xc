//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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

//ProblemaEF.cc

#include "ProblemaEF.h"

//Salida de resultados.
#include "utility/handler/DataOutputHandler.h"
#include "utility/handler/DataOutputFileHandler.h"
#include "utility/handler/DataOutputDatabaseHandler.h"
#include "utility/handler/DataOutputStreamHandler.h"
#include "utility/database/FE_Datastore.h"


#include "utility/actor/objectBroker/FEM_ObjectBrokerAllClasses.h"
#include "utility/database/FileDatastore.h"
#include "utility/database/MySqlDatastore.h"
#include "utility/database/BerkeleyDbDatastore.h"
#include "utility/database/SQLiteDatastore.h"

#include "domain/mesh/Mesh.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "utility/med_xc/MEDMeshing.h"
#include "utility/med_xc/MEDMesh.h"

XC::FEM_ObjectBrokerAllClasses XC::ProblemaEF::theBroker;
XC::Domain *XC::ProblemaEF::theActiveDomain= nullptr;

//! @brief Default constructor.
XC::ProblemaEF::ProblemaEF(void)
  : preprocessor(this,&output_handlers),proc_solu(this), dataBase(nullptr) {}

//! @brief Database definition.
XC::FE_Datastore *XC::ProblemaEF::defineDatabase(const std::string &tipo, const std::string &nombre)
  {
    if(dataBase)
      {
        delete dataBase;
        dataBase= nullptr;
      }
    if(tipo == "File")
      dataBase= new FileDatastore(nombre, preprocessor, theBroker);
    else if(tipo == "MySql")
      dataBase= new MySqlDatastore(nombre, preprocessor, theBroker);
    else if(tipo == "BerkeleyDB")
      dataBase= new BerkeleyDbDatastore(nombre, preprocessor, theBroker);
    else if(tipo == "SQLite")
      dataBase= new SQLiteDatastore(nombre, preprocessor, theBroker);
    else
      {  
        std::cerr << "WARNING No database type exists ";
        std::cerr << "for database of type:" << tipo << "valid database type File\n";
      }
    if(!dataBase)
      std::cerr << "WARNING ran out of memory - database File " << nombre << std::endl;
    return dataBase; 
  }

XC::ProblemaEF::~ProblemaEF(void)
  { clearAll(); }

//! @brief Borra todas las entidades del problema.
void XC::ProblemaEF::clearAll(void)
  {
    for(DataOutputHandler::map_output_handlers::iterator i= output_handlers.begin();i!=output_handlers.end();i++)
      {
        DataOutputHandler *tmp= (*i).second;
        if(tmp) delete tmp;
      }
    output_handlers.clear();
    fields.clearAll();
    proc_solu.clearAll();
    preprocessor.clearAll();
  }
