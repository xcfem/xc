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
// $Date: 2004/07/15 21:36:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasBond1Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasBond1Material. FedeasBond1Material wraps the FEDEAS
// 1d material subroutine Bond_1.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasBond1Material.h>
#include "utility/matrix/Vector.h"


XC::FedeasBond1Material::FedeasBond1Material(int tag,
	double u1p, double q1p, double u2p, double u3p, double q3p,
	double u1n, double q1n, double u2n, double u3n, double q3n,
	double s0, double bb):
// 6 history variables and 12 material parameters
  XC::FedeasBondMaterial(tag, MAT_TAG_FedeasBond1, 6, 12,u1p,q1p,u2p,u3p,q3p,u1n,q1n,u2n,u3n,q3n,s0,bb) {}

XC::FedeasBond1Material::FedeasBond1Material(int tag, const Vector &d):
// 6 history variables and 12 material parameters
  XC::FedeasBondMaterial(tag, MAT_TAG_FedeasBond1, 6, 12,d)
  {}

//! @brief Constructor.
XC::FedeasBond1Material::FedeasBond1Material(int tag)
  : XC::FedeasBondMaterial(tag, MAT_TAG_FedeasBond1, 6, 12) {}


//! @brief Constructor virtual.
XC::UniaxialMaterial *XC::FedeasBond1Material::getCopy(void) const
  { return new FedeasBond1Material(*this); }

