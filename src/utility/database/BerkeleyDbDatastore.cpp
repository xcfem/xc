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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2002, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2003/10/16 00:06:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/BerkeleyDbDatastore.cpp,v $

#include "BerkeleyDbDatastore.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>
#include "preprocessor/Preprocessor.h"
#include "domain/domain/Domain.h"

#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//! @brief Initialise the db structures
int XC::BerkeleyDbDatastore::createDB(void)
  {
    int retval= db_create(&dbMatrix, dbenv, 0);
    if(retval!= 0)
      {
        std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed to init dbMatrix";
        std::cerr << db_strerror(retval) << std::endl;
        connection = false;
        return retval;
      }

    retval= db_create(&dbVector, dbenv, 0);
    if(retval!=0)
        {
          std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed to init dbVector";
          std::cerr << db_strerror(retval) << std::endl;
          connection = false;
          return retval;
        }
    retval= db_create(&dbID, dbenv, 0);
    if(retval!= 0)
      {
        std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed to init dbID";
        std::cerr << db_strerror(retval) << std::endl;
        connection = false;
        return retval;
      }
    return retval;
  }

//! @brief Abre la base de datos
int XC::BerkeleyDbDatastore::open(const DBTYPE &type)
  {
    // open the database files .. use DB_UNKNOWN first to see if database already exists
    int retval = dbMatrix->open(dbMatrix, nullptr, "Matrices.db", nullptr, type,DB_CREATE | DB_EXCL, 0664);
    if(retval!= 0)
      {
        //type = DB_UNKNOWN;
        retval = dbMatrix->open(dbMatrix, nullptr, "Matrices.db", nullptr, type, DB_CREATE, 0664);
        if(retval!=0)
          {
            std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed to open dbMatrix\n";
            std::cerr << db_strerror(retval) << std::endl;
            connection = false;     
            return retval;
          }
      }
    retval = dbVector->open(dbVector, nullptr, "Vectors.db", nullptr, type, DB_CREATE | DB_EXCL, 0664);
    if(retval!=0)
      {
        //type = DB_UNKNOWN;
        retval = dbVector->open(dbVector, nullptr, "Vectors.db", nullptr, type, DB_CREATE, 0664);
        if(retval!=0)
          {
            std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed to open dbVector\n";
            std::cerr << db_strerror(retval) << std::endl;
            connection = false;     
            return retval;
          }
      }

    retval = dbID->open(dbID, nullptr, "IDs.db", nullptr, type, DB_CREATE | DB_EXCL, 0664);
    if(retval!= 0)
      {
        DBTYPE IDtype= DB_UNKNOWN;
        retval = dbID->open(dbID, nullptr, "IDs.db", nullptr, IDtype, 0, 0664);
        if(retval!=0)
          {
            std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed to open dbID\n";
            std::cerr << db_strerror(retval) << std::endl;
            connection = false;     
            return retval;
          }
        else
          retval= dbID->get_type(dbID, &IDtype);
      }
    return retval;
  }

//! @brief Devuelve el tipo de acceso a emplear a partir
//! de la cadena de caracteres que se pasa como parámetro.
DBTYPE XC::BerkeleyDbDatastore::get_type(const std::string &dbType)
  {
    // set the database storage type
    DBTYPE retval= DB_BTREE;
    if(!dbType.empty())
      {
        if(dbType=="hash" || dbType=="HASH")
          retval = DB_HASH;
        else if(dbType=="queue" || dbType=="QUEUE")
          retval = DB_QUEUE;	 
        else if(dbType=="btree" || dbType=="BTREE")
          retval = DB_BTREE;
        else if(dbType=="recno" || dbType=="RECNO")
          retval = DB_RECNO;
	else
	  std::cerr << "BerkeleyDbDatastore::getType; no se reconoce: '"
                    << dbType << "' como tipo de almacenamiento." << std::endl;
      }
    return retval;
  }

//! @brief Cierra la base de datos
void XC::BerkeleyDbDatastore::close(void)
  {
    if(connection) 
      {
        dbMatrix->close(dbMatrix, 0);
        dbVector->close(dbVector, 0);
        dbID->close(dbID, 0);
        dbenv->close(dbenv, 0);
      }
  }

//! @brief Cierra la base de datos
bool XC::BerkeleyDbDatastore::create_aux_entities(void)
  {
    // create the directory if the database directory does not exist
    struct stat sb;
    if(stat(project.c_str(), &sb) != 0)
      {
        // Create the directory, read/write/access owner only.
        if(mkdir(project.c_str(), S_IRWXU) != 0)
          {
            std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed mkdir: ";
            std::cerr << project << " " << strerror(errno);
            connection = false;
            return false;
          }
      }

    // create the environment handle
    int ret = db_env_create(&dbenv, 0);
    if(ret!=0)
      {
        std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed db_env_create: ";
        std::cerr << db_strerror(ret) << std::endl;
        connection = false;
      }

    // Set up error handling.
    dbenv->set_errpfx(dbenv, project.c_str());

    //Set up cache size.
    const Preprocessor *modelador= get_modelador();
    if(modelador)
      {
        const Domain *dom= modelador->GetDominio();
        if(dom)
          {
            const int numNodos= dom->getNumNodes();
            const int nBytes= std::max(numNodos*21,256)*1024; //21 KB por nodo.
            ret= dbenv->set_cachesize(dbenv,0,nBytes,0);
            if(ret!= 0)
              {
                std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed set_cachesize: ";
                std::cerr << db_strerror(ret) << std::endl;
                connection = false;
                return false;
              }
          }
      }

    // now create the enviroment
    ret= dbenv->open(dbenv, project.c_str(),DB_CREATE | DB_INIT_LOG | DB_PRIVATE | DB_INIT_MPOOL, S_IRUSR | S_IWUSR);
    if(ret!= 0)
      {
        std::cerr << "BerkeleyDbDatastore::BerkeleyDbDatastore - failed db_env_create: ";
        std::cerr << db_strerror(ret) << std::endl;
        connection = false;
        return false;
      }
    return true;
  }

//! @brief Constructor
XC::BerkeleyDbDatastore::BerkeleyDbDatastore(const std::string &projectName, Preprocessor &preprocessor,FEM_ObjectBroker &theObjectBroker,const std::string &dbType)
  :DBDatastore(preprocessor, theObjectBroker), connection(true), project(projectName)
  {
    if(create_aux_entities())
      {
        int result= createDB();
        // set the database storage type
        DBTYPE type= get_type(dbType);
        result+= open(type);
      }
  }


XC::BerkeleyDbDatastore::~BerkeleyDbDatastore(void)
  {
    std::cerr << "CLOSING DATABASE\n";
    close();
  }


int XC::BerkeleyDbDatastore::getDbTag(void) const
  {
    dbTAG++;
    return dbTAG;
  }


int XC::BerkeleyDbDatastore::sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress)
  {
    std::cerr << "BerkeleyDbDatastore::sendMsg() - not yet implemented\n";
    return -1;
  }		       

int XC::BerkeleyDbDatastore::recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress)
  {
    std::cerr << "BerkeleyDbDatastore::recvMsg() - not yet implemented\n";
    return -1;
  }		       

//! @brief set up the key structure
void XC::BerkeleyDbDatastore::setup_key(const int &dbTag,const int &commitTag,const int &sz)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en BerkeleyDbDatastore::setup_key." << std::endl;
    static char q[128];
    sprintf(q,"%d-%d-%d", commitTag, dbTag, sz);
    key.data = q;
    key.size = strlen(q);
    key.ulen = key.size;
    key.doff = 0;
    key.flags = DB_DBT_USERMEM;
  }

//! @brief set up the data structure
void XC::BerkeleyDbDatastore::setup_data(const void *dat,const int &sz,const int &szTipo)
  {
    data.data= const_cast<void *>(dat);
    data.size= sz * szTipo;
    data.ulen= data.size;
    data.doff= 0;
    data.flags = DB_DBT_USERMEM;
  }

//! @brief set up structures
void XC::BerkeleyDbDatastore::setup_structures(const int &dbTag,const int &commitTag,const void *dat,const int &sz,const int &szTipo)
  {
    setup_key(dbTag,commitTag,sz);
    setup_data(dat,sz,szTipo);
  }

int XC::BerkeleyDbDatastore::sendMatrix(int dbTag, int commitTag, const Matrix &theMatrix, ChannelAddress *theAddress)
  {
    
    if(connection) // check that we have a connection
      {
        // set up the key structure
        setup_structures(dbTag,commitTag,theMatrix.getDataPtr(),theMatrix.getDataSize(),sz_dbl);

        int ret;
        if((ret = dbMatrix->put(dbMatrix, nullptr, &key, &data, 0)) != 0)
          {
            std::cerr << "BerkeleyDbDatastore::sendMatrix() - failed to send the Matrix to database\n";
            dbMatrix->err(dbMatrix, ret, "DB->put");
            return -2;      
          }
        return 0;
      }
    else
      return -1;
  }
 

int XC::BerkeleyDbDatastore::recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress)
  {
    // check that we have a connection
    if(connection)
      {
        // set up the key structure
        setup_structures(dbTag,commitTag,theMatrix.getDataPtr(),theMatrix.getDataSize(),sz_dbl);

        int ret;
        if((ret = dbMatrix->get(dbMatrix, nullptr, &key, &data, 0)) != 0)
          {
            std::cerr << "BerkeleyDbDatastore::recvMatrix() - failed to get the XC::Matrix from database\n";
            dbMatrix->err(dbMatrix, ret, "DB->get");
            return -2;      
          }
         return 0;
       }
    else
      return -1;
  }


int XC::BerkeleyDbDatastore::sendVector(int dbTag, int commitTag, const Vector &theVector, ChannelAddress *theAddress)
  {
    // check that we have a connection
    if(connection)
      {

        // set up the key structure
        setup_structures(dbTag,commitTag,theVector.getDataPtr(),theVector.Size(),sz_dbl);
        int ret;
        if((ret = dbVector->put(dbVector, nullptr, &key, &data, 0)) != 0)
          {
            std::cerr << "BerkeleyDbDatastore::sendVector() - failed to send the Vector to database\n";
            dbVector->err(dbVector, ret, "DB->put");
            return -2;      
          }
        return 0;
      }
    else
      return -1;
  }
 

int XC::BerkeleyDbDatastore::recvVector(int dbTag, int commitTag, Vector &theVector, ChannelAddress *theAddress)
  {
    // check that we have a connection
    if(connection)
      {
        // set up the key structure
        setup_structures(dbTag,commitTag,theVector.getDataPtr(),theVector.Size(),sz_dbl);
  
        int ret;
        if((ret = dbVector->get(dbVector, nullptr, &key, &data, 0)) != 0)
          {
            std::cerr << "BerkeleyDbDatastore::recvVector() - failed to get the XC::Vector from database\n";
            dbVector->err(dbVector, ret, "DB->get");
            return -2;      
          }
        return 0;
      }
    else
      return -1;
  }


int XC::BerkeleyDbDatastore::sendID(int dbTag, int commitTag, const ID &theID, ChannelAddress *theAddress)
  {
    if(connection)
      {
        // set up the key structure
        setup_structures(dbTag, commitTag,theID.getDataPtr(), theID.Size(),sz_int);
        int ret;
        if((ret = dbID->put(dbID, 0, &key, &data, 0)) != 0)
          {
            std::cerr << "BerkeleyDbDatastore::sendID() - failed to send the XC::ID to database\n";
            dbID->err(dbID, ret, "DB->put");
            return -2;      
          }
        return 0;
      }
    else
      return -1;
  }
 

int XC::BerkeleyDbDatastore::recvID(int dbTag, int commitTag, ID &theID,ChannelAddress *theAddress)
  {
    if(connection) // check that we have a connection
      {
        // set up the key structure
        setup_structures(dbTag, commitTag,theID.getDataPtr(), theID.Size(),sz_int);

        int ret;
        if((ret = dbID->get(dbID, nullptr, &key, &data, 0)) != 0)
          {
            std::cerr << "BerkeleyDbDatastore::recvID() - failed to get the ID from database\n";
            dbID->err(dbID, ret, "DB->get");
            return -2;      
          }
        return 0;
      }
    else
      return -1;
  }
