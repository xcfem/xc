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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ViscousMaterial.h,v $
                                                                        
// Written: Mehrdad Sasani 
// Created: June 2000
// Revision: A
//
// Description: This file contains the class interface for 
// ViscousMaterial.  ViscousMaterial defines a force(F)-velocity(v)
// relationship of the form F = C*pow(v,a), where C is a prescribed
// constant and a is a real number.

#ifndef ViscousMaterial_h
#define ViscousMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Material de tipo viscoso.
class ViscousMaterial : public UniaxialMaterial
  {
  private:
    double trialRate;
    double C;
    double Alpha;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ViscousMaterial(int tag= 0); 
    ViscousMaterial(int tag, double C, double Alpha);    

    int setTrialStrain(double velocity, double strainRate = 0.0); 
    double getStrain(void) const; 
    double getStrainRate(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getInitialTangent(void) const;
    double getDampTangent(void) const;


    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
  };
} // fin namespace XC


#endif

