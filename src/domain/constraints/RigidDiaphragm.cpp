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

// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/RigidDiaphragm.cpp,v $


// File: ~/model/constraints/RigidDiaphragm.C
//
// Written: fmk 1/99
// Revised:
//
// Purpose: This file contains the class implementation for XC::RigidDiaphragm.


#include "RigidDiaphragm.h"
#include <cstdlib>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/constraints/MP_Constraint.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>


XC::RigidDiaphragm::RigidDiaphragm(Domain &theDomain, int nR, ID &nC, int perpPlaneConstrained, int startMPtag)
  : MP_Constraint(startMPtag)
  {
    setDomain(&theDomain);

    // check plane is valid, i.e. perpPlaneConstrained must be 0, 1 or 2
    if(perpPlaneConstrained < 0 || perpPlaneConstrained > 2)
      {
        std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - " <<
          "the dirn of perpendicular to constrained plane" << perpPlaneConstrained <<  "not valid\n";
        return;
      }

    // check constrainedNodes XC::ID does not contain the retained node
    if(nC.getLocation(nR) >= 0)
      {
        std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - " <<
          "retained node" << nR << "is in constrained node list\n";
        return;
      }

    // get a pointer to the retained node and check node in 3d with 6 dof
    Node *nodeR = theDomain.getNode(nR);
    if(!nodeR)
      {
        std::cerr << "RigidDiaphragm::RigidDiaphragm - " <<
          "retained Node" <<  nR <<  "not in domain\n";
        return;
      }
    else
      nodeR->connect(this);

    const Vector &crdR = nodeR->getCrds();
    if((nodeR->getNumberDOF() != 6) || (crdR.Size() != 3))
      {
        std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - " <<
          "retained XC::Node" << nR << "not in 3d space with 6 dof\n";
        return;
      }

    //
    // create some objects which will be passed to the MP_Constraint
    // constructor, elements of objects are filled in later
    //

    // create the ID to identify the constrained dof
    ID id(3);

    // construct the tranformation matrix Ccr, where  Uc = Ccr Ur & set the diag
    Matrix mat(3,3);
    mat.Zero();
    mat(0,0) = 1.0; mat(1,1) = 1.0; mat(2,2) = 1.0;


    // now for each of the specified constrained dof we:
    // 1. check it's in the plane of the retained node,
    // 2. set the ID and transformation matrix,
    // 3. create the MP_Constrainet and add it to the domain

    for(int i=0; i<nC.Size(); i++)
      {
      // get the constrained node
      int ndC = nC(i);
      Node *nodeC = theDomain.getNode(ndC);

      // ensure node exists
      if(nodeC)
        {
          nodeC->connect(this);
          // get node coordinates
          const Vector &crdC = nodeC->getCrds();

          // check constrained node has correct dim and number of dof
          if((nodeR->getNumberDOF() == 6) && (crdR.Size() == 3))
            {

              // determine delta Coordintaes
              const double deltaX = crdC(0) - crdR(0);
              const double deltaY = crdC(1) - crdR(1);
              const double deltaZ = crdC(2) - crdR(2);

              // rigid diaphragm in xy plane
              if(perpPlaneConstrained == 2)
                {
                  // check constrained node in xy plane with retained node
                  if(deltaZ == 0.0)
                    {
                      // dof corresponding to dX, dY and theta Z (0,1,5)
                      id(0) = 0; id(1) = 1; id(2) = 5;
                      // set up transformation matrix
                      mat(0,2) = - deltaY;
                      mat(1,2) = deltaX;
                    }
                  else
                      std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - ignoring constrained XC::Node " << ndC << ", not in xy plane\n";
                      // rigid diaphragm in xz plane
                }
              else
                if(perpPlaneConstrained == 1)
                  {
                    // check constrained node in xy plane with retained node
                    if(deltaY == 0.0)
                      {
                        // dof corresponding to dX, dZ and theta Y (0,2,4)
                        id(0) = 0; id(1) = 2; id(2) = 4;
                        // set up transformation matrix
                        mat(0,2) = deltaZ;
                        mat(1,2) = -deltaX;
                      }
                    else
                      std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - ignoring constrained XC::Node " << ndC << ", not in xz plane\n";
                     // rigid diaphragm in yz plane
                  }
                else
                  {
                    // check constrained node in xy plane with retained node
                    if(deltaX == 0.0)
                      {
                        // dof corresponding to dY, dZ and theta X (1,2,3)
                        id(0) = 1; id(1) = 2; id(2) = 3;
                        // set up transformation matrix
                        mat(0,2) = -deltaZ;
                        mat(1,2) = deltaY;
                      }
                    else
		      std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - ignoring constrained XC::Node " 
                                << ndC << ", not in xz plane\n";
                  }
              // create the XC::MP_Constraint
              MP_Constraint *newC = new XC::MP_Constraint(startMPtag+i, nR, ndC,mat, id, id);
              if(newC == 0)
                {
                  std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - ignoring constrained XC::Node " << ndC <<
                   ", out of memory\n";
                }
              else
                {
                  // add the constraint to the domain
                 if(theDomain.addMP_Constraint(newC) == false)
                   {
                     std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - ignoring constrained XC::Node " << ndC <<
                     ", failed to add\n";
                     delete newC;
                   }
                }

        }
      else  // node not in 3d space
        std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - ignoring constrained XC::Node " << ndC <<
          ", not 3d node\n";
      }
    else // node does not exist
      std::cerr << "XC::RigidDiaphragm::RigidDiaphragm - ignoring constrained XC::Node " << ndC <<
        " as no node in domain\n";
    } // for each node in constrained nodes
  }

//! @brief Destructor.
XC::RigidDiaphragm::~RigidDiaphragm(void)
  {
    //XXX Put here calls to disconnec.
  }



