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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/22 19:41:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/ImposedMotionSP1.cpp,v $
                                                                        
// Written: fmk 
// Created: 11/00
// Revision: A
//
// Purpose: This file contains the implementation of class XC::ImposedMotionSP1.

#include <domain/constraints/ImposedMotionSP1.h>
#include <classTags.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/load/groundMotion/GroundMotion.h>
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include <domain/load/pattern/LoadPattern.h>
#include <utility/matrix/ID.h>

//! @brief constructor for FEM_ObjectBroker
XC::ImposedMotionSP1::ImposedMotionSP1(void)
  :ImposedMotionBase(CNSTRNT_TAG_ImposedMotionSP1),destroyMotion(0) {}

//! @brief constructor for a subclass to use
XC::ImposedMotionSP1::ImposedMotionSP1(int tag, int node, int ndof, int pattern, int motion)
 :ImposedMotionBase(CNSTRNT_TAG_ImposedMotionSP1,tag, node, ndof,pattern,motion) {}


bool XC::ImposedMotionSP1::isHomogeneous(void) const
  { return false; }

int XC::ImposedMotionSP1::applyConstraint(double time)
  {
    // on first 
    if(theGroundMotion == 0 || theNode == 0)
      {
        int retval= getMotion();
        if(retval!=0)
          { return retval; }
      }

    // now get the response from the ground motion
    theGroundMotionResponse = theGroundMotion->getDispVelAccel(time);

    return 0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::ImposedMotionSP1::sendData(CommParameters &cp)
  {
    int result= ImposedMotionBase::sendData(cp);
    result+= cp.sendInt(destroyMotion,getDbTagData(),CommMetaData(9));
    return result;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::ImposedMotionSP1::recvData(const CommParameters &cp)
  {
    int res= ImposedMotionBase::recvData(cp);
    res+= cp.receiveInt(destroyMotion,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ImposedMotionSP1::sendSelf(CommParameters &cp)
  {
    static ID data(10);
    int result= sendData(cp);
    const int dbTag = this->getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "ImposedMotionSP1::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ImposedMotionSP1::recvSelf(const CommParameters &cp)
  {
    static ID data(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ImposedMotionSP1::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

void XC::ImposedMotionSP1::Print(std::ostream &s, int flag) 
  {
    s << "ImposedMotionSP1: " << this->getTag();
    s << "\t XC::Node: " << this->getNodeTag();
    s << " DOF: " << this->getDOF_Number() << std::endl;    
  }
