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
//RayleighDampingFactors.h

#ifndef RayleighDampingFactors_h
#define RayleighDampingFactors_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"

class Pos3dArray3d;

namespace XC {

class Vector;
class Information;

//! @ingroup FEMisc
//
//! @brief Rayleigh damping factors.
//!
//! Objects of this class are used to assign damping to all
//! previously-defined elements and nodes. When using rayleigh damping
//! in XC, the damping matrix for an element or node, D is specified as a
//! combination of stiffness and mass-proportional damping matrices:
//! D = alphaM * M + betaK * Kcurrent + betaKinit * Kinit + betaKcomm * KlastCommit
//! References:
//! - The following paper is something that you should read if you are new
//!   to the damping and nonlinear analysis. Finley A. Charney, <a href="ftp://jetty.ecn.purdue.edu/ayhan/Fabian/Damping/Unintended%20Consequences%20of%20Modeling%20Damping_Finley2008.pdf">"Unintended Consequences of Modeling Damping in Structures"</a>, J. Struct. Engrg.
//!    Volume 134, Issue 4, pp. 581-592 (April 2008).
//! - Petrini, Lorenza , Maggi, Claudio , Priestley, M. J. Nigel and Calvi,
//! G. Michele (2008) <a href="https://www.researchgate.net/publication/232862035_Experimental_Verification_of_Viscous_Damping_Modeling_for_Inelastic_Time_History_Analyzes">"Experimental Verification of Viscous Damping Modeling for Inelastic Time History Analyzes"</a>, Journal of Earthquake Engineering, 12:S1, pp. 125 — 145
//! - Hall, J. F.(2006) <a href="https://www.usbr.gov/ssle/damsafety/TechDev/DSOTechDev/DSO-07-03.pdf">"Problems encountered from the use (or misuse) of Rayleigh damping"</a> Earthquake Engineering and Structural Dynamic, 35, 525–545 
class RayleighDampingFactors: public EntCmd, public MovableObject
  {
    double alphaM; //!< factor applied to elements or nodes mass matrix. 
    double betaK; //!< factor applied to elements current stiffness matrix.
    double betaK0; //!< factor applied to elements initial stiffness matrix. 
    double betaKc; //!< factor applied to elements committed stiffness matrix.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    RayleighDampingFactors(void);
    RayleighDampingFactors(const double &alphaM,const double &betaK,const double &betaK0,const double &betaKc);
    explicit RayleighDampingFactors(const Vector &);

    //! @brief return the damping factor applied to elements or nodes
    //! mass matrix 
    inline const double &getAlphaM(void) const
      { return alphaM; }
    //! @brief set the damping factor applied to elements or nodes
    //! mass matrix 
    inline void setAlphaM(const double &d)
      { alphaM= d; }
    //! @brief return the damping factor applied to elements current
    //! stiffness matrix 
    inline const double &getBetaK(void) const
      { return betaK; }
    //! @brief set the damping factor applied to elements current
    //! stiffness matrix 
    inline void setBetaK(const double &d)
      { betaK= d; }
    //! @brief return the damping factor applied to elements initial
    //! stiffness matrix 
    inline const double &getBetaK0(void) const
      { return betaK0; }
    //! @brief set the damping factor applied to elements initial
    //! stiffness matrix 
    inline void setBetaK0(const double &d)
      { betaK0= d; }
    //! @brief return the damping factor applied to elements committed
    //! stiffness matrix 
    inline const double &getBetaKc(void) const
      { return betaKc; }
    //! @brief set the damping factor applied to elements
    //! committed stiffness matrix 
    inline void setBetaKc(const double &d)
      { betaKc= d; }
    //! @brief Returns true if all Rayleigh factors are zero.
    inline bool nullValues(void) const
      { return (alphaM == 0.0 && nullKValues()); }
    //! @brief Returns true if all Rayleigh stiffness factors are zero.
    inline bool nullKValues(void) const
      { return (betaK == 0.0 && betaK0 == 0.0 && betaKc == 0.0); }
    int updateParameter(int parameterID, Information &info);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag= 0) const;

  };

std::ostream &operator<<(std::ostream &,const RayleighDampingFactors &);

} // end of XC namespace

#endif

