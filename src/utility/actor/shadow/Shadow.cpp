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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/23 18:25:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/shadow/Shadow.cpp,v $
                                                                        

// Written: fmk
// Revision: A
//
// Purpose: This file contains the implementation of XC::Shadow.
//
// What: "@(#) Shadow.C, revA"

#include <utility/actor/shadow/Shadow.h>
#include <stdlib.h>

#include <utility/actor/channel/Channel.h>
#include <utility/actor/machineBroker/MachineBroker.h>
#include <utility/actor/message/Message.h>
#include <utility/actor/actor/MovableObject.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

XC::Shadow::Shadow(Channel &theChan, FEM_ObjectBroker &myBroker)
  :theRemoteActorsAddress(nullptr), commitTag(0), theChannel(&theChan), theBroker(&myBroker)
  { theChannel->setUpConnection(); }


XC::Shadow::Shadow(Channel &theChan, FEM_ObjectBroker &myBroker, ChannelAddress &theAddress)
  :theRemoteActorsAddress(&theAddress), commitTag(0), theChannel(&theChan), theBroker(&myBroker)
  { theChannel->setUpConnection(); }

XC::Shadow::Shadow(int actorType, FEM_ObjectBroker &myBroker, MachineBroker &theMachineBroker, int compDemand)
  :theRemoteActorsAddress(nullptr), commitTag(0), theBroker(&myBroker)
  {
    // start the remote actor process running
    theChannel = theMachineBroker.startActor(actorType, compDemand);
    if(theChannel < 0)
      {
        std::cerr << "Shadow::Shadow - could not start remote actor\n";
        std::cerr << " using program " << actorType << std::endl;
        exit(-1);
      }

    // now call setUpShadow on the channel
    theChannel->setUpConnection();
    theRemoteActorsAddress = theChannel->getLastSendersAddress();
  }

int XC::Shadow::sendObject(MovableObject &theObject)
  { return theChannel->sendObj(commitTag, theObject, theRemoteActorsAddress); }

int XC::Shadow::recvObject(MovableObject &theObject)
{
    return theChannel->recvObj(commitTag, theObject,*theBroker, theRemoteActorsAddress);
}


int
XC::Shadow::recvMessage(Message &theMessage)
{
    return theChannel->recvMsg(0, commitTag, theMessage, theRemoteActorsAddress);
}

int
XC::Shadow::sendMessage(const XC::Message &theMessage)
{
    return theChannel->sendMsg(0, commitTag, theMessage, theRemoteActorsAddress);
}

int
XC::Shadow::sendMatrix(const XC::Matrix &theMatrix)
{
    return theChannel->sendMatrix(0, commitTag, theMatrix, theRemoteActorsAddress);
}

int
XC::Shadow::recvMatrix(Matrix &theMatrix)
{
    return theChannel->recvMatrix(0, commitTag, theMatrix, theRemoteActorsAddress);
}

int
XC::Shadow::sendVector(const XC::Vector &theVector)
{
    return theChannel->sendVector(0, commitTag, theVector, theRemoteActorsAddress);
}

int
XC::Shadow::recvVector(Vector &theVector)
{
    return theChannel->recvVector(0, commitTag, theVector, theRemoteActorsAddress);
}

int
XC::Shadow::sendID(const XC::ID &theID)
{
    return theChannel->sendID(0, commitTag, theID, theRemoteActorsAddress);
}

int
XC::Shadow::recvID(ID &theID)
{
    return theChannel->recvID(0, commitTag, theID, theRemoteActorsAddress);
}


void XC::Shadow::setCommitTag(int tag)
  { commitTag = tag; }


XC::Channel *XC::Shadow::getChannelPtr(void) const
  { return theChannel; }

XC::FEM_ObjectBroker *XC::Shadow::getObjectBrokerPtr(void) const
  { return theBroker; }

XC::ChannelAddress *XC::Shadow::getActorAddressPtr(void) const
  { return theRemoteActorsAddress; }


