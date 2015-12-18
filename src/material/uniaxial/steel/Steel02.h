//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

// $Revision: 1.2 $
// $Date: 2006/08/03 23:42:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Steel02.h,v $

// Written: fmk
// Created: 03/06
//
// Description: This file contains the class definition for
// Steel02. Steel02 is based on an f2c of the FEDEAS material
// Steel02.f which is:
//-----------------------------------------------------------------------
// MENEGOTTO-PINTO STEEL MODEL WITH FILIPPOU ISOTROPIC HARDENING
//            written by MOHD YASSIN (1993)
//          adapted to FEDEAS material library
//    by E. Spacone, G. Monti and F.C. Filippou (1994)
//-----------------------------------------------------------------------


#ifndef Steel02_h
#define Steel02_h

#include "material/uniaxial/steel/SteelBase.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Modelización del acero 02.
class Steel02 : public SteelBase
  {
  private:
    double sigini; //!< Tensión inicial.
    // matpar : STEEL FIXED PROPERTIES
    double R0;  //!<  = matpar(4)  : exp transition elastic-plastic
    double cR1; //!<  = matpar(5)  : coefficient for changing R0 to R
    double cR2; //!<  = matpar(6)  : coefficient for changing R0 to R
    // hstvP : STEEL HISTORY VARIABLES
    double epsminP; //!<  = hstvP(1) : max eps in compression
    double epsmaxP; //!<  = hstvP(2) : max eps in tension
    double epsplP;  //!<  = hstvP(3) : plastic excursion
    double epss0P;  //!<  = hstvP(4) : eps at asymptotes intersection
    double sigs0P;  //!<  = hstvP(5) : sig at asymptotes intersection
    double epssrP;  //!<  = hstvP(6) : eps at last inversion point
    double sigsrP;  //!<  = hstvP(7) : sig at last inversion point
    int    konP;    //!<  = hstvP(8) : index for loading/unloading
    // hstv : STEEL HISTORY VARIABLES
    double epsP;  //!<  = strain at previous converged step
    double sigP;  //!<  = stress at previous converged step
    double eP;    //!<   stiffness modulus at last converged step;

    double epsmin;
    double epsmax;
    double epspl;
    double epss0;
    double sigs0;
    double epsr;
    double sigr;
    int kon;
    double sig;
    double e;
    double eps;   //!<  = strain at current step
  protected:
    int setup_parameters(void);
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    Steel02(int tag, double fy, double E0, double b,
            double R0, double cR1, double cR2,
            double a1, double a2, double a3, double a4, double sigInit =0.0);
    Steel02(int tag, double fy, double E0, double b,double R0, double cR1, double cR2);
    // Constructor for no isotropic hardening
    // Also provides default values for R0, cR1, and cR2
    Steel02(int tag, double fy, double E0, double b);
    Steel02(int tag);
    Steel02(void);

    UniaxialMaterial *getCopy(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void setInitialStress(const double &);
    inline double getInitialStress(void) const
      { return sigini; }


    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif

