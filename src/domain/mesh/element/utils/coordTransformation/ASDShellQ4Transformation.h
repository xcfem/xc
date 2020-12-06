// -*-c++-*-
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

// $Revision: 1.10 $
// $Date: 2020/05/18 22:51:21 $

// Original implementation: Massimo Petracca (ASDEA)
//
// Implementation of a linear coordinate transformation 4-node shells
//

#ifndef ASDShellQ4Transformation_h
#define ASDShellQ4Transformation_h

#include "ASDShellQ4LocalCoordinateSystem.h"
#include "domain/mesh/node/Node.h"
#include "domain/domain/Domain.h"

namespace XC {

//! @brief This class represents a basic (linear) coordinate transformation
//! that can be used by any element whose geometry is a QUAD 4 in 3D space,
//! with 6 D.O.F.s per node.
//!
//!  Its main aim is to:
//! 1) Create the local coordinate system
//! 2) Transform the incoming global displacements in local coordinate system
//! 3) Transform the outgoing matrices and vectors in global coordinate system
//! @ingroup ElemCT
class ASDShellQ4Transformation
  {
  public:
    typedef ASDVector3<double> Vector3Type;
    typedef ASDQuaternion<double> QuaternionType;
    typedef Vector VectorType;
    typedef Matrix MatrixType;
    typedef std::array<Node*, 4> NodeContainerType;

  protected:
    Vector m_U0= Vector(24);
    NodeContainerType m_nodes= { {nullptr, nullptr, nullptr, nullptr} };
    
  public:
    ASDShellQ4Transformation(void)
      {}
    virtual ~ASDShellQ4Transformation(void)
      {}
    virtual ASDShellQ4Transformation *getCopy(void) const;

    virtual ASDShellQ4Transformation* create() const
      { return getCopy(); }

    virtual bool isLinear(void) const
      { return true; }

    virtual void revertToStart(void)
      { }

    virtual void setDomain(Domain *, const ID &);

    virtual void revertToLastCommit()
      {}

    virtual void commit()
      {}

    virtual void update(const VectorType& globalDisplacements)
      { }

    virtual ASDShellQ4LocalCoordinateSystem createReferenceCoordinateSystem()const
      {
        // the reference coordinate system in the underformed configuration
        // using the default alignment to the first column of the jacobian at center
        return ASDShellQ4LocalCoordinateSystem(
            Vector3Type(m_nodes[0]->getCrds()),
            Vector3Type(m_nodes[1]->getCrds()),
            Vector3Type(m_nodes[2]->getCrds()),
            Vector3Type(m_nodes[3]->getCrds())
        );
      }

    virtual ASDShellQ4LocalCoordinateSystem createLocalCoordinateSystem(const VectorType& globalDisplacements)const
      {
        // same as reference
        return createReferenceCoordinateSystem();
      }

    virtual void computeGlobalDisplacements(VectorType& globalDisplacements) const
      {
        for (int i = 0; i < 4; i++) {
            int index = i * 6;
            const VectorType& iU = m_nodes[i]->getTrialDisp();
            for (int j = 0; j < 6; j++) {
                globalDisplacements(index + j) = iU(j) - m_U0(index + j);
            }
        }
      }

    virtual const MatrixType& computeTransformationMatrix(const ASDShellQ4LocalCoordinateSystem& LCS) const
      {
        static MatrixType R(24, 24);
        static MatrixType T(24, 24);
        static MatrixType W(24, 24);
        if (LCS.IsWarped()) {
            LCS.ComputeTotalRotationMatrix(R);
            LCS.ComputeTotalWarpageMatrix(W);
            T.addMatrixProduct(0.0, W, R, 1.0);
        }
        else {
            LCS.ComputeTotalRotationMatrix(T);
        }
        return T;
      }

    virtual void calculateLocalDisplacements(
        const ASDShellQ4LocalCoordinateSystem& LCS,
        const VectorType& globalDisplacements,
        VectorType& localDisplacements)
      {
        const MatrixType& R = computeTransformationMatrix(LCS);
        localDisplacements.addMatrixVector(0.0, R, globalDisplacements, 1.0);
      }

    virtual void transformToGlobal(
        const ASDShellQ4LocalCoordinateSystem& LCS,
        const VectorType& globalDisplacements,
        const VectorType& localDisplacements,
        MatrixType& LHS,
        VectorType& RHS,
        bool LHSrequired)
      {
        static MatrixType RT_LHS(24, 24);
        static VectorType RHScopy(24);
        const MatrixType& R = computeTransformationMatrix(LCS);
        RHScopy = RHS;
        RHS.addMatrixTransposeVector(0.0, R, RHScopy, 1.0);
        if (LHSrequired) {
            RT_LHS.addMatrixTransposeProduct(0.0, R, LHS, 1.0);
            LHS.addMatrixProduct(0.0, RT_LHS, R, 1.0);
        }
      }

    virtual void transformToGlobal(
        const ASDShellQ4LocalCoordinateSystem& LCS,
        MatrixType& LHS,
        VectorType& RHS,
        bool LHSrequired)
      {
        static VectorType dummy;
        transformToGlobal(LCS, dummy, dummy, LHS, RHS, LHSrequired);
      }

    virtual int internalDataSize() const
      {
        // just the size of the initial displacements
        return 24;
      }

    //! @brief Return the data needed to recreate the object
    virtual Vector getInternalData(void) const
      { 
	Vector retval(internalDataSize());
        for(int i = 0; i < 24; i++)
            retval(i) = m_U0(i);
	return retval;
      }

    //! @brief Restore the object from its internal data
    virtual void setInternalData(const VectorType &v)
      {
        for(int i = 0; i < 24; i++)
	  m_U0(i) = v(i);
      }

public:

    inline const NodeContainerType& getNodes()const { return m_nodes; }
    inline NodeContainerType& getNodes() { return m_nodes; }

  };
 
} // end of XC namespace

#endif // !ASDShellQ4Transformation_h

