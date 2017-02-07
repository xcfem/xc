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

// $Revision: 1.2 $
// $Date: 2004/09/01 04:01:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/joint/Joint3D.h,v $

// Written: Arash Altoontash, Gregory Deierlein
// Created: 04/03
// Revision: Arash

// Joint3D.h: interface for the Joint3D class.
//
//////////////////////////////////////////////////////////////////////

#ifndef Joint3D_h
#define Joint3D_h


#include <domain/mesh/element/ElemWithMaterial.h>
#include <domain/mesh/element/utils/physical_properties/Joint3DPhysicalProperties.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>

namespace XC {
class Node;
class UniaxialMaterial;
class Response;

//! \ingroup Elem
//
//! @defgroup SpecialElem Special purpose elements.
//
//! \ingroup SpecialElem
//
//! @defgroup ElemJoint Joint modelling elements.
//
//! \ingroup ElemJoint
//
//! @brief Joint element for three-dimensional problems.
class Joint3D: public ElemWithMaterial<7,Joint3DPhysicalProperties>  
  {
  private:
    ID ExternalNodes;
    ID InternalConstraints;
    int numDof, nodeDbTag, dofDbTag;
    static Matrix K;
    static Vector V;
  protected:
   int addMFreedom_Joint(Domain *theDomain, int mpNum, int RetNodeID, int ConNodeID,
                    int RotNodeID, int Rdof, int DspNodeID, int Ddof, 
                    int LrgDispFlag );   
  public:
    Joint3D(void);
    Joint3D(int tag, int nd1, int nd2, int nd3, int nd4, int nd5, int nd6, int IntNodeTag, const UniaxialMaterial &springx, const UniaxialMaterial &springy, const UniaxialMaterial &springz, Domain *theDomain, int LrgDisp);
    Element *getCopy(void) const;  
    ~Joint3D(void);
  
    // methods dealing with domain
    int getNumDOF(void) const;
  
    void setDomain(Domain *theDomain);  
    bool isSubdomain(void) { return false; } ;
        
    // methods dealing with committed state and update
    int update(void);
  
    // methods to return the current linearized stiffness,
    // damping and mass matrices
    const        Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;   
    const        Matrix &getDamp(void) const;
    const        Matrix &getMass(void) const;
        
    // methods for returning and applying loads
    //virtual Vector &getUVLoadVector(double q1, double q2);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const        Vector &getResistingForce(void) const;
    const        Vector &getResistingForceIncInertia(void) const;     

    // method for obtaining information specific to an element
    Response* setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif
