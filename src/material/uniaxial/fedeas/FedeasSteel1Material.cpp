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
// $Date: 2004/07/15 21:36:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasSteel1Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasSteel1Material. FedeasSteel1Material wraps the FEDEAS
// 1d material subroutine Steel_1.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasSteel1Material.h>
#include "utility/matrix/Vector.h"

XC::FedeasSteel1Material::FedeasSteel1Material(int tag,
					 double fy, double E0, double b,
					 double a1, double a2, double a3, double a4):
// 7 history variables and 7 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel1, 7, 7)
{
	data[0]  = fy;
	data[1]  = E0;
	data[2]  = b;
	data[3]  = a1;
	data[4]  = a2;
	data[5]  = a3;
	data[6]  = a4;

	trial.Tangent()= E0;
	converged.Tangent()= E0;
}

XC::FedeasSteel1Material::FedeasSteel1Material(int tag,
					 double fy, double E0, double b):
// 7 history variables and 7 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel1, 7, 7)
{
	data[0]  = fy;
	data[1]  = E0;
	data[2]  = b;

	// Default values for no isotropic hardening
	data[3]  = 0.0;
	data[4]  = 1.0;
	data[5]  = 0.0;
	data[6]  = 1.0;

	trial.Tangent()= E0;
	converged.Tangent()= E0;
  }

XC::FedeasSteel1Material::FedeasSteel1Material(int tag, const XC::Vector &d):
// 7 history variables and 7 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel1, 7, 7)
{
  if (d.Size() != numData) {
    std::cerr << "XC::FedeasSteel1Material::FedeasSteel1Material -- not enough input arguments\n";
    exit(-1);
  }

  for (int i = 0; i < numData; i++)
    data[i] = d(i);
  trial.Tangent()= data[1];
  converged.Tangent()= data[1];
}

XC::FedeasSteel1Material::FedeasSteel1Material(int tag)
  : XC::FedeasMaterial(tag, MAT_TAG_FedeasSteel1, 7, 7)
  {}

XC::UniaxialMaterial *XC::FedeasSteel1Material::getCopy(void) const
  { return new FedeasSteel1Material(*this); }

double XC::FedeasSteel1Material::getInitialTangent(void) const
  {
    //return E;
    return data[1];
  }
