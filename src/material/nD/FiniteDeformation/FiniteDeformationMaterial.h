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

#ifndef FiniteDeformationMaterial_h
#define FiniteDeformationMaterial_h

#include <material/nD/NDMaterial.h>

namespace XC {
//
//! @brief Base class for finite deformation materials.
//! @ingroup FDNDMat
//!
//! In continuum mechanics, the finite strain theory—also called large
//! strain theory, or large deformation theory—deals with deformations
//! in which strains and/or rotations are large enough to invalidate
//! assumptions inherent in infinitesimal strain theory. In this case,
//! the undeformed and deformed configurations of the continuum are
//! significantly different, requiring a clear distinction between them.
class FiniteDeformationMaterial: public NDMaterial
  {
  public:    
    FiniteDeformationMaterial(int tag, int classTag);
    
//Zhao (zcheng@ucdavis.edu)
// added Sept 22 2003 for Large Deformation, F is the Deformation Gradient
    virtual int setTrialF(const straintensor &f);
    virtual int setTrialFIncr(const straintensor &df);
    virtual int setTrialC(const straintensor &c);
    virtual int setTrialCIncr(const straintensor &dc);
    virtual const stresstensor getPK1StressTensor(void) const;
    virtual const stresstensor getCauchyStressTensor(void) const;
    virtual const straintensor &getF(void) const;
    virtual const straintensor &getC(void) const;
    virtual const straintensor getFp(void) const;
// Only For Large Deformation, END////////////////////////////////////////

  };
} // end of XC namespace

#endif

