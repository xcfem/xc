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
// $Date: 2003/10/07 21:18:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/FourNodeQuad.h,v $
                                                                        
// Written: MHS
// Created: Feb 2000
// Revised: Dec 2000 for efficiency
//
// Description: This file contains the class definition for FourNodeQuad.

#ifndef FourNodeQuad_h
#define FourNodeQuad_h

#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/utils/physical_properties/SolidMech2D.h"
#include "domain/mesh/element/utils/body_forces/BodyForces2D.h"

namespace XC {
class NDMaterial;
class Material;
class Response;
class GaussPoint;

//! @ingroup PlaneElements
//
//! @brief Four node quad.
class FourNodeQuad: public QuadBase4N<SolidMech2D>
  {
  private:    
    BodyForces2D bf; //!< Body forces.
    Vector pressureLoad; //!< Pressure load at nodes

    double pressure; //!< Normal surface traction (pressure) over entire element (note: positive for outward normal).
    mutable Matrix *Ki;

    static double matrixData[64]; //!< array data for matrix
    static Matrix K; //!< Element stiffness, damping, and mass Matrix
    static Vector P; //!< Element resisting force vector
    static double shp[3][4]; //!< Stores shape functions and derivatives (overwritten)

    // private member functions - only objects of this class can call these
    double shapeFunction(const GaussPoint &gp) const;
    void setPressureLoadAtNodes(void);

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool check_material_type(const std::string &type) const;
  public:
    FourNodeQuad(int tag, int nd1, int nd2, int nd3, int nd4, NDMaterial &m, const std::string &type, double t, double pressure = 0.0, double rho = 0.0, const BodyForces2D &bForces= BodyForces2D());
    FourNodeQuad(int tag,const NDMaterial *ptr_mat);
    FourNodeQuad(void);
    Element *getCopy(void) const;
    virtual ~FourNodeQuad(void);

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int update(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;    
    const Matrix &getMass(void) const;    

    const GaussModel &getGaussModel(void) const;

    inline double getRho(void) const
      { return physicalProperties.getRho(); }
    void setRho(const double &r)
      { physicalProperties.setRho(r); }
    double getThickness(void) const
      { return physicalProperties.getThickness(); }
    void setThickness(const double &t)
      { physicalProperties.setThickness(t); }

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);

    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;

    double detJ(const double &,const double &) const;

  };
} // end of XC namespace

#endif

