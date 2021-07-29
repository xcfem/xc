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

#ifndef ChebyshevBeamIntegration_h
#define ChebyshevBeamIntegration_h

#include "BeamIntegration.h"

namespace XC {

#define BEAM_INTEGRATION_TAG_Chebyshev 1234

//! @brief Chebyshev integration scheme.
//! @ingroup BeamInteg
class ChebyshevBeamIntegration : public BeamIntegration
  {
  private:
    int type;
  public:
    ChebyshevBeamIntegration(int type = 1);

    void getSectionLocations(int nIP, double L, double *xi) const;
    void getSectionWeights(int nIP, double L, double *wt) const;

    BeamIntegration *getCopy(void) const;

    // These two methods do nothing
    int sendSelf(Communicator &)
      {return 0;}
    int recvSelf(const Communicator &)
      {return 0;}
    void Print(std::ostream &, int flag = 0) const;  
  };
} // end of XC namespace

#endif
