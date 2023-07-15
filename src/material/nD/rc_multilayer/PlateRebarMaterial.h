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
//===============================================================================
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
                                                                        
// $Revision: 1.0 $
// $Date: 2012-05-23 21:11:45 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/PlateRebarMaterial.h,v $

// Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)
//
// Generic Plate Rebar Material
//
/* Ref: Lu X, Lu XZ, Guan H, Ye LP, Collapse simulation of reinforced 
concrete high-rise building induced by extreme earthquakes, 
Earthquake Engineering & Structural Dynamics, 2013, 42(5): 705-723*/

#ifndef PlateRebarMaterial_h
#define PlateRebarMaterial_h

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "material/nD/NDMaterial.h"

namespace XC {
  class UniaxialMaterial;

//! @ingroup NDMat
//
//! @brief Reinforcing steel in multilayer reinforced concrete materials.
class PlateRebarMaterial: public NDMaterial
  {
  private :
    UniaxialMaterial *theMaterial;
    double angle, c, s;

    Vector strain;
    static Vector stress;
    static Matrix tangent;
  protected:
    void free_mem(void);
    void copy(const UniaxialMaterial *);    
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public: 
    PlateRebarMaterial(int tag= 0);
    PlateRebarMaterial(int tag, const UniaxialMaterial &uniMat, const double &ang );

    PlateRebarMaterial(const PlateRebarMaterial &);
    PlateRebarMaterial &operator=(const PlateRebarMaterial &);
    virtual ~PlateRebarMaterial(void);

    //make a clone of this material
    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;

    const UniaxialMaterial *getMaterial(void) const;
    UniaxialMaterial *getMaterial(void);
    virtual void setMaterial(const UniaxialMaterial &);
    void setMaterial(const std::string &);
    inline double getAngle(void) const
      { return this->angle; }
    void setAngle(const double &);
    
    //send back order of strain in vector form
    int getOrder(void) const;

    //send back order of strain in vector form
    const std::string &getType(void) const;

    //swap history variables
    int commitState(void); 

    //revert to last saved state
    int revertToLastCommit(void);

    //revert to start
    int revertToStart(void);

    //get the strain 
    int setTrialStrain( const Vector &strainFromElement );

    //send back the strain
    const Vector &getStrain(void) const;

    //send back the stress 
    const Vector &getStress(void) const;

    //send back the tangent 
    const Matrix &getTangent(void) const;

    const Matrix &getInitialTangent(void) const;  // AV Not Sure if it works

    //density
    double getRho(void) const;

    void Print(std::ostream &s, int flag = 0) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif






