// -*-c++-*-
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
                                                                        
#ifndef ElasticPPMaterial_h
#define ElasticPPMaterial_h

// File: ~/material/ElasticPPMaterial.h
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// ElasticPPMaterial. ElasticPPMaterial provides the abstraction
// of an elastic perfectly plastic uniaxial material, 
//
// What: "@(#) ElasticPPMaterial.h, revA"

#include "material/uniaxial/ElasticPPMaterialBase.h"

namespace XC {
//
//! @brief Elastic perfectly plastic material.
//!
//! This class provides the abstraction of an elastic
//! perfectly plastic uniaxial material, i.e. the stress-strain
//! relationship is given by the linear equation \f$\sigma = E * \epsilon\f$
//! while the material is elastic and \f$| \sigma = E * \epsilon_p |\f$  while
//! the material is undergoing plastic deformation.
//! @ingroup MatUnx
class ElasticPPMaterial: public ElasticPPMaterialBase
  {
  public:
    ElasticPPMaterial(int tag= 0);    
    ElasticPPMaterial(int tag, double E, double eyp);    
    ElasticPPMaterial(int tag, double E, double eyp, double eyn, double ezero);
    UniaxialMaterial *getCopy(void) const;    

    void set_fyp(const double &);
    void set_fyn(const double &);
  };
} // end of XC namespace


#endif



