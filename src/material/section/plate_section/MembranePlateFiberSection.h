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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/MembranePlateFiberSection.h,v $

// Ed "C++" Love
//
// Generic Plate Section with membrane
//

#ifndef MEMBRANEPLATEFIBERSECTION_H
#define MEMBRANEPLATEFIBERSECTION_H

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "PlateBase.h"


namespace XC {

class NDMaterial;

//! \ingroup MATPLAC
//
//! @brief Fiber model for plate/membrane materials.
class MembranePlateFiberSection: public PlateBase
  {
  private:
    //quadrature data
    static const double sg[5];
    static const double wg[5];
    NDMaterial *theFibers[5];  //pointers to five materials (fibers)
    static const double root56; // =sqrt(5/6) 
    Vector strainResultant;
    static Vector stressResultant;
    static Matrix tangent;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public : 
    MembranePlateFiberSection(int tag);
    MembranePlateFiberSection(void);
    MembranePlateFiberSection(int tag, double thickness, NDMaterial &Afiber);
    virtual ~MembranePlateFiberSection(void);

    SectionForceDeformation *getCopy(void) const;
    double getRho(void) const;
    int getOrder(void) const;
    const ResponseId &getType(void) const;

    
    int commitState(void); //swap history variables
    int revertToLastCommit(void); //revert to last saved state
    int revertToStart(void); //revert to start

    int setInitialSectionDeformation(const Vector &strain_from_element);
    void zeroInitialSectionDeformation(void);
    int setTrialSectionDeformation(const Vector &strain_from_element);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector& getSectionDeformation(void) const; //send back the strain
    const Vector &getStressResultant(void) const; //send back the stress 
    const Matrix &getSectionTangent(void) const; //send back the tangent 
    const Matrix &getInitialTangent(void) const //send back the initial tangent 
      {return this->getSectionTangent();}

    //print out data
    void Print( std::ostream &s, int flag );

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  }; //end of MembranePlateFiberSection declarations

} // end of XC namespace

#endif
