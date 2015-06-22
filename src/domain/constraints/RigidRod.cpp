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

// $Revision: 1.4 $
// $Date: 2005/01/08 01:22:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/RigidRod.cpp,v $


// File: ~/model/constraints/RigidRod.C
//
// Written: fmk 12/99
// Revised:
//
// Purpose: This file contains the class implementation for XC::RigidRod.
#include "RigidRod.h"

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>

void XC::RigidRod::setup(Domain *theDomain)
  {
    RigidBase::setDomain(theDomain);

    //get the coordinates of the two nodes - check dimensions are the same
    const Vector &crdR = nodeR->getCrds();
    const Vector &crdC = nodeC->getCrds();
    int dimR = crdR.Size();
    int dimC = crdC.Size();
    if(dimR != dimC)
      {
        std::cerr << "XC::RigidRod::RigidRod - mismatch in dimension " <<
          "between constrained XC::Node " <<  getNodeConstrained() <<  " and Retained node " << getNodeRetained() << std::endl;
        return;
      }

    // check the number of dof at each node is the same
    int numDOF = nodeR->getNumberDOF();
    if(numDOF != nodeC->getNumberDOF())
      {
        std::cerr << "XC::RigidRod::RigidRod - mismatch in numDOF " <<
          "between constrained XC::Node " <<  getNodeConstrained() <<  " and Retained node " << getNodeRetained() << std::endl;
        return;
      }

    // check the number of dof at the nodes >= dimension of problem
    if(numDOF < dimR)
      {
        std::cerr << "XC::RigidRod::RigidRod - numDOF at nodes " << getNodeRetained() << " and " << getNodeConstrained() <<
          "must be >= dimension of problem\n";
        return;
      }


    // create the ID to identify the constrained dof
    ID id(dimR);

    // construct the tranformation matrix Ccr, where  Uc = Ccr Ur & set the diag
    Matrix mat(dimR,dimR);
    mat.Zero();

    // set the values
    for(int i=0; i<dimR; i++)
      {
        mat(i,i) = 1.0;
        id(i) = i;
      }

    set_constraint(mat);
    set_constrained_retained_dofs(id,id);
  }

XC::RigidRod::RigidRod(int mPtag)
  : RigidBase(mPtag) {}


XC::RigidRod::RigidRod(int mPtag,const int &nm, const int &ns)
  : RigidBase(mPtag,nm,ns,0) {}

