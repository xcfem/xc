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
                                                                        
// $Revision: 1.2 $
// $Date: 2005/06/20 21:35:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.cpp,v $

// Written: fmk 
// Created: 05/05
//
// What: "@(#) DistributedDiagonalSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.h>
#include "utility/matrix/Vector.h"
#include <utility/actor/channel/Channel.h>

XC::DistributedDiagonalSolver::DistributedDiagonalSolver(int classTag)
:LinearSOESolver(classTag), minDiagTol(0.0), theSOE(nullptr) {}    

XC::DistributedDiagonalSolver::DistributedDiagonalSolver(double tol)
:LinearSOESolver(SOLVER_TAGS_DistributedDiagonalSolver), minDiagTol(tol), theSOE(nullptr) {}    

//! @brief Asigna el sistema de ecuaciones a resolver.
bool XC::DistributedDiagonalSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    DistributedDiagonalSOE *tmp= dynamic_cast<DistributedDiagonalSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << nombre_clase() << "::setLinearSOE: el sistema de ecuaciones no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

bool XC::DistributedDiagonalSolver::setLinearSOE(DistributedDiagonalSOE &theProfileSPDSOE)
  { return setLinearSOE(&theProfileSPDSOE); }

int XC::DistributedDiagonalSolver::setSize(void)
  { return 0; }


int XC::DistributedDiagonalSolver::solve(void)
  {
    int size = theSOE->size;
    int processID = theSOE->processID;

    ChannelQueue theChannels= theSOE->theChannels;
    int numChannels= theChannels.size();

    int numShared = theSOE->numShared;
    ID &myDOFs = theSOE->myDOFs;
    ID &myDOFsShared = theSOE->myDOFsShared;
  
    double *X = theSOE->getPtrX();
    double *B = theSOE->getPtrB();
    double *A = theSOE->A.getDataPtr();
    double *dataShared = theSOE->dataShared.getDataPtr();
    Vector *vectShared = &theSOE->vectShared;

    //
    // first copy A & B contributions to sharedData
    //

    for(int i=0; i<numShared; i++)
      dataShared[i] = 0.0;

    // assuming numShared < size .. could do an if statement

    for(int i=0; i<numShared; i++)
      {
        int dof = myDOFsShared(i);
        int loc = myDOFs.getLocation(dof);
        if(loc >= 0)
          {
            dataShared[i] = A[loc];
            dataShared[i+numShared] = B[loc];
          }
      }

  //
  // use P0 to gather & send back out
  //

  if (numShared != 0) {
    if (processID != 0) {
      Channel *theChannel = theChannels[0];
      theChannel->sendVector(0, 0, *vectShared);
      theChannel->recvVector(0, 0, *vectShared);
    } 
    else {

      static XC::Vector otherShared(1);
      otherShared.resize(2*numShared);
      for (int i=0; i<numChannels; i++) {
	Channel *theChannel = theChannels[i];
	theChannel->recvVector(0, 0, otherShared);
	*vectShared += otherShared;
      }
      for (int i=0; i<numChannels; i++) {
	Channel *theChannel = theChannels[i];
	theChannel->sendVector(0, 0, *vectShared);
      }
    }
  }
  
  
  //
  // set the corresponding A & B entries
  //
  
  
  for (int i=0; i<numShared; i++) {
    int dof = myDOFsShared(i);
    int loc = myDOFs.getLocation(dof);
    if (loc >= 0) {
      A[loc] = dataShared[i];
      B[loc] = dataShared[i+numShared];
    }
  }  

  //
  // now solve
  //
  
  for (int i=0; i<size; i++) {
    X[i] = B[i]/A[i];
  }

  return 0;
}

int XC::DistributedDiagonalSolver::sendSelf(CommParameters &cp)
  { return cp.sendDouble(minDiagTol,getDbTagData(),CommMetaData(0)); }


int XC::DistributedDiagonalSolver::recvSelf(const CommParameters &cp)
  { return cp.receiveDouble(minDiagTol,getDbTagData(),CommMetaData(0)); }


