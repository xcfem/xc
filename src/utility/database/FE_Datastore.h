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
** (C) Copyright 1999, The Regents of the University of California    **
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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/08 00:04:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/database/FE_Datastore.h,v $
                                                                        
                                                                        
#ifndef FE_Datastore_h
#define FE_Datastore_h

// File: ~/utility/database/FE_Datastore.h
//
// Written: fmk 
// Created: 10/98
// Revision: A
//
// Description: This file contains the class definition for FE_Datastore.
// FE_Datastore is an abstract base class. An FE_datastore object is used
// in the program to store/restore the geometry and state information in
// a modelador at a particular instance in the analysis.
//
// What: "@(#) FE_Datastore.h, revA"

#include "utility/actor/channel/Channel.h"

namespace XC {
class Modelador;
class FEM_ObjectBroker;

class FE_Datastore: public Channel
  {
  private:
    FEM_ObjectBroker *theObjectBroker;
    Modelador *mdlr;
    static int lastDbTag;
    std::set<int> savedStates;
  protected:
    FEM_ObjectBroker *getObjectBroker(void);
    const Modelador *get_modelador(void) const
      { return mdlr; }
  public:
    FE_Datastore(Modelador &, FEM_ObjectBroker &theBroker);
    inline virtual ~FE_Datastore(void) {} 

    // methods defined in the Channel class interface which mean nothing for a Datastore
    char *addToProgram(void);
    int setUpConnection(void);
    int setNextAddress(const ChannelAddress &otherChannelAddress);
    ChannelAddress *getLastSendersAddress(void);

    // methods defined in the Channel class interface which can be implemented here    
    int sendObj(int commitTag,MovableObject &theObject, ChannelAddress *theAddress =0);

    int recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &theBroker,ChannelAddress *theAddress =0);
		
    // pure virtual functions in addition to those defined
    // in the ModelBuilder and Channel classes for database applications
    int getDbTag(void) const;
    virtual bool isDatastore(void) const;

    virtual int commitState(int commitTag);
    virtual int restoreState(int commitTag);
    bool isSaved(int commitTag) const;

    virtual int createTable(const std::string &tableName, const std::vector<std::string> &);
    virtual int insertData(const std::string &tableName,const std::vector<std::string> &, int commitTag, const Vector &data);
    virtual int getData(const std::string &tableName,const std::vector<std::string> &, int commitTag, Vector &data);

    int save(const int &commitTag);
    int restore(const int &commitTag);

  };
} // fin namespace XC


#endif

