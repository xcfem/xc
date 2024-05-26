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

#ifndef ContactMaterialBase_h
#define ContactMaterialBase_h

#include <material/nD/NDMaterial.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>

namespace XC {

//! @brief Base class for ND contact materials.
//! @ingroup UWMat
class ContactMaterialBase: public NDMaterial
  {
  protected:

    //material parameters
    double frictionCoeff;   // friction coefficient tan(phi)
    double stiffness;       // shear stiffness of the interface
    double cohesion;        // interface cohesion (force)
    double tensileStrength;  // tensile strength

    // functions
    int UpdateFrictionalState(void);

    // variables for update of friction coefficient
    bool mFrictFlag;
    int mFlag;
    double mMu;
    double mCo;
    double mTen;

    int sendData(Communicator &);  
    int recvData(const Communicator &);

    // state variables

    bool inSlip;            // sliding indicator

    // static vectors and matrices
    Vector strain_vec;      // generalized strain state
    // strain_vec(0) -> gap     ... current gap distance
    // strain_vec(1) -> slip    ... incremental slip
    // strain_vec(2) -> lambda  ... lagrangean multiplier -> t_n

    Vector stress_vec;      // generalized stress state
        // stress_vec(0) -> t_n     ... normal contact force
        // stress_vec(1) -> t_s     ... tangential contact force
        // stress_vec(2) -> gap     ... current gap

    mutable Matrix tangent_matrix;  // material tangent
  public:
    // Null constructor
    ContactMaterialBase(int tag, int classTag, int dim);
    // Full constructor
    ContactMaterialBase(int tag, int classTag, int dim, double mu, double G, double c, double t);


    // Calculates current tangent stiffness.
    const Matrix &getInitialTangent(void) const;

    // Calculates the corresponding stress increment (rate),
    // for a given strain increment. 
    const Vector &getStress(void) const;
    const Vector &getStrain(void) const;

    bool getFrictionFlag(void) const
      { return this->mFrictFlag; }
    void setFrictionFlag(const bool &b)
      { this->mFrictFlag= b; }
    double getFrictionCoeff(void) const
      { return this->frictionCoeff; }
    void setFrictionCoeff(const double &fc)
      { this->frictionCoeff= fc; }
    double getStiffness(void) const
      { return this->stiffness; }
    void setStiffness(const double &s)
      { this->stiffness= s; }
    //Get cohesion function for use in contact element
    double getcohesion(void) const;
    void setcohesion(const double &s)
      { this->cohesion= s; }
    void ScaleCohesion(const double len);

    //Get tensile strength function for use in contact element
    double getTensileStrength(void) const;
    void setTensileStrength(const double &ts)
      { this->tensileStrength= ts; }
    void ScaleTensileStrength(const double len);

    bool getContactState(void) const
      {return inSlip;}
  
    // Revert the stress/strain states to the last committed states.
    // Return 0 on success.
    int revertToLastCommit ();

    // public methods for material stage update
    int setParameter(const std::vector<std::string> &, Parameter &);
    int updateParameter(int responseID, Information &eleInformation);
  };
} // end XC namespace

#endif

