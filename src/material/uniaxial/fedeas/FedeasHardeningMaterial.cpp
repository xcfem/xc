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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasHardeningMaterial.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasHardeningMaterial. FedeasHardeningMaterial wraps the FEDEAS
// 1d material subroutine Hard_1.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasHardeningMaterial.h>
#include "utility/matrix/Vector.h"

XC::FedeasHardeningMaterial::FedeasHardeningMaterial(int tag,
					 double E, double sigmaY, double Hiso, double Hkin):
// 3 history variables and 4 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHardening, 3, 4)
  {
	// Fill in material parameters
	data[0] = E;
	data[1] = sigmaY;
	data[2] = Hiso;
	data[3] = Hkin;

    trial.Tangent()=  E;
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasHardeningMaterial::FedeasHardeningMaterial(int tag, const Vector &d):
// 3 history variables and 4 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHardening, 3, 4)
{
  if (d.Size() != numData) {
    std::cerr << "XC::FedeasHardeningMaterial::FedeasHardeningMaterial -- not enough input arguments\n";
    exit(-1);	
  }

  for (int i = 0; i < numData; i++)
    data[i] = d(i);

    trial.Tangent()= data[0];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasHardeningMaterial::FedeasHardeningMaterial(int tag)
  : XC::FedeasMaterial(tag, MAT_TAG_FedeasHardening, 3, 4) {}

XC::UniaxialMaterial* XC::FedeasHardeningMaterial::getCopy(void) const
  { return new FedeasHardeningMaterial(*this); }

double XC::FedeasHardeningMaterial::getInitialTangent(void) const
  { 
    //return E;
    return data[0];
  }
