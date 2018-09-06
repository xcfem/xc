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
//BeamUniformLoad.h
                                              
#ifndef BeamUniformLoad_h
#define BeamUniformLoad_h

#include "domain/load/beam_loads/BeamMecLoad.h"

namespace XC {

//! @ingroup ElemLoads
//
//! @brief Uniform load over beam elements.
class BeamUniformLoad : public BeamMecLoad
  {
  public:
    BeamUniformLoad(int tag, int classTag, double wTrans, double wAxial,const ID &theElementTags);
    BeamUniformLoad(int tag, int classTag);
    BeamUniformLoad(int classTag);

    std::string Categoria(void) const;
    //! @brief Returns distributed force vectors (one for each element) expressed in element local coordinates. Is simply a convenience function that makes the distributedness more explicit.
    inline const Matrix &getDistributedLocalForces(void) const
      { return getLocalForces(); }
    //! @brief Returns distributed moments (one for each element) expressed in element local coordinates. Is simply a convenience function that makes the distributedness more explicit.
    inline const Matrix &getDistributedLocalMoments(void) const
      { return getLocalMoments(); }
    //! @brief Returns distributed force vectors (one for each element) expressed in global coordinates. Is simply a convenience function that makes the distributedness more explicit.
    inline const Matrix &getDistributedGlobalForces(void) const
      { return getGlobalForces(); }
    //! @brief Returns distributed moments (one for each element) expressed in global coordinates. Is simply a convenience function that makes the distributedness more explicit.
    inline const Matrix &getDistributedGlobalMoments(void) const
      { return getGlobalMoments(); }
    virtual SlidingVectorsSystem3d getResultant(const Pos3d &p= Pos3d(), bool initialGeometry= true) const;
  };
} // end of XC namespace

#endif

