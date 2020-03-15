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

// $Revision: 1.1 $
// $Date: 2007-10-12 21:03:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/FixedLocationBeamIntegration.cpp,v $

#include "FixedLocationBeamIntegration.h"

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


XC::FixedLocationBeamIntegration::FixedLocationBeamIntegration(const Vector &pt)
  : ParameterIDBeamIntegration(BEAM_INTEGRATION_TAG_FixedLocation,pt)
  {
    const int nIP = pts.Size();

    Vector R(nIP);
    for(int i = 0; i < nIP; i++)
      R(i) = 1.0/(i+1);

    Matrix J(nIP,nIP);
    for(int i = 0; i < nIP; i++)
      for(int j = 0; j < nIP; j++)
        J(i,j) = pow(pts(j),i);

    J.Solve(R, wts);
  }

XC::FixedLocationBeamIntegration::FixedLocationBeamIntegration(void)
  : ParameterIDBeamIntegration(BEAM_INTEGRATION_TAG_FixedLocation)
  {}


XC::BeamIntegration *XC::FixedLocationBeamIntegration::getCopy(void) const
  { return new FixedLocationBeamIntegration(*this); }

int XC::FixedLocationBeamIntegration::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    return -1;
  }

int XC::FixedLocationBeamIntegration::updateParameter(int parameterID, Information &info)
  {
    // Does nothing for now -- MHS
    return 0;
  }

void XC::FixedLocationBeamIntegration::Print(std::ostream &s, int flag) const
  {
    s << "FixedLocation" << std::endl;
    ParameterIDBeamIntegration::Print(s);
    s << " Points: " << pts;
    s << " Weights: " << wts;
    double sum = 0.0;
    int N = wts.Size();
    for (int i = 0; i < N; i++)
      sum += fabs(wts(i));
    s << " Condition Number: " << sum << std::endl;
  }

void XC::FixedLocationBeamIntegration::getLocationsDeriv(int numSections, double L, double dLdh, double *dptsdh) const
  {
    for(int i = 0; i < numSections; i++)
      dptsdh[i] = 0.0;
  }

void XC::FixedLocationBeamIntegration::getWeightsDeriv(int numSections, double L, double dLdh, double *dwtsdh) const
  {
    for(int i = 0; i < numSections; i++)
      dwtsdh[i] = 0.0;
  }
