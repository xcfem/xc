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
// $Date: 2012-05-21 23:49:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/LayeredShellFiberSection.h,v $

// Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)
//
// Layered Shell Section
//
/* Ref: Lu X, Lu XZ, Guan H, Ye LP, Collapse simulation of reinforced 
concrete high-rise building induced by extreme earthquakes, 
Earthquake Engineering & Structural Dynamics, 2013, 42(5): 705-723*/

#ifndef LayeredShellFiberSection_h
#define LayeredShellFiberSection_h

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "MembranePlateFiberSectionBase.h"
namespace XC {

//! @brief Fiber model for plate/membrane materials.
//! @ingroup MATPLAC
class LayeredShellFiberSection : public MembranePlateFiberSectionBase
  {
  private:
    //quadrature data
    std::vector<double> sg; //Gauss integration.
    std::vector<double> wg;

    static ResponseId array;
  protected:
    void setHWgSg(const std::vector<double> &);
  public:
    //null constructor
    LayeredShellFiberSection(int tag= 0);

    //full constructor
    LayeredShellFiberSection(int tag, const std::vector<double> &, const std::vector<NDMaterial *> &);

    //make a clone of this material
    SectionForceDeformation *getCopy(void) const;

    void setThicknesses(const boost::python::list &);
    void setupPy(const boost::python::list &);
    
    std::vector<double> getFiberZs(void) const;

    //mass per unit area
    double getRho(void) const;

    Response *setResponse(const std::vector<std::string> &, Information &);
    int getResponse(int responseID, Information &info);

    // parameters
    int setParameter(const std::vector<std::string> &, Parameter &);
    int updateParameter(int parameterID, Information& info);

    //send back order of strain in vector form
    const ResponseId &getResponseType(void) const;

    int setInitialSectionDeformation(const Vector &);
     //get the strain and integrate plasticity equations
    int setTrialSectionDeformation(const Vector &strain_from_element );

    //send back the stress 
    const Vector &getStressResultant(void) const;

    //send back the tangent 
    const Matrix &getSectionTangent(void) const;

    //send back the initial tangent 
    const Matrix& getInitialTangent(void) const
      { return this->getSectionTangent(); }

    //print out data
    void Print( std::ostream &s, int flag ) const;

  }; //end of LayeredShellFiberSection declarations

} // end of XC namespace

#endif



