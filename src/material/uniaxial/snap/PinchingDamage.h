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
                                                                        
// $Revision: 1.1 $
// $Date: 2004/09/01 03:53:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/snap/PinchingDamage.h,v $
//
//
// PinchingDamage.h: implementation of the PinchingDamage class from Fortran version.
// Originally from SNAP PROGRAM by Prof H.K. Krawinkler
//
// Written: A. Altoontash & Prof. G. Deierlein 12/01
// Revised: 05/03
//
// Purpose: This file contains the implementation for the PinchingDamage class.
//
//////////////////////////////////////////////////////////////////////

// PinchingDamage.h: interface for the PinchingDamage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef PinchingDamage_H
#define PinchingDamage_H

#include <material/uniaxial/UniaxialMaterial.h>
#include <utility/matrix/Vector.h>
#include <stdio.h>

namespace XC {
class DamageModel;

//! @ingroup MatUnx
//
//! @brief ??.
class PinchingDamage : public UniaxialMaterial  
  {
 private:
  
  // Input parameters
  double elstk,fyieldPos,fyieldNeg,alpha,Resfac; // Properties
  double capSlope,capDispPos,capDispNeg;	 // Cap
  double fpPos,fpNeg,a_pinch;                    // Pinching
  
  // Parameters calculated from input data
  double dyieldPos,dyieldNeg;
  DamageModel *StrDamage;
  DamageModel *StfDamage;
  DamageModel *AccDamage;
  DamageModel *CapDamage;
  
  
  double hsTrial[24], hsCommit[24], hsLastCommit[24];
  
  FILE *OutputFile;		// For debugging
  public:
    PinchingDamage();
    PinchingDamage(int tag);
    PinchingDamage(int tag, Vector inputParam , DamageModel *strength, DamageModel *stiffness, DamageModel *accelerated, DamageModel *capping );
    virtual ~PinchingDamage();
  
  int setTrialStrain(double d, double strainRate = 0.0);
  
  double getStrain(void) const;
  double getStress(void) const;
  double getTangent(void) const;
  double getInitialTangent(void) const;

  int commitState(void);
  int revertToLastCommit(void);    
  int revertToStart(void);  
  
  //virtual
  UniaxialMaterial *getCopy(void) const;
  
  int sendSelf(CommParameters &);  
  int recvSelf(const CommParameters &);    
  
  void Print(std::ostream &s, int flag =0);
  
 protected:
  void envelPosCap(double fy, double alfaPos, double alfaCap,
		   double cpDsp, double d, double *f, double *ek );
  
  void envelNegCap(double fy, double alfaNeg, double alfaCap,
		   double cpDsp, double d, double *f, double *ek);
  
  void recordInfo(int cond =0);
  
  
};
} // end of XC namespace

#endif
