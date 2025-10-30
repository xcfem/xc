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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/10/07 21:18:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/FourNodeQuad.h,v $
                                                                        
// Written: MHS
// Created: Feb 2000
// Revised: Dec 2000 for efficiency
//
// Description: This file contains the class definition for FourNodeQuad.

#ifndef FourNodeQuad_h
#define FourNodeQuad_h

#include "domain/mesh/element/plane/SolidMech4N.h"
#include "domain/mesh/element/utils/physical_properties/SolidMech2D.h"
#include "domain/mesh/element/utils/body_forces/BodyForces2D.h"
#include "domain/mesh/element/utils/fvectors/FVectorQuad.h"

namespace XC {
class NDMaterial;
class Material;
class Response;
class GaussPoint;
class ParticlePos2d;

//! @ingroup PlaneElements
//
//! @brief Four node quadrilateral element for two-dimensional problems.
class FourNodeQuad: public SolidMech4N
  {
  private:    
    BodyForces2D bf; //!< Body forces.
    Vector pressureLoad; //!< Pressure load at nodes

    double pressure; //!< Normal surface traction (pressure) over entire element (note: positive for outward normal).
    FVectorQuad p0; //!< Reactions in the basic system due to element loads
    mutable std::vector<Vector> eps; //!< strains at gauss points.
    std::vector<Vector> persistentInitialDeformation; //!< Persistent initial strain at element level. Used to store the deformation during the inactive phase of the element (if any).

    static double matrixData[64]; //!< array data for matrix
    static Matrix K; //!< Element stiffness, and damping matrix.
    static Matrix mass; //!< mass matrix.
    static Vector P; //!< Element resisting force vector
    static double shp[3][4]; //!< Stores shape functions and derivatives (overwritten)

    // private member functions - only objects of this class can call these
    double shapeFunction(const GaussPoint &gp) const;
    void setPressureLoadAtNodes(void);
    
    //inertia terms
    void formInertiaTerms(int tangFlag) const;

  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    FourNodeQuad(int tag= 0,const NDMaterial *ptr_mat= nullptr);
    FourNodeQuad(int tag, int nd1, int nd2, int nd3, int nd4, NDMaterial &m, const std::string &type, double t, double pressure = 0.0, const BodyForces2D &bForces= BodyForces2D());
    Element *getCopy(void) const;
    virtual ~FourNodeQuad(void);

    // Element birth and death stuff.
    const std::vector<Vector> &getPersistentInitialDeformation(void) const;
    void incrementPersistentInitialDeformationWithCurrentDeformation(void);
    
    int getNumDOF(void) const;
    int resetNodalCoordinates(void);
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int update(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;    
    const Matrix &getMass(void) const;    

    const GaussModel &getGaussModel(void) const;

    void alive(void);
    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            
    virtual ElementalLoad *createInertiaLoad(const Vector &);

    Pos2d getCartesianCoordinates(const ParticlePos2d &, bool initialGeometry= true) const;
    
    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);

    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;

    double detJ(const double &,const double &) const;

  };
} // end of XC namespace

#endif

