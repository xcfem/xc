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
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/ElasticSection3d.h,v $
                                                                        
                                                                        
///////////////////////////////////////////////////////
// File:  ~/Src/element/hinge/ElasticSection3d.h
//
// Written: MHS
// Date: May 2000
//
//
// Purpose:  This header file contains the prototype
// for the ElasticSection class.

#ifndef ElasticSection3d_h
#define ElasticSection3d_h

#include <material/section/elastic_section/BaseElasticSection3d.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class Channel;
class FEM_ObjectBroker;
class Information;

//! \ingroup MATSCCElastica
//
//! @brief Cross section with linear elastic material
//! in three-dimensional problems (6 degrees of freedom in each section).
class ElasticSection3d: public BaseElasticSection3d
  {
  private:   
    static Vector s;
  protected:

  public:
    ElasticSection3d(int tag, MaterialLoader *mat_ldr= NULL, const CrossSectionProperties3d &ctes= CrossSectionProperties3d());
    ElasticSection3d(int tag, double E, double A, double Iz, double Iy, double G, double J);
    ElasticSection3d(int tag, double EA, double EIz, double EIy, double GJ);
    ElasticSection3d(void);

    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
    const Matrix &getInitialFlexibility(void) const;
    
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    

    void Print (std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace

#endif
