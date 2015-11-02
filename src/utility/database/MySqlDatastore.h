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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/07 21:34:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/database/MySqlDatastore.h,v $

#ifndef MySqlDatastore_h
#define MySqlDatastore_h

// Written:  fmk
// Created:  02/02
//
// Description: This file contains the class definition for MySqlDatastore.
// MySqlDatastore is a concrete subclas of DBDatastore. A MySqlDatastore 
// object is used in the program to store/restore the geometry and state 
// information in a model at a particular instance in the analysis. The
// information is stored in a MySQL Database which can be local or remote.
//
// What: "@(#) MySqlDatastore.h, revA"

#include "DBDatastore.h"
#include <mysql.h>

namespace XC {
class MySqlDatastore: public DBDatastore
  {
  private:
    int dbRun;
  
    bool connection;
    MYSQL mysql;

    std::string query;
    int sizeColumnString;
  protected:
    int createOpenSeesDatabase(const std::string &projectName);
    int execute(const std::string &query);
  public:
    MySqlDatastore(const std::string &,Preprocessor &, FEM_ObjectBroker &,int dbRun = 0);    
    MySqlDatastore(const std::string &,const std::string &, const std::string &, const std::string &, unsigned int port, const std::string &, unsigned int clientFlag, Preprocessor &, FEM_ObjectBroker &, int dbRun = 0);
    ~MySqlDatastore();

    // methods for sending and recieving matrices, vectors and id's
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress= NULL);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress= NULL);        

    int sendMatrix(int dbTag, int commitTag, const Matrix &,ChannelAddress *theAddress= NULL);
    int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress= NULL);

    int sendVector(int dbTag, int commitTag, const Vector &,ChannelAddress *theAddress= NULL);
    int recvVector(int dbTag, int commitTag, Vector &,ChannelAddress *theAddress= NULL);
    
    int sendID(int dbTag, int commitTag,const ID &,ChannelAddress *theAddress= NULL);
    int recvID(int dbTag, int commitTag,ID &,ChannelAddress *theAddress= NULL);    

    int createTable(const std::string &tableName,const std::vector<std::string> &columns);
    int insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const Vector &data);
    int getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &data);

    int setDbRun(int run);
    int getDbRun(void);
  };
} // fin namespace XC

#endif
