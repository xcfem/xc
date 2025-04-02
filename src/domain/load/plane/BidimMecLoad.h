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
//BidimMecLoad.h
                                                                        
#ifndef BidimMecLoad_h
#define BidimMecLoad_h


#include "BidimLoad.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/geom/pos_vec/Vector3d.h"

class SlidingVectorsSystem3d;

namespace XC {

//! @ingroup ElemLoads
//
//! @brief Mechanical load (forces) over bidimensional elements.
template <size_t force_vector_dim>
class BidimMecLoad: public BidimLoad
  {
  public:
    BidimMecLoad(int tag, int classTag, const ID &theElementTags)
      : BidimLoad(tag, classTag, theElementTags) {}
    BidimMecLoad(int tag, int classTag)
      : BidimLoad(tag, classTag) {}
    
    static size_t getForceVectorDimension(void)
      { return force_vector_dim; }

    virtual Vector getLocalForce(void) const;    
    Vector3d getVector3dLocalForce(void) const;
    virtual const Matrix &getLocalForces(void) const;
    virtual const Matrix &getGlobalVectors(const Matrix &) const= 0;
    virtual const Matrix &getGlobalForces(void) const;

    virtual SlidingVectorsSystem3d getResultant(const Pos3d &p= Pos3d(), bool initialGeometry= true) const= 0;
  };

//! @brief Returns force expressed in local coordinates.
template <size_t force_vector_dim>
Vector BidimMecLoad<force_vector_dim>::getLocalForce(void) const
  {
    Vector retval(force_vector_dim);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }
  
//! @brief Return the local components of the
//! force in a Vector3d. 
template <size_t force_vector_dim>
Vector3d BidimMecLoad<force_vector_dim>::getVector3dLocalForce(void) const
  {
    Vector f= getLocalForce();
    Vector3d retval;
    if(f.Size()==2)
      retval= Vector3d(f[0],f[1],f[2]);
    else
      retval= Vector3d(f[0],f[1],0.0);
    return retval;
  }
  
//! @brief Returns the components of the force vectors.
template <size_t force_vector_dim>
const Matrix &BidimMecLoad<force_vector_dim>::getLocalForces(void) const
  {
    static Matrix retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Return the forces on each element expressed in global coordinates.
template <size_t force_vector_dim>
const Matrix &BidimMecLoad<force_vector_dim>::getGlobalForces(void) const
  { return this->getGlobalVectors(this->getLocalForces()); }

} // end of XC namespace

#endif

