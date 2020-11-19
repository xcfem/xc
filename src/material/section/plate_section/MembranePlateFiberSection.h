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

//! @ingroup MATPLAC
//
//! @brief Fiber model for plate/membrane materials.
class MembranePlateFiberSection: public PlateBase
  {
  private:
    static constexpr int numFibers= 5;
    static constexpr int order= 8;
    
    //quadrature data
    static const double sg[numFibers];
    static const double wg[numFibers];
    static const double root56; //shear correction
    static Vector stressResultant;
    static Matrix tangent;
    
    NDMaterial *theFibers[numFibers];  //pointers to five materials (fibers)
    Vector strainResultant;
    Vector initialStrain;

    void init(void);
    void alloc(const NDMaterial &);
    void copy_fibers(const MembranePlateFiberSection &);
    void free(void);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public: 
    MembranePlateFiberSection(int tag= 0);
    MembranePlateFiberSection(int tag, double thickness, NDMaterial &Afiber);
    MembranePlateFiberSection(const MembranePlateFiberSection &);
    MembranePlateFiberSection &operator=(const MembranePlateFiberSection &);
    virtual ~MembranePlateFiberSection(void);

    inline void setMaterial(const NDMaterial &ndmat)
      { alloc(ndmat); }

    std::vector<double> getFiberZs(void) const;
    std::vector<double> getFiberWeights(void) const;
    std::vector<std::pair<double, double> > getFiberZsAndWeights(void) const;

    SectionForceDeformation *getCopy(void) const;
    double getRho(void) const;
    void setRho(const double &);
    double getArealDensity(void) const;
    void setArealDensity(const double &);
    int getOrder(void) const;
    const ResponseId &getType(void) const;
    
    int commitState(void); //swap history variables
    int revertToLastCommit(void); //revert to last saved state
    int revertToStart(void); //revert to start

    int setInitialSectionDeformation(const Vector &strain_from_element);
    void zeroInitialSectionDeformation(void);
    int setTrialSectionDeformation(const Vector &strain_from_element);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const; //send back the strain
    const Vector &getStressResultant(void) const; //send back the stress 
    const Matrix &getSectionTangent(void) const; //send back the tangent 
    const Matrix &getInitialTangent(void) const //send back the initial tangent 
      {return this->getSectionTangent();}
    Vector getVonMisesStressAtFibers(void) const;
    double getMinVonMisesStress(void) const;
    double getMaxVonMisesStress(void) const;
    double getAvgVonMisesStress(void) const;
    
    virtual Matrix getValues(const std::string &, bool silent= false) const;

    //print out data
    void Print( std::ostream &s, int flag ) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  }; //end of MembranePlateFiberSection declarations

} // end of XC namespace

#endif
