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
**    Module:	TzLiq1.h 
**
**    Purpose:	Provide a t-z material that gets pore pressure from a
**				specified element that contains a PorousFluidSolid.
**              
**
**    Developed by Ross W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/2/5
// $Source: /OpenSees/SRC/material/uniaxial/TzLiq1.h

#ifndef TZLIQ1_H
#define TZLIQ1_H

// Written: RWB
// Created: Feb 2002
//
// Description: This file contains the class definition for TzLiq1.
// 

#include <material/uniaxial/PY/TzSimple1.h>

namespace XC {
class FourNodeQuad;

//! @ingroup MatUnx
//
//! @brief ??.
class TzLiq1 : public TzSimple1
  {
  private:
    // Committed and trial values for t, z, and ru
    double Tz;
    double Cz;
    double Tt;
    double Ct;
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
    double ru;

    static int loadStage;
    int    lastLoadStage;
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
    TzLiq1(int tag, int classtag, int tzType, double tult, double z50,
    	      double dashpot, int solidElem1, int solidElem2, Domain *theDomain);
    TzLiq1(int tag, int classtag= MAT_TAG_TzLiq1);
    TzLiq1(void);

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
} // end of XC namespace

#endif // TZLIQ1_H
