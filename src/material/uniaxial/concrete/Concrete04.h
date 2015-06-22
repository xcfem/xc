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
// $Date: 2005/09/23 22:51:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete04.h,v $
                                                                        
                                                                        
#ifndef Concrete04_h
#define Concrete04_h

// File: Concrete04.h
//
// Written: N.Mitra (nmitra@u.washington.edu)
// Created: 09/04
// Revision: A
//
// Description: This file contains the class definition for 
// Concrete04.h 
//   - No tension 
//   - Linear unloading/reloading
//
// What: "@(#) Concrete04.h, revA"
// Revision: 1. Adding in Exponential tension part (05-16-05)

#include "material/uniaxial/concrete/ConcreteBase.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Modelo unidimensional de tipo hormigón.
class Concrete04: public ConcreteBase
  {
   private:
      /*** Material Properties ***/
    double fpc;    // Compressive strength
    double epsc0;  // Strain at compressive strength
    double epscu;  // Strain at crushing strength
    double Ec0;    // initial tangent
    double fct;   // Concrete tensile strength
    double etu;   // ultimate tensile strain              
    double beta;  // exponential curve parameter, residual stress (as a factor of ft)
	                // at etu. 

      /*** CONVERGED History Variables ***/
    double CmaxStrain;  
    double CcompStrain;   // strain value at which the compression unloading intersects the   
                          // zero stress value or the strain value at which tensile reloading starts.                                  
    double CUtenStress;   // tensile stress value at which unloading begins
    double CUtenSlope;    // unloading tensile slope value

      /*** TRIAL History Variables ***/
    double TmaxStrain;
    double TcompStrain;
    double TUtenStress;
    double TUtenSlope;

    void CompReload(void);
    void CompEnvelope(void);
    void setCompUnloadEnv(void);
    void TensReload(void);
    void TensEnvelope(void);
    void setTenUnload(void);
    void setup_parameters(void);
  public:
    Concrete04(int tag, double fpc, double eco, double ecu, double Ec0, double fct, double etu);  
    Concrete04(int tag, double fpc, double eco, double ecu, double Ec0, double fct, double etu, double beta);
    Concrete04(int tag, double fpc, double eco, double ecu, double Ec0);
    Concrete04(int tag= 0);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    inline double getInitialTangent(void) const
      { return Ec0; }

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);

    // LOWES: add function for use with variable hinge lnegth model
    int getMaterialType(void);
    // LOWES: end
  };
} // fin namespace XC

#endif
