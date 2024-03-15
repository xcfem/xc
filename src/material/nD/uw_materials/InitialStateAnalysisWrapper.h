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

#ifndef InitialStateAnalysisWrapper_h
#define InitialStateAnalysisWrapper_h

// Written: Chris McGann
//          February 2011

// Description: This file contains the class definition for InitialStateAnalysisWrapper.

#include <material/nD/NDMaterial.h>
#include "material/nD/NDMaterialWrapper.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>

namespace XC {

//! @brief Initial state analysis wrapper material.
//! @ingroup UWMat
class InitialStateAnalysisWrapper : public NDMaterial
  {
  protected:
    static bool initialStateAnalysis;
    NDMaterialWrapper theMainMaterial; // pointer to main material object

    // input variables
    int mDIM; //!< number of dimensions in problem
    
    // member variables
    Vector mEpsilon_o; //!< initial strain stored here
    Vector mStrain; //!< strain sent to the main material

    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:

    InitialStateAnalysisWrapper(int tag= 0);
    // full constructor
    InitialStateAnalysisWrapper(int tag, NDMaterial &mainMat, int ndim);
    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
		
    // set the strain to be sent to the main material
    int setTrialStrain(const Vector &strain_from_element);

    virtual void setMaterial(const NDMaterial &);
    void setMaterial(const std::string &);
    void setup(int ndim, NDMaterial &);
    
    // get mass density from main material
    double getRho(void) const;

    static bool getInitialStateAnalysisPhase(void);
    static void setInitialStateAnalysisPhase(const bool &);

    // send back strain
    const Vector& getStrain() const;
    // send back stress
    const Vector& getStress() const;
    // send back the tangent
    const Matrix &getTangent() const;
    const Matrix &getInitialTangent() const;

    NDMaterial *getCopy(const std::string &) const;
    NDMaterial *getCopy(void) const;
    const std::string &getType(void) const;
    int getOrder(void) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &matInfo);
    int getResponse (int responseID, Information &matInformation);

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &); 

    void Print(std::ostream &, int flag =0) const;

    int setParameter(const std::vector<std::string> &, Parameter &);
    int updateParameter(int parameterID, Information &info);
	
    friend class PyLiq1;
    friend class TzLiq1;
    friend class QzLiq1; // Sumeet

    int getMainClassTag() const; // sends class tag of main material object
  };
} // end XC namespace
#endif
