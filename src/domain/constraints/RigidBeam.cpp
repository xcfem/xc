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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/01/08 01:22:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/RigidBeam.cpp,v $
                                                                        
                                                                        
// File: ~/model/constraints/RigidBeam.C
//
// Written: fmk 12/99
// Revised:
//
// Purpose: This file contains the class implementation for XC::RigidBeam.


#include "RigidBeam.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MFreedom_Constraint.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>

XC::Matrix XC::RigidBeam::setup_matrix(int numDOF,const Vector &crdR,const Vector &crdC,ID &id)
  {
    const int dimR = crdR.Size();
    const int dimC = crdC.Size();

    // construct the tranformation matrix Ccr, where  Uc = Ccr Ur & set the diag, Ccr = I
    Matrix retval(numDOF,numDOF);
    retval.Zero();

    if(dimR != dimC)
      {
        std::cerr << "RigidBeam::setup_matrix - mismatch in dimension "  <<
	  "between constrained Node " <<  getNodeConstrained() <<  " and Retained node" << getNodeRetained() << std::endl;
        return retval;
      }

    // check the number of dof at the nodes >= dimension of problem
    if(numDOF < dimR)
      {    
	std::cerr << "RigidBeam::RigidBeam - numDOF at nodes " << 
	  getNodeRetained() << " and " <<  getNodeConstrained() <<  "must be >= dimension of problem\n";
        return retval;
      }


    // set the values
    for(int i=0; i<numDOF; i++)
      {
        retval(i,i) = 1.0;
        id(i)= i;
      }

    // if there are rotational dof - we must modify Ccr DONE ASSUMING SMALL ROTATIONS
    if(dimR != numDOF)
      {
        if(dimR == 2 && numDOF == 3)
          {
            double deltaX= crdC(0) - crdR(0);
            double deltaY= crdC(1) - crdR(1);
            retval(0,2)= -deltaY;
            retval(1,2)= deltaX;
          }
        else if(dimR == 3 && numDOF == 6)
          {
            const double deltaX= crdC(0) - crdR(0);
            const double deltaY= crdC(1) - crdR(1);
            const double deltaZ= crdC(2) - crdR(2);
            // rotation about z/3 axis
            retval(0,5)= -deltaY;
            retval(1,5)= deltaX;

            // rotation about y/2 axis
            retval(0,4)= deltaZ;
            retval(2,4)= -deltaX;

            // rotation about x/1 axis
            retval(1,3) = -deltaZ;
            retval(2,3) = deltaY;
          }
        else
          { // not valid
            std::cerr << "RigidBeam::RigidBeam -  for nodes " <<
	      getNodeRetained() << "and " << getNodeConstrained() <<  "nodes do not have valid numDOF for their dimension\n";
            return retval;
          }
      }
    // check the number of dof at the nodes >= dimension of problem
    if(numDOF < dimR)
      {
        std::cerr << "XC::RigidBeam::RigidBeam - numDOF at nodes " <<
	  getNodeRetained() << " and " <<  getNodeConstrained() <<  "must be >= dimension of problem\n";
        return retval;
      }
    return retval;
  }

void XC::RigidBeam::setup(Domain *theDomain)
  {
    setDomain(theDomain);

    if(nodeR && nodeC)
      {
        // check the number of dof at each node is the same
        const int numDOF = nodeR->getNumberDOF();
        if(numDOF != nodeC->getNumberDOF())
          {
            std::cerr << "RigidBeam::RigidBeam - mismatch in numDOF "
                      << " between constrained XC::Node " 
                      <<  getNodeConstrained() <<  " and Retained node"
                      << getNodeRetained() << std::endl;
          }
        else
          {
            // create the XC::ID to identify the constrained dof
            ID id(numDOF);

            // get the coordinates of the two nodes - check dimensions are the same FOR THE MOMENT
            const Vector &crdR = nodeR->getCrds();
            const Vector &crdC = nodeC->getCrds();
 
            Matrix mat= setup_matrix(numDOF,crdR,crdC,id);
            set_constraint(mat);
            set_constrained_retained_dofs(id,id);
	  }
      }
  }

XC::RigidBeam::RigidBeam(int mPtag)
  : RigidBase(mPtag) {}

XC::RigidBeam::RigidBeam(int mPtag,const int &nm, const int &ns)
  : RigidBase(mPtag,nm,ns,0) {}


