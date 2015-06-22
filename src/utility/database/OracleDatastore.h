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
                                                                        
#ifndef OracleDatastore_h
#define OracleDatastore_h

// $Revision: 1.2 $
// $Date: 2005/11/07 21:34:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/OracleDatastore.h,v $

// Written:  Jun Peng 
//
// Description: This file contains the class definition for OracleDatastore.
// OracleDatastore is a concrete subclas of DBDatastore. A OracleDatastore 
// object is used in the program to store/restore the geometry and state 
// information in a modelador at a particular instance in the analysis. The
// information is stored in ORACLE8i Database.
//
// What: "@(#) OracleDatastore.h, revA"

#include "DBDatastore.h"
#include <fstream>

namespace XC {
class FEM_ObjectBroker;


class OracleDatastore: public DBDatastore
  {
  private:
    
    // private attributes
    int projTag;
    std::string dataBase;

    int lastDomainChangeStamp;
  public:
    OracleDatastore(const std::string &dataBase,Modelador &mdlr, FEM_ObjectBroker &theBroker);    
    
    ~OracleDatastore(void);

    // methods to set and get a project tag
    int getProjTag();
    void setProjTag(int projectID);

    // method to search for project Tag
    int searchProjTag(const std::string &projName);

    // methods defined in the Channel class interface which do nothing
    char *addToProgram(void);
    int setUpShadow(void);
    int setUpActor(void);
    int setNextAddress(const ChannelAddress &otherChannelAddress);
    ChannelAddress *getLastSendersAddress(void);
    
    int commitState(int commitTag);        

    // methods for sending and receiving the data
    int sendObj(int commitTag,MovableObject &,ChannelAddress *theAddress= NULL);
    int recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &,ChannelAddress *theAddress= NULL);
                
    int sendMsg(int dbTag, int commitTag,const Message &,ChannelAddress *theAddress= NULL);    
    int recvMsg(int dbTag, int commitTag,Message &,ChannelAddress *theAddress= NULL);        

    int sendMatrix(int dbTag, int commitTag,const Matrix &,ChannelAddress *theAddress= NULL);
    int recvMatrix(int dbTag, int commitTag,Matrix &,ChannelAddress *theAddress= NULL);
    
    int sendVector(int dbTag, int commitTag,const Vector &, ChannelAddress *theAddress= NULL);
    int recvVector(int dbTag, int commitTag,Vector &,ChannelAddress *theAddress= NULL);
    
    int sendID(int dbTag, int commitTag, const ID &, ChannelAddress *theAddress= NULL);
    int recvID(int dbTag, int commitTag, ID &theID, ChannelAddress *theAddress= NULL);    
  };
} // fin namespace XC


#endif

