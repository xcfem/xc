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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/02/25 23:33:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLength.h,v $
                                                                        
                                                                        
#ifndef ZeroLength_h
#define ZeroLength_h

// File: ~/element/zeroLength/ZeroLength.h
// 
// Written: GLF
// Created: 12/99
// Revision: A
//
// Description: This file contains the class definition for ZeroLength.
// A ZeroLength element is defined by two nodes with the same coordinate.
// One or more material objects may be associated with the nodes to
// provide a force displacement relationship.
// ZeroLength element will work with 1d, 2d, or 3d material models.
//
// What: "@(#) ZeroLength.h, revA"

#include <domain/mesh/element/Element0D.h>
#include "material/uniaxial/ZeroLengthMaterials.h"
#include <utility/matrix/Matrix.h>
#include <material/uniaxial/DqUniaxialMaterial.h>


// Type of dimension of element NxDy has dimension x=1,2,3 and
// y=2,4,6,12 degrees-of-freedom for the element
enum Etype { D1N2, D2N4, D2N6, D3N6, D3N12 };


namespace XC {
class Node;
class Channel;
class UniaxialMaterial;
class Response;

//! @ingroup ElemZL
//
//! @brief Zero length element.
//!
//! The ZeroLength class represents an element defined by two nodes at the
//! same geometric location, hence it has zero length. The nodes are connected
//! by of uniaxial materials to represent the force-deformation relationship
//! for the element. ZeroLength elements are constructed with a \p tag in a
//! domain of \p dimension 1, 2, or 3, connected by nodes {\em Nd1} and
//! {\em Nd2}. The vector \p x defines the local x-axis for the element and
//! the vector \p yprime lies in the local x-y plane for the element.  The
//! local z-axis is the cross product between \p x and \p yprime, and the
//! local y-axis is the cross product between the local z-axis and \p x.
class ZeroLength: public Element0D
  {
  private:
    Etype elemType;
    Matrix *theMatrix; //!< pointer to objects matrix
    Vector *theVector; //!< pointer to objects vector
    // Storage for uniaxial material models
    ZeroLengthMaterials theMaterial1d; //!< array of pointers to 1d materials and related directionss.

    Matrix t1d; //!< hold the transformation matrix.

    // private methods
    void checkDirection(ID &dir) const;
    
    void setUp(int Nd1, int Nd2,const Vector &x,const Vector &y);
    void setTran1d ( Etype e, int n );
    double computeCurrentStrain1d(int mat, const Vector& diff ) const;    



    // static data - single copy for all objects of the class    
    static Matrix ZeroLengthM2;   // class wide matrix for 2*2
    static Matrix ZeroLengthM4;   // class wide matrix for 4*4
    static Matrix ZeroLengthM6;   // class wide matrix for 6*6
    static Matrix ZeroLengthM12;  // class wide matrix for 12*12
    static Vector ZeroLengthV2;   // class wide Vector for size 2
    static Vector ZeroLengthV4;   // class wide Vector for size 4
    static Vector ZeroLengthV6;   // class wide Vector for size 6
    static Vector ZeroLengthV12;  // class wide Vector for size 12

  protected:
    void setUpType(const size_t &);

    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    // Constructor for a single 1d material model
    ZeroLength(int tag,int dimension,int Nd1, int Nd2,const Vector &,const Vector &,UniaxialMaterial &,int direction );
    // Constructor for a multiple 1d material models
    ZeroLength(int tag,int dimension,int Nd1, int Nd2, const Vector &,const Vector &,const DqUniaxialMaterial &,const ID &direction);
    ZeroLength(int tag,int dimension,const Material *ptr_mat,int direction);
    ZeroLength(void);
    Element *getCopy(void) const;
    ~ZeroLength(void);

    void setDomain(Domain *theDomain);

    inline void clearMaterials(void)
      { theMaterial1d.clear(); }
    void setMaterial(const int &,const std::string &);
    void setMaterials(const std::deque<int> &,const std::vector<std::string> &);
    ZeroLengthMaterials &getMaterials(void)
      { return theMaterial1d; }
    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    std::string getElementType(void) const;
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);    

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;    

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    
    void updateDir(const Vector& x, const Vector& y);

  };
} // end of XC namespace

#endif




