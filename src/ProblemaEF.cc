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
#include "xc_utils/src/base/CmdStatus.h"

//Salida de resultados.
#include "utility/handler/DataOutputHandler.h"
#include "utility/handler/DataOutputFileHandler.h"
#include "utility/handler/DataOutputDatabaseHandler.h"
#include "utility/handler/DataOutputStreamHandler.h"
#include "utility/database/FE_Datastore.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "utility/actor/objectBroker/FEM_ObjectBrokerAllClasses.h"
#include "utility/database/FileDatastore.h"
#include "utility/database/MySqlDatastore.h"
#include "utility/database/BerkeleyDbDatastore.h"
#include "utility/database/SQLiteDatastore.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
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

//! @brief Lee un objeto ProblemaEF desde archivo
bool XC::ProblemaEF::procesa_comando(CmdStatus &status)
  {
    static FEM_ObjectBroker theBroker;
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ProblemaEF) Procesando comando: " << cmd << std::endl;
    if(cmd=="preprocessor")
      {
        preprocessor.LeeCmd(status);
        return true;
      }
    else if(cmd=="sol_proc")
      {
        proc_solu.LeeCmd(status);
        return true;
      }
    else if(cmd=="fields")
      {
        fields.LeeCmd(status);
        return true;
      }
    else if(cmd == "file_handler")
      {
        const CmdParser &parser= status.Parser();
        std::string cod_file_handler= "nil";
        if(parser.TieneIndices())
          {
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              cod_file_handler= convert_to_string(fnc_indices[0]);
          }
        DataOutputFileHandler *fh= new DataOutputFileHandler();
        fh->LeeCmd(status);
        output_handlers[cod_file_handler]= fh;
        return true;
      }
    else if(cmd == "database_handler")
      {
        const CmdParser &parser= status.Parser();
        std::string cod_file_handler= "nil";
        if(parser.TieneIndices())
          {
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              cod_file_handler= convert_to_string(fnc_indices[0]);
          }
        DataOutputDatabaseHandler *fh= new DataOutputDatabaseHandler();
        fh->LeeCmd(status);
        output_handlers[cod_file_handler]= fh;
        return true;
      }
    else if(cmd == "stream_handler")
      {
        const CmdParser &parser= status.Parser();
        std::string cod_file_handler= "nil";
        std::deque<std::string> fnc_indices;
        if(parser.TieneIndices())
          {
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              cod_file_handler= convert_to_string(fnc_indices[0]);
          }
        DataOutputStreamHandler *fh= new DataOutputStreamHandler();
        fh->LeeCmd(status);
        output_handlers[cod_file_handler]= fh;
        return true;
      }
    else if(cmd == "database")
      {
        if(dataBase)
          dataBase->LeeCmd(status);
        else
          {
            const std::string str_error= "uso: \\database[nombre,tipo]{...}.";
            const CmdParser &parser= status.Parser();
            if(parser.TieneIndices())
              {
                interpreta(parser.GetIndices());
                if(InterpreteRPN::HayArgumentos(2,cmd))
                  {
                    const std::string tipo= convert_to_string(InterpreteRPN::Pila().Pop());
                    const std::string nombre= convert_to_string(InterpreteRPN::Pila().Pop());
                    defineDatabase(tipo,nombre);
                  }
                else
	          std::cerr << str_error << std::endl;
              }
            else
	      std::cerr << str_error << std::endl;
            if(dataBase)
              dataBase->LeeCmd(status);
            else
              {
                status.GetBloque();
	        std::cerr << "No se ha definido el objeto base de datos, se ignora la entrada" << std::endl;
              }
          }
        return true;
      }
    else if(cmd == "fileDataStore")
      {
        const std::string fName= status.GetString();
        if(dataBase)
          delete dataBase;
        dataBase= new FileDatastore(fName,preprocessor,theBroker);
        return true;
      }
    else if(cmd=="clearAll")
      {
        status.GetString(); //Ignoramos entrada.
        clearAll();
        return true;
      }
    else if(cmd == "med_export")
      {
        MEDMeshing med_xc(*this);
        med_xc.set_owner(this);
        med_xc.LeeCmd(status);
        return true;
      }
    else if(cmd == "med_import")
      {
        MEDMesh med_mesh;
        med_mesh.set_owner(this);
        med_mesh.LeeCmd(status);
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
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

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
any_const_ptr XC::ProblemaEF::GetProp(const std::string &cod) const
  { return EntCmd::GetProp(cod); }
