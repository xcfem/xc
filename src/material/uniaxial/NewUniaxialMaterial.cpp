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
// $Date: 2005/08/22 20:50:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/NewUniaxialMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class implementation for 
// NewUniaxialMaterial.

#include <material/uniaxial/NewUniaxialMaterial.h>
#include <utility/matrix/Vector.h>

#include <cmath>
#include <cfloat>

//! @brief Constructor.
XC::NewUniaxialMaterial::NewUniaxialMaterial(int tag)
  :UniaxialMaterial(tag,MAT_TAG_NewUniaxialMaterial),
   Tstrain(0.0), Tstress(0.0), Ttangent(0.0)
  {}

//! @brief Constructor por defecto.
XC::NewUniaxialMaterial::NewUniaxialMaterial()
  :UniaxialMaterial(0,MAT_TAG_NewUniaxialMaterial),
   Tstrain(0.0), Tstress(0.0), Ttangent(0.0)
  {}

//! @brief Asigna la deformación al material.
int XC::NewUniaxialMaterial::setTrialStrain(double strain, double strainRate)
  {
    // set the trial strain
    Tstrain = strain;

    // determine trial stress and tangent
    Tstress = 0.0;
    Ttangent = 0.0;
    return 0;
  }

//! @brief Devuelve la tensión del material.
double XC::NewUniaxialMaterial::getStress(void) const
  { return Tstress; }

//! @brief Devuelve el módulo de rigidez tangente del material.
double XC::NewUniaxialMaterial::getTangent(void) const
  { return Ttangent; }

//! @brief Devuelve el módulo de rigidez tangente inicial del material.
double XC::NewUniaxialMaterial::getInitialTangent(void) const
  {
    // return the initial tangent
    return 0.0;
  }

//! @brief Devuelve la deformación del material.
double XC::NewUniaxialMaterial::getStrain(void) const
  { return Tstrain; }

//! @brief Consuma los valores de las variables de estado del material.
int XC::NewUniaxialMaterial::commitState(void)
  { return 0; }

//! @brief Devuelve el estado del material al último consolidado.
int XC::NewUniaxialMaterial::revertToLastCommit(void)
  { return 0; }

//! @brief Devuelve los valores de las variables de estado del material al
//! los del estado inicial.
int XC::NewUniaxialMaterial::revertToStart(void)
  { return 0; }

//! @brief Constructor virtual.
XC::UniaxialMaterial *XC::NewUniaxialMaterial::getCopy(void) const
  { return new XC::NewUniaxialMaterial(this->getTag()); }

int XC::NewUniaxialMaterial::sendSelf(CommParameters &cp)
  { return -1; }

int XC::NewUniaxialMaterial::recvSelf(const CommParameters &cp)
  { return -1; }

void XC::NewUniaxialMaterial::Print(std::ostream &s, int flag)
  { return; }


