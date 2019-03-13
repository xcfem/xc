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
                                                                        
// $Revision$
// $Date$
// $Source$

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for DispBeamColumnNL2d.
// The element displacement field gives rise to constant axial strain and
// linear curvature.

#ifndef DispBeamColumnNL2d_h
#define DispBeamColumnNL2d_h


#include "DispBeamColumn2dBase.h"

namespace XC {
class Node;
class SectionForceDeformation;
class CrdTransf2d;
class Response;
class BeamIntegration;

class DispBeamColumnNL2d : public DispBeamColumn2dBase
  {
  private:
    const Matrix &getInitialBasicStiff(void) const;
    void getBasicStiff(Matrix &kb, int initial = 0) const;
    BeamIntegration *beamIntegration;
  public:
    DispBeamColumnNL2d(int tag, int nd1, int nd2,int numSections, const std::vector<PrismaticBarCrossSection *> &s,
		     BeamIntegration &bi, CrdTransf2d &coordTransf,
		     double rho = 0.0);
    DispBeamColumnNL2d(void);
    ~DispBeamColumnNL2d(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    int update(void);
    const Matrix &getTangentStiff(void);
    const Matrix &getInitialStiff(void);

    const Vector &getResistingForce(void);


    Response *setResponse(const std::vector<std::string> &argv, Information &eleInfo);
    int getResponse(int responseID, Information &eleInfo);
    int getResponseSensitivity(int responseID, int gradNumber,
			       Information &eleInformation);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    const Vector & getResistingForceSensitivity(int gradNumber);
    const Matrix & getInitialStiffSensitivity(int gradNumber);
    const Matrix & getMassSensitivity(int gradNumber);
    int            commitSensitivity(int gradNumber, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif

