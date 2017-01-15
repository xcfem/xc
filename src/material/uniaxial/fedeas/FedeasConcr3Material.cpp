//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/fedeas/FedeasConcr3Material.cpp,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasConcr3Material. FedeasConcr3Material wraps the FEDEAS
// 1d material subroutine Concr_3.

#include <cstdlib>
#include <material/uniaxial/fedeas/FedeasConcr3Material.h>
#include "utility/matrix/Vector.h"

XC::FedeasConcr3Material::FedeasConcr3Material(int tag,
					 double fc, double ec, double fu, double eu,
					 double rat, double ft, double epst0,
					 double ft0, double beta, double epstu):
// 2 history variables and 10 material parameters
 XC::FedeasConcrMaterial(tag, MAT_TAG_FedeasConcrete3, 2, 10,fc,ec,fu,eu)
  {
    data[4]  = rat;
    data[5]  = ft;
	data[6]  = epst0;
	data[7]  = ft0;
	data[8]  = beta;
	data[9]  = epstu;
  }

XC::FedeasConcr3Material::FedeasConcr3Material(int tag, const Vector &d):
// 2 history variables and 10 material parameters
  XC::FedeasConcrMaterial(tag, MAT_TAG_FedeasConcrete3, 2, 10,d) {}

XC::FedeasConcr3Material::FedeasConcr3Material(int tag)
  : XC::FedeasConcrMaterial(tag, MAT_TAG_FedeasConcrete3, 2, 10) {}

XC::UniaxialMaterial* XC::FedeasConcr3Material::getCopy(void) const
  { return new FedeasConcr3Material(*this); }


