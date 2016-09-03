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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/04/02 22:02:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PathIndependentMaterial.cpp,v $

// Written: MHS
// Created: Aug 2000
//
// Description: This file contains the class definition for 
// PathIndependentMaterial.  PathIndependentMaterial uses a XC::UniaxialMaterial
// object to represent a path-independent uniaxial material.  Since
// it is path-independent, no state information is stored by
// PathIndependentMaterial.

#include <cstdlib>
#include <material/uniaxial/PathIndependentMaterial.h>
#include <utility/matrix/ID.h>

XC::PathIndependentMaterial::PathIndependentMaterial(int tag, UniaxialMaterial &material)
:EncapsulatedMaterial(tag,MAT_TAG_PathIndependent)
  {
    theMaterial = material.getCopy();
    if(theMaterial == 0)
      {
        std::cerr <<  "XC::PathIndependentMaterial::PathIndependentMaterial -- failed to get copy of material\n";
        exit(-1);
      }
  }

XC::PathIndependentMaterial::PathIndependentMaterial(int tag)
  : EncapsulatedMaterial(tag,MAT_TAG_PathIndependent) {}

XC::PathIndependentMaterial::PathIndependentMaterial(void)
  :EncapsulatedMaterial(0,MAT_TAG_PathIndependent) {}

int XC::PathIndependentMaterial::setTrialStrain(double strain, double strainRate)
  {
    return theMaterial->setTrialStrain(strain, strainRate);
  }

double XC::PathIndependentMaterial::getStress(void) const
  { return theMaterial->getStress(); }


double XC::PathIndependentMaterial::getTangent(void) const
  { return theMaterial->getTangent(); }

double XC::PathIndependentMaterial::getDampTangent(void) const
  { return theMaterial->getDampTangent(); }

double XC::PathIndependentMaterial::getInitialTangent(void) const
  { return theMaterial->getInitialTangent(); }

int XC::PathIndependentMaterial::commitState(void)
  { return 0; }

int XC::PathIndependentMaterial::revertToLastCommit(void)
  { return 0; }

int XC::PathIndependentMaterial::revertToStart(void)
  { return theMaterial->revertToStart(); }

XC::UniaxialMaterial *XC::PathIndependentMaterial::getCopy(void) const
  { return new PathIndependentMaterial(*this); }


int XC::PathIndependentMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4); 
    int res= EncapsulatedMaterial::sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "PathIndependentMaterial::sendSelf() - failed to send the ID.\n";
    return res;

  }

int XC::PathIndependentMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "PathIndependentMaterial::recvSelf() - failed to get the ID\n";
    else
      res+= EncapsulatedMaterial::recvData(cp);
    return res;
  }

void XC::PathIndependentMaterial::Print(std::ostream &s, int flag)
  {
    s << "PathIndependentMaterial tag: " << this->getTag() << std::endl;
    s << "\tmaterial: " << theMaterial->getTag() << std::endl;
  }
