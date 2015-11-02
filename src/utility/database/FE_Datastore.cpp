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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/07 23:53:45 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/FE_Datastore.cpp,v $
                                                                        
// Written: fmk 
// Created: 10/98
// Revision: A
//
// Description: This file contains the class implementation for XC::FE_Datastore.
// FE_Datastore is an abstract base class. An XC::FE_Datastore object is used
// in the program to store/restore the geometry and state information in
// a model at a particular instance in the analysis.
//
// What: "@(#) FE_Datastore.C, revA"

#include "utility/database/FE_Datastore.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/actor/actor/MovableObject.h>
#include "preprocessor/Preprocessor.h"
#include <utility/matrix/ID.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

int XC::FE_Datastore::lastDbTag(0);

//! @brief Constructor.
XC::FE_Datastore::FE_Datastore(Preprocessor &m, FEM_ObjectBroker &theBroker) 
  :theObjectBroker(&theBroker), preprocessor(&m), savedStates() {}

bool XC::FE_Datastore::isDatastore(void) const
  { return true; }

/********************************************************************
 *                   CHANNEL METHODS  THAT DO NOTHING               *
 ********************************************************************/

char *XC::FE_Datastore::addToProgram(void)
  { return nullptr; }

int XC::FE_Datastore::setUpConnection(void)
  { return 0; }

int XC::FE_Datastore::setNextAddress(const ChannelAddress &otherChannelAddress)
  { return 0; }


XC::ChannelAddress *XC::FE_Datastore::getLastSendersAddress(void)
  { return nullptr; }


int XC::FE_Datastore::sendObj(int commitTag,MovableObject &theObject, ChannelAddress *theAddress)
  { return sendMovable(commitTag,theObject); }

int XC::FE_Datastore::recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &theNewBroker, ChannelAddress *theAddress)
  { return receiveMovable(commitTag,theObject,theNewBroker); }

//! @brief Salva el estado en la base de datos.
int XC::FE_Datastore::commitState(int commitTag)
  {
    int res = 0;
    if(commitTag<1)
      std::cerr << "FE_Datastore::commitState se esperaba un valor de commitTag mayor que 0, se obtuvo: "
                << commitTag << std::endl;
    clearDbTags();
    if(preprocessor)
      {
        CommParameters cp(commitTag,*this);
        res = preprocessor->sendSelf(cp);
        if(res < 0)
          std::cerr << "FE_Datastore::commitState - modeler failed to sendSelf\n";
        else
          {
            ID maxlastDbTag(1);
            maxlastDbTag(0)= lastDbTag;
            if(this->sendID(-1,commitTag,maxlastDbTag) < 0)
              std::cerr << "FE_Datastore::commitState - failed to send max lastDbTag data from database - problems may arise\n";
            else
              savedStates.insert(commitTag);
	  }
      }
    return res;
  }

//! @brief Devuelve verdadero si el estado se salvó
//" con anterioridad en la base de datos.
bool XC::FE_Datastore::isSaved(int commitTag) const
  { return (savedStates.count(commitTag)>0); }

int XC::FE_Datastore::restoreState(int commitTag)
  {
    int res= 0;
    clearDbTags();
    if(isSaved(commitTag))
      {
        if(preprocessor)
          {
            CommParameters cp(commitTag,*this,*theObjectBroker);
            res= preprocessor->recvSelf(cp);
            if(res < 0)
              { std::cerr << "FE_Datastore::restoreState - preprocessor failed to recvSelf\n"; }
            ID maxlastDbTag(1);
            res= recvID(-1,commitTag,maxlastDbTag);
            if(res<0)
              std::cerr << "FE_Datastore::restoreState - failed to get max lastDbTag data from database - problems may arise\n";
            else
              lastDbTag = maxlastDbTag(0);
          }
      }
    return res;
  }

int XC::FE_Datastore::createTable(const std::string &table, const std::vector<std::string> &columns)
  {
    std::cerr << "FE_Datastore::createTable - not yet implemented\n";
    return -1;
  }


int XC::FE_Datastore::insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const XC::Vector &data)
  {
    std::cerr << "FE_Datastore::insertData - not yet implemented\n";
    return -1;
  }


int XC::FE_Datastore::getData(const std::string &table,const std::vector<std::string> &columns, int commitTag, Vector &data)
  {
    std::cerr << "FE_Datastore::getData - not yet implemented\n";
    return -1;
  }

int XC::FE_Datastore::getDbTag(void) const
  {
    lastDbTag++;
    return lastDbTag;
  }

int XC::FE_Datastore::save(const int &commitTag)
  {
    const int retval= commitState(commitTag);
    if(retval < 0)
      std::cerr << "WARNING - database failed to commitState \n";
    return retval;
  }

int XC::FE_Datastore::restore(const int &commitTag)
  {
    const int retval= restoreState(commitTag);
    if(retval < 0)
      std::cerr << "WARNING - database failed to commitState \n";
    return retval;
  }

