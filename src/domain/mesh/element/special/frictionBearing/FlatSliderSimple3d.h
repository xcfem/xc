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

// $Revision: 1.2 $
// $Date: 2009/11/03 23:12:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/frictionBearing/FlatSliderSimple3d.h,v $

#ifndef FlatSliderSimple3d_h
#define FlatSliderSimple3d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for FlatSliderSimple3d.
// FlatSliderSimple3d is a friction slider element defined by two nodes. This
// simplified version uses small angle approximations and accounts for
// rotations of the sliding surface by shifting the shear forces.

#include "FrictionElementBase.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class Channel;
class FrictionModel;
class UniaxialMaterial;
class Response;

//! @brief Flat slider bearing element for two dimensional problems.
//!
//! This element is defined by two nodes. The iNode represents the flat
//! sliding surface and the jNode represents the slider. The element can
//! have zero length or the appropriate bearing height. The bearing has
//! unidirectional (2D) or coupled (3D) friction properties for the shear
//! deformations, and force-deformation behaviors defined by UniaxialMaterials
//! in the remaining four (3D) directions. To capture the uplift behavior
//! of the bearing, the user-specified UniaxialMaterial in the axial direction
//! is modified for no-tension behavior. By default (sDratio = 0.0) P-Delta
//! moments are entirely transferred to the flat sliding surface (iNode). It
//! is important to note that rotations of the flat sliding surface (rotations
//! at the iNode) affect the shear behavior of the bearing. To avoid the
//! introduction of artificial viscous damping in the isolation system
//! (sometimes referred to as "damping leakage in the isolation system"), the
//! bearing element does not contribute to the Rayleigh damping by default. If
//! the element has non-zero length, the local x-axis is determined from the
//! nodal geometry unless the optional x-axis vector is specified in which
//! case the nodal geometry is ignored and the user-defined orientation
//! is utilized. 
class FlatSliderSimple3d: public FrictionElementBase
  {
  private:
    
    // state variables
    Vector ubPlastic;   // plastic displacements in basic system
    
    // committed history variables
    Vector ubPlasticC;  // plastic displacements in basic system

    static Matrix theMatrix;
    static Vector theVector;

    // private methods
    void setUp();
    double sgn(double x);
    int sendData(Communicator &);
    int recvData(const Communicator &);
    
  public:
    // constructor
    FlatSliderSimple3d(int tag, int Nd1, int Nd2,const FrictionModel &theFrnMdl, double uy,const std::vector<UniaxialMaterial *> &theMaterials, const Vector &y= Vector(), const Vector &x= Vector(),const  double &mass= 0.0,const int &maxIter = 20,const double &tol= 1E-8);
    FlatSliderSimple3d(void);
    
    // public methods to obtain information about dof & connectivity    
    int getNumDOF();
    void setDomain(Domain *theDomain);
    
    // public methods to set the state of the element    
    int commitState();
    int revertToLastCommit();        
    int revertToStart();        
    int update();
    
    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff();
    const Matrix &getInitialStiff();
    const Matrix &getMass();
    
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;
    
    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag = 0) const;    
    
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  };
} // end of XC namespace

#endif
