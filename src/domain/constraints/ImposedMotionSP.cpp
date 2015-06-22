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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/22 19:41:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/ImposedMotionSP.cpp,v $
                                                                        
// Written: fmk 
// Created: 11/00
// Revision: A
//
// Purpose: This file contains the implementation of class XC::ImposedMotionSP.

#include <domain/constraints/ImposedMotionSP.h>
#include <classTags.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/load/groundMotion/GroundMotion.h>
#include <domain/mesh/node/Node.h>
#include <domain/domain/Domain.h>
#include <domain/load/pattern/LoadPattern.h>
#include <utility/matrix/ID.h>

//! @brief Constructor.
XC::ImposedMotionSP::ImposedMotionSP(void)
  :ImposedMotionBase(CNSTRNT_TAG_ImposedMotionSP), theNodeResponse(nullptr) {}

// constructor for a subclass to use
XC::ImposedMotionSP::ImposedMotionSP(int tag, int node, int ndof, int pattern, int motion)
  :ImposedMotionBase(CNSTRNT_TAG_ImposedMotionSP,tag, node, ndof,pattern,motion), theNodeResponse(nullptr) {}

//! @brief Destructor.
XC::ImposedMotionSP::~ImposedMotionSP(void)
  {
    if(theNodeResponse) delete theNodeResponse;
  }


int XC::ImposedMotionSP::applyConstraint(double time)
  {
    // on first 
    if(theGroundMotion == 0 || theNode == 0 || theNodeResponse)
      {
        int retval= getMotion();
        if(retval!=0)
          { return retval; }

        theNodeResponse = new Vector(theNode->getNumberDOF());
        if(!theNodeResponse)
          { return -2; }
      }
  
    // now get the response from the ground motion
    theGroundMotionResponse = theGroundMotion->getDispVelAccel(time);
  
  
    //
    // now set the responses at the node
    //
  
    /* ***********************************************************
     * disp response the responsibility of constraint handler
   
     *theNodeResponse = theNode->getTrialDisp();
     (*theNodeResponse)(dofNumber) = theGroundMotionResponse(0);
     theNode->setTrialDisp(*theNodeResponse);
    *************************************************************/
  
    *theNodeResponse = theNode->getTrialVel();
    (*theNodeResponse)(dofNumber) = theGroundMotionResponse(1);
    theNode->setTrialVel(*theNodeResponse);    
  
    *theNodeResponse = theNode->getTrialAccel();
    (*theNodeResponse)(dofNumber) = theGroundMotionResponse(2);
    theNode->setTrialAccel(*theNodeResponse);        
  
    return 0;
  }


void XC::ImposedMotionSP::Print(std::ostream &s, int flag) 
  {
    s << "ImposedMotionSP: " << this->getTag();
    s << "\t XC::Node: " << this->getNodeTag();
    s << " DOF: " << this->getDOF_Number() << std::endl;
  }
