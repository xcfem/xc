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
// $Date: 2012-05-27 21:11:45 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/PlateFromPlaneStressMaterial.h,v $

// Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)
//
// Generic Plate Material from Plane Stress Material
//
/* Ref: Lu X, Lu XZ, Guan H, Ye LP, Collapse simulation of reinforced 
concrete high-rise building induced by extreme earthquakes, 
Earthquake Engineering & Structural Dynamics, 2013, 42(5): 705-723*/

#ifndef PlateFromPlaneStressMaterial_h
#define PlateFromPlaneStressMaterial_h

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "material/nD/NDMaterialWrapper.h"
#include "material/nD/rc_multilayer/PlateAdaptorMaterial.h"

namespace XC {

//
//! @brief Reinforcing steel in multilayer reinforced concrete materials.
//! @ingroup NDMat
class PlateFromPlaneStressMaterial: public PlateAdaptorMaterial<NDMaterialWrapper>
  {
  private:
    Vector strain;
    double gmod; //! out of plane shear modulus.

    static Vector stress;
    static Matrix tangent;
  protected:
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public : 
    PlateFromPlaneStressMaterial(int tag= 0);
    PlateFromPlaneStressMaterial(int tag, NDMaterial &ndMat, const double &g);

    inline double getGmod(void) const
      { return this->gmod; }
    void setGmod(const double &);
    
    //make a clone of this material
    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;

    //swap history variables
    int commitState(void); 

    //revert to last saved state
    int revertToLastCommit(void);

    //revert to start
    int revertToStart(void);

    //get the strain 
    int setTrialStrain(const Vector &strainFromElement );

    //send back the strain
    const Vector &getStrain(void) const;

    //send back the stress 
    const Vector &getStress(void) const;

    //send back the tangent 
    const Matrix &getTangent(void) const;

    const Matrix &getInitialTangent(void) const;  // AV Not Sure if it works

    //print out data
    void Print(std::ostream &s, int flag = 0) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    //setResponse - added by V.K. Papanikolaou [AUTh] - start
    Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    //setResponse - added by V.K. Papanikolaou [AUTh] - end

  };

} // end of XC namespace

#endif





