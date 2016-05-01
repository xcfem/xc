//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

//! @brief Constructor por defecto.
XC::ProblemaEF::ProblemaEF(void)
  : preprocessor(this,&output_handlers),proc_solu(this), dataBase(nullptr) {}

//! @brief Define el objeto base de datos.
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
