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
/* *********************************************************************
**    Module:	PyLiq1.h 
**
**    Purpose:	Provide a p-y material that gets pore pressure from a
**				specified element that contains a PorousFluidSolid.
**              
**
**    Developed by Ross W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/5/15
// $Source: /OpenSees/SRC/material/uniaxial/PyLiq1.h

#ifndef PYLIQ1_H
#define PYLIQ1_H

// Written: RWB
// Created: May 2002
//
// Description: This file contains the class definition for PyLiq1.
// 

#include <material/uniaxial/PY/PySimple1.h>
#include <iostream>
#include <string>

namespace XC{
  class FourNodeQuad;

//! @ingroup MatUnx
//
//! @brief ??.
class PyLiq1 : public PySimple1
  {
  private:
    // Residual p (other parameters in PySimple1 base class)
    double pRes;

    // Committed and trial values for p, y, and ru
    double Ty;
    double Cy;
    double Tp;
    double Cp;
    double Tangent;
    double maxTangent;
    double Tru;
    double Cru;
    double Hru;

    // Solid element from which pore pressures are obtained, domain pointer
    // and stage information to get the initial vertical effective stress.
    int solidElem1;
    int solidElem2;
    double meanConsolStress;
    static int loadStage;
    int lastLoadStage;
    std::string elemFlag;
    Domain *theDomain;
    FourNodeQuad *theQuad1;
    FourNodeQuad *theQuad2;

    // Initial tangent
    double initialTangent;
     
    // Function for obtaining effective stresses from adjoining solid soil elements
    double getEffectiveStress(void);
    static Vector stressV3;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    PyLiq1(int tag, int classtag, int soilType, double pult, double y50, double drag,
           double dashpot, double pRes, int solidElem1, int solidElem2, Domain *theDomain);
    PyLiq1(int tag, int classtag= MAT_TAG_PyLiq1);
    PyLiq1(void);

    int setTrialStrain(double y, double yRate); 
    double getStrain(void) const;          
    double getStress(void) const;
    double getTangent(void) const;
    double getStrainRate(void) const;
    double getDampTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    //  Command for initiating vertConsolStress from TclUpdateMaterialStageCommand
    int updateParameter(int snum, Information &eleInformation);
    void Print(std::ostream &s, int flag =0);
  };
}// fin namespace
#endif // PYLIQ1_H
