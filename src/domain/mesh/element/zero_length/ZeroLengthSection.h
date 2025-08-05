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
// $Date: 2003/02/25 23:33:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLengthSection.h,v $
                                                                        
// Written: MHS
// Created: Sept 2000
//
// Description: This file contains the class definition for ZeroLengthSection.
// A ZeroLengthSection element is defined by two nodes with the same coordinate.
// A SectionForceDeformation object is associated with the nodes to
// provide the basic force-deformation relationship for the element.

#ifndef ZeroLengthSection_h
#define ZeroLengthSection_h

#include "domain/mesh/element/Element0D.h"

namespace XC {
class Matrix;
class Channel;
class SectionForceDeformation;
class Response;

//! @ingroup Elem
//!
//! @defgroup ElemZL Zero length elements.
//
//! @ingroup ElemZL
//! @brief Zero length element with SectionForceDeformation material.
//!
//! The ZeroLengthSection class represents an element defined by two nodes at
//! the same geometric location, hence it has zero length.
//! The nodes are connected by a SectionForceDeformation material which
//! represents the force-deformation relationship for the element. 
//! ZeroLengthSection elements are constructed in a domain of \p dimension 2 or
//! 3, connected by nodes {\em Nd1} and {\em Nd2}. 
//! The vector \p x defines the local x-axis for the element and the vector \p
//! yprime lies in the local x-y plane for the element.  The local z-axis is
//! the cross product between \p x and \p yprime, and the local y-axis is the
//! cross product between the local z-axis and \p x.
//! The section model acts in the local space defined by the \p x and
//! \p yprime vectors. The section axial force-deformation acts along the 
//! element local x-axis and the section y-z axes directly corresponsd to the
//! local element y-z axes.
class ZeroLengthSection: public Element0D
  {
  private:
    Matrix A; //!< Transformation matrix ... e = A*(u2-u1)
    mutable Vector v; //!< Section deformation vector, the element basic deformations
    
    Matrix *K; //!< Pointer to element stiffness matrix
    Vector *P; //!< Pointer to element force vector
    
    SectionForceDeformation *theSection;  //!< Pointer to section object
    int order;	//!< Order of the section model
    
    // Class wide matrices for return
    static Matrix K6;
    static Matrix K12;
    
    // Class wide vectors for return
    static Vector P6;
    static Vector P12;

    // private methods
    void setTransformation(void);
    void computeSectionDefs(void) const;

    void setup_section(const Material *sec);
    void inicAv(void);
    void free_mem(void);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    ZeroLengthSection(int tag, int dimension, int Nd1, int Nd2, const Vector& x, const Vector& yprime, SectionForceDeformation& theSection);
    ZeroLengthSection(int tag, int dimension,const Material *theSection);
    ZeroLengthSection(void);
    ZeroLengthSection(const ZeroLengthSection &);
    ZeroLengthSection &operator=(const ZeroLengthSection &);
    Element *getCopy(void) const;
    ~ZeroLengthSection(void);

    void setDomain(Domain *theDomain);

    inline int getOrder(void) const
      { return order; }
   
    // public methods to set the state of the element    
    int update(void); // added by MSN to allow errors in setting section trial deformation
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);

    //! @brief Return the matrix that transforms internal forces into element reactions.
    inline const Matrix &getInternalForcesTransformation(void) const
      { return A; }
    void setUpVectors(const Vector &, const Vector &);

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;

    void alive(void);
    
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    Matrix getExtrapolatedValues(const Matrix &) const;
    boost::python::list getValuesAtNodes(const std::string &, bool silent= false) const;
    
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    Vector getVDirStrongAxisLocalCoord(void) const;
    Vector getVDirWeakAxisLocalCoord(void) const;
    double getStrongAxisAngle(void) const;
    double getWeakAxisAngle(void) const;
    const Vector &getVDirStrongAxisGlobalCoord(bool initialGeometry) const;
    const Vector &getVDirWeakAxisGlobalCoord(bool initialGeometry) const;

    inline SectionForceDeformation *getSection(void)
      { return theSection; }

    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;    

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    
// AddingSensitivity:BEGIN //////////////////////////////////////////
    const Vector &getResistingForceSensitivity(int gradIndex);
    int commitSensitivity(int gradIndex, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif




