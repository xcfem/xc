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

#ifndef ElasticIsotropic2D_h
#define ElasticIsotropic2D_h

#include <material/nD/ElasticIsotropicMaterial.h>

namespace XC {
//! @ingroup NDMat
//
//! @brief Isotropic elastic material for plane problems.
class ElasticIsotropic2D : public ElasticIsotropicMaterial
  {
  protected:
    static Matrix D;	        // Elastic constants
  public:
    ElasticIsotropic2D(int tag, int classTag, double E, double nu, double rho);
    ElasticIsotropic2D(int tag, int classTag);

    int getOrder (void) const;
  };
} // end of XC namespace


#endif


