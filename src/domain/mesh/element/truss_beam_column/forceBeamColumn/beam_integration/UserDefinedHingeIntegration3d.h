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

// $Revision: 1.3 $
// $Date: 2003/06/10 00:36:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedHingeIntegration3d.h,v $

#ifndef UserDefinedHingeIntegration3d_h
#define UserDefinedHingeIntegration3d_h

#include "UserDefinedHingeIntegrationBase.h"

#include <utility/matrix/Vector.h>
#include "material/section/repres/CrossSectionProperties3d.h"

namespace XC {

//! \ingroup BeamInteg
//
//! @brief Integración en la barra definida por el usuario.
class UserDefinedHingeIntegration3d : public UserDefinedHingeIntegrationBase
  {
  protected:
    CrossSectionProperties3d ctes_scc; //Mechanical properties of the section E,A,Iy,...
  public:
    UserDefinedHingeIntegration3d(int npL, const Vector &ptL, const Vector &wtL,
				int npR, const Vector &ptR, const Vector &wtR,
				double E, double A, double Iz,
				double Iy, double G, double J);
    UserDefinedHingeIntegration3d(int npL, const Vector &ptL, const Vector &wtL,
				  int npR, const Vector &ptR, const Vector &wtR,
				  const CrossSectionProperties3d &cts);
    UserDefinedHingeIntegration3d(const CrossSectionProperties3d &cts= CrossSectionProperties3d());
  
    void getSectionLocations(int numSections, double L, double *xi) const;
    void getSectionWeights(int numSections, double L, double *wt) const;

    void addElasticDeformations(ElementalLoad *theLoad, double loadFactor,double L, double *v0);
    int addElasticFlexibility(double L, Matrix &fe);

    BeamIntegration *getCopy(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);

    void Print(std::ostream &s, int flag = 0);  
  };
} // end of XC namespace

#endif
