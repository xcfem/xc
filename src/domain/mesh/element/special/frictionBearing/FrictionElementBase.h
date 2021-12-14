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
//FrictionElementBase.h,v $

#ifndef FrictionElementBase_h
#define FrictionElementBase_h

#include "domain/mesh/element/Element0D.h"
#include "domain/mesh/element/utils/physical_properties/UniaxialMatPhysicalProperties.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class FrictionModel;
class UniaxialMaterial;
class Response;

//! @ingroup Elem
//!
//! @defgroup FrictionElementGrp Element used to represent <a href="http://en.wikipedia.org/wiki/Friction">frictional</a> contacts.
//
//! @ingroup FrictionElementGrp
//
//! @brief Base class for friction elements.
class FrictionElementBase: public Element0D
  {
  protected:    
    FrictionModel *theFrnMdl; //!< pointer to friction model
    UniaxialMatPhysicalProperties physicalProperties;  //!<array of uniaxial materials
    
    // parameters
    double uy;          // yield displacement
    Vector x;           // local x direction
    Vector y;           // local y direction
    double mass;        // mass of element
    int maxIter;        // maximum number of iterations
    double tol;         // tolerance for convergence criterion
    double L;           // element length
    
    // state variables
    Vector ub;          // displacements in basic system
    Vector qb;          // forces in basic system
    Matrix kb;          // stiffness matrix in basic system
    Vector ul;          // displacements in local system
    Matrix Tgl;         // transformation matrix from global to local system
    Matrix Tlb;         // transformation matrix from local to basic system
    
    // committed history variables
    
    // initial stiffness matrix in basic system
    Matrix kbInit;
    
    void free_friction_model(void);
    void alloc_friction_model(const FrictionModel &);
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructors
    FrictionElementBase(int tag, int classTag, int Nd1, int Nd2,const size_t &dim,const FrictionModel &theFrnMdl, const UniaxialMatPhysicalProperties &, const double &uy= 0.0, const Vector &y= Vector(), const Vector &x= Vector(),const double &mass = 0.0,const int &maxIter= 20,const double &tol= 1E-8);
    FrictionElementBase(int classTag,const size_t &);
    FrictionElementBase(const FrictionElementBase &);
    FrictionElementBase &operator=(const FrictionElementBase &);
    
    // destructor
    ~FrictionElementBase(void);
    
    // public methods to obtain information about dof & connectivity    
    int getNumDOF();
    
  };
} // end of XC namespace

#endif
