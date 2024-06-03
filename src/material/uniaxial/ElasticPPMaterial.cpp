//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ElasticPPMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/ElasticPPMaterial.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class implementation for 
// ElasticMaterial. 
//
// What: "@(#) ElasticPPMaterial.C, revA"


#include <material/uniaxial/ElasticPPMaterial.h>
#include <utility/matrix/Vector.h>
#include <cmath>
#include <cfloat>
#include "utility/utils/misc_utils/colormod.h"


//! @brief Sets the positive yield stress value (tension).
void XC::ElasticPPMaterial::set_fyp(const double &f)
  {
    fyp= f;
    if(fyp < 0)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; fyp < 0, setting > 0"
	          << Color::def << std::endl;
        fyp*= -1.;
      }
  }

//! @brief Set the yield stress a compression value.
void XC::ElasticPPMaterial::set_fyn(const double &f)
  {
    fyn= f;
    if(fyn > 0)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; fyn > 0, setting < 0"
	          << Color::def << std::endl;
        fyn*= -1.;
      }  
  }

//! @brief Constructor.
//! @param[in] tag material identifier.
//! @param[in] e material elastic modulus.
//! @param[in] eyp positive yield strain value (tension).
XC::ElasticPPMaterial::ElasticPPMaterial(int tag, double e, double eyp)
  // MAT_TAG_ElasticPPMaterial
  :ElasticPPMaterialBase(tag, MAT_TAG_ElasticPPMaterial, e, eyp) {}

//! @brief Constructor.
//! @param[in] tag material identifier.
//! @param[in] e material elastic modulus.
//! @param[in] eyp positive yield strain value (tension).
//! @param[in] eyn negative yield strain value (compression).
XC::ElasticPPMaterial::ElasticPPMaterial(int tag, double e, double eyp,double eyn, double ez )
  :ElasticPPMaterialBase(tag, MAT_TAG_ElasticPPMaterial,e,eyp, eyn, ez) {}

//! @brief Constructor.
XC::ElasticPPMaterial::ElasticPPMaterial(int tag)
  :ElasticPPMaterialBase(tag, MAT_TAG_ElasticPPMaterial){}

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::ElasticPPMaterial::getCopy(void) const
  { return new ElasticPPMaterial(*this); }

