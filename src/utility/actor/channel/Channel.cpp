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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/07 23:52:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/channel/Channel.cpp,v $
                                                                        
                                                                        
// File: ~/actor/channel/Channel.C
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of XC::Channel.
//
// What: "@(#) Channel.C, revA"

#include "utility/actor/channel/Channel.h"
#include "../message/Message.h"
#include "../actor/MovableObject.h"
#include "../objectBroker/FEM_ObjectBroker.h"
#include "boost/lexical_cast.hpp"
#include "utility/matrix/ID.h"
#include "utility/actor/address/ChannelAddress.h"

int XC::Channel::numChannel = 0;

//! @brief Constructor.
XC::Channel::Channel(void)
  {
    numChannel++;
    tag = numChannel;
  }

bool XC::Channel::isDatastore(void) const
  { return false; }

int XC::Channel::getDbTag(void) const
  { return 0; }

//! @brief Devuelve la lista de dbTags usados.
const XC::ID &XC::Channel::getUsedTags(void) const
  {
    static ID retval;
    const size_t sz= usedDbTags.size();
    retval.resize(sz);
    int cont= 0;
    for(std::set<int>::const_iterator i= usedDbTags.begin();i!=usedDbTags.end();i++,cont++)
      retval[cont]= *i;
    return retval;
  }

//! @brief Comprueba si ya se ha usado este dbTag.
bool XC::Channel::checkDbTag(const int &dbTag)
  {
    bool retval= true;
    if(usedDbTags.find(dbTag)!=usedDbTags.end())
      {
        std::cerr << "Channel::checkDbTag; el dbTag " << dbTag
                  << " ya se ha usado." << std::endl;
        retval= false;
      }
    else
      usedDbTags.insert(dbTag);
    return retval;
  }

void XC::Channel::clearDbTags(void)
  { usedDbTags.clear(); }

int XC::Channel::getTag(void) const
  { return tag; }

//! @brief Envía el objeto a través de éste canal.
int XC::Channel::sendMovable(int commitTag, MovableObject &theObject)
  {
    CommParameters cp(commitTag,*this);
    return theObject.sendSelf(cp);
  }

//! @brief Recibe el objeto a través de éste canal.
int XC::Channel::receiveMovable(int commitTag, MovableObject &theObject, FEM_ObjectBroker &theBroker)
  {
    CommParameters cp(commitTag,*this, theBroker);
    return theObject.recvSelf(cp);
  }


