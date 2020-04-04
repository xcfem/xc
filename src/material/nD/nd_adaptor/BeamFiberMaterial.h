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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/BeamFiberMaterial.h,v $

// Written: MHS
// Created: Aug 2001
//

#include <utility/matrix/Matrix.h>
#include <material/nD/NDAdaptorMaterial.h>

namespace XC {
//! @ingroup NDMat
//
//! @brief BeamFiberMaterial class is a wrapper class that performs static
//! condensation on a three-dimensional material model to give the 11, 12, and 13
//! stress components which can then be integrated over an area to model a
//! shear flexible 3D beam.
class BeamFiberMaterial: public NDAdaptorMaterial
  {
  private:
    double Tstrain33;
    double Tgamma23;
    double Cstrain33;
    double Cgamma23;

    static Vector stress;
    static Matrix tangent;

    int indexMap(int i) const;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    BeamFiberMaterial(int tag, NDMaterial &theMat);
    BeamFiberMaterial(void);

    int setTrialStrain( const Vector &strainFromElement);
    const Vector &getStress(void) const;
    const Matrix &getTangent(void) const;
    const Matrix &getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;
    const std::string &getType(void) const;
    int getOrder(void) const; 

    void Print(std::ostream &s, int flag) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace





