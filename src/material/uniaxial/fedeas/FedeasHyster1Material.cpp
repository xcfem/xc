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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasHyster1Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasHyster1Material. FedeasHyster1Material wraps the FEDEAS
// 1d material subroutine Hyster_1.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasHyster1Material.h>
#include "utility/matrix/Vector.h"

XC::FedeasHyster1Material::FedeasHyster1Material(int tag,
	double mom1p, double rot1p, double mom2p, double rot2p,
	double mom1n, double rot1n, double mom2n, double rot2n,
	double pinchX, double pinchY, double damfc1, double damfc2):
// 6 history variables and 12 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHysteretic1, 6, 12)
{
	data[0]  = mom1p;
	data[1]  = rot1p;
	data[2]  = mom2p;
	data[3]  = rot2p;

	data[4]  = mom1n;
	data[5]  = rot1n;
	data[6]  = mom2n;
	data[7]  = rot2n;

	data[8]  = pinchX;
	data[9]  = pinchY;
	data[10] = damfc1;
	data[11] = damfc2;

    trial.Tangent() =  data[0]/data[1];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasHyster1Material::FedeasHyster1Material(int tag, const XC::Vector &d):
// 6 history variables and 12 material parameters
 XC::FedeasMaterial(tag, MAT_TAG_FedeasHysteretic1, 6, 12)
{
  if (d.Size() != numData) {
    std::cerr << "XC::FedeasHyster1Material::FedeasHyster1Material -- not enough input arguments\n";
    exit(-1);
  }
		
    for(int i = 0; i < numData; i++)
      data[i] = d(i);

    trial.Tangent() =  data[0]/data[1];
    converged.Tangent()= trial.getTangent();
  }

XC::FedeasHyster1Material::FedeasHyster1Material(int tag)
  : XC::FedeasMaterial(tag, MAT_TAG_FedeasHysteretic1, 6, 12)
  {}

XC::UniaxialMaterial* XC::FedeasHyster1Material::getCopy(void) const
  { return new XC::FedeasHyster1Material(*this); }
  

double XC::FedeasHyster1Material::getInitialTangent(void) const
  {
    //return mom1p/rot1p;
    return data[0]/data[1];
  }
