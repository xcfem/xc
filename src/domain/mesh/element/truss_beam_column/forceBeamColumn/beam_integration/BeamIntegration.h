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
// $Date: 2006/01/17 21:24:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/BeamIntegration.h,v $

#ifndef BeamIntegration_h
#define BeamIntegration_h

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/nucleo/EntCmd.h"

class ExprAlgebra;

namespace XC {
class Matrix;
class ElementalLoad;
class Information;
class FVector;
class CrdTransf;
class Vector;

//! \ingroup OneDimensionalElemF
//
//! @defgroup BeamInteg Function integration over the element domain.
//
//! \ingroup BeamInteg
//! @brief Base class for integration on beam elements.
class BeamIntegration: public MovableObject, public EntCmd
  {
  public:
    BeamIntegration(int classTag);
    inline virtual ~BeamIntegration(void) {}

    virtual void getSectionLocations(int nIP, double L, double *xi) const= 0;
    virtual void getSectionWeights(int nIP, double L, double *wt) const= 0;
    const Matrix &getIntegrPointCoords(int nIP, double L) const;
    const Matrix &getIntegrPointNaturalCoords(int nIP, double L) const;
    const Matrix &getIntegrPointLocalCoords(int nIP, double L) const;

    const Matrix &getIntegrPointNaturalCoords(int nIP,const CrdTransf &trf) const;
    const Matrix &getIntegrPointNormalizedCoords(int nIP,const CrdTransf &trf) const;
    const Matrix &getIntegrPointLocalCoords(int nIP,const CrdTransf &trf) const;
    const Matrix &getIntegrPointGlobalCoords(int nIP,const CrdTransf &trf) const;

    const Vector &evalInIntegrPoints(const ExprAlgebra &expr,int nIP,const CrdTransf &trf) const;
    double getIntegral(const ExprAlgebra &expr,int nIP,const CrdTransf &trf) const;

    virtual void addElasticDeformations(ElementalLoad *theLoad,double loadFactor,double L,FVector &v0) {return;}
    // Return 0 if there is no elastic interior, -1 otherwise
    virtual int addElasticFlexibility(double L, Matrix &fe) {return 0;}

    virtual double getTangentDriftI(double L, double LI, double q2,double q3, bool yAxis = false) {return 0.0;}
    virtual double getTangentDriftJ(double L, double LI, double q2,double q3, bool yAxis = false) {return 0.0;}

    virtual BeamIntegration *getCopy(void) const= 0;

    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);

    virtual void getLocationsDeriv(int nIP, double L, double dLdh,double *dptsdh);
    virtual void getWeightsDeriv(int nIP, double L, double dLdh,double *dwtsdh);
  // Return 0 if there is no elastic interior, -1 otherwise
    virtual int addElasticFlexDeriv(double L, Matrix &dfedh,double dLdh = 0.0) {return 0;}

    virtual void Print(std::ostream &s, int flag = 0) = 0;

  };

int sendBeamIntegrationPtr(BeamIntegration *,int posClassTag, int posDbTag,DbTagData &,CommParameters &);
BeamIntegration *receiveBeamIntegrationPtr(BeamIntegration *,int posClassTag, int posDbTag,DbTagData &,const CommParameters &);

} // end of XC namespace

#endif
