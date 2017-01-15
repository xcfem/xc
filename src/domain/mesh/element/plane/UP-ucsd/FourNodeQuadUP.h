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
///////////////////////////////////////////////////////////////////////////////
// Description: This file contains the class declaration for FourNodeQuadUP. //
// FourNodeQuadUP is a 4-node plane strain element for solid-fluid fully     //
// coupled analysis. This implementation is a simplified u-p formulation     //
// of Biot theory (u - solid displacement, p - fluid pressure). Each element //
// node has two DOFs for u and 1 DOF for p.                                  //
// Written by Zhaohui Yang	(May 2002)                                   //
// based on FourNodeQuad element by Michael Scott                            //
///////////////////////////////////////////////////////////////////////////////

// $Revision: 1.1 $
// $Date: 2005/09/22 21:28:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/UP-ucsd/FourNodeQuadUP.h,v $

#ifndef FourNodeQuadUP_h
#define FourNodeQuadUP_h

#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/physical_properties/SolidMech2D.h"
#include "domain/mesh/element/body_forces/BodyForces2D.h"

namespace XC {
class Node;
class NDMaterial;
class Response;

//! \ingroup ElemPlanos
//
//! @brief Cuadrilátero de cuatro nodos UP.
class FourNodeQuadUP : public QuadBase4N<SolidMech2D>
  {
  private:

    static Matrix K;		// Element stiffness, damping, and mass Matrix
    static Vector P;		// Element resisting force vector
    BodyForces2D bf;  //!< Body forces
    Vector pressureLoad;	// Pressure load at nodes

    double rho;			// Fluid mass per unit volume
    double kc;   // combined bulk modulus
    double pressure;	// Normal surface traction (pressure) over entire element
    // Note: positive for outward normal
    double perm[2];  // lateral/vertical permeability

    static double shp[3][4][4];	// Stores shape functions and derivatives (overwritten)
    static double pts[4][2];	// Stores quadrature points
    static double wts[4];		// Stores quadrature weights
    static double dvol[4];  // Stores detJacobian (overwritten)
    static double shpBar[3][4]; // Stores averaged shap functions (overwritten)

    Node *nd1Ptr(void);
    const Node *nd1Ptr(void) const;
    Node *nd2Ptr(void);
    const Node *nd2Ptr(void) const;
    Node *nd3Ptr(void);
    const Node *nd3Ptr(void) const;
    Node *nd4Ptr(void);
    const Node *nd4Ptr(void) const;

    // private member functions - only objects of this class can call these
    double mixtureRho(int ipt) const;  // Mixture mass density at integration point i
    void shapeFunction(void) const;
    void setPressureLoadAtNodes(void);

    mutable Matrix *Ki;
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    FourNodeQuadUP(int tag, int nd1, int nd2, int nd3, int nd4, NDMaterial &m,const std::string &type, double t, double bulk, double rhof, double perm1, double perm2,const BodyForces2D &bForces= BodyForces2D(), double p = 0.0);
    Element *getCopy(void) const;
    FourNodeQuadUP();
    virtual ~FourNodeQuadUP();

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element
    int update(void);

    // public methods to obtain stiffness, mass, damping and residual information
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);


    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
  };
} // end of XC namespace

#endif

