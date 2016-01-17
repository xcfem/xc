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
**    Module:        TzSimple1.h 
**
**    Purpose:        Provide a simple t-z spring for OpenSees
** 
**    Developed by Ross W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/1/19
// $Source: /OpenSees/SRC/material/uniaxial/TzSimple1.h

#ifndef TZSIMPLE1_H
#define TZSIMPLE1_H

// Written: RWB
// Created: Jan 2002
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class definition for TzSimple1.
// 

#include <material/uniaxial/PY/PYBase.h>


namespace XC {
//! @ingroup MatUnx
//
//! @brief Material que representa una curva t-z.
class TzSimple1: public PYBase
  {
  private:
    InternalParamsIn CNF; // Committed internal parameters for the NearField plastic component
    InternalParamsIn TNF; // Trial internal parameters for the NearField plastic component

    // Functions to get t & z for elastic & plastic components
    void getNearField(double zlast, double dz, double dz_old);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    
  public:
    TzSimple1(int tag, int classtag, int tzType, double tult, double z50, double dashpot);
    TzSimple1(int tag, int classtag= MAT_TAG_TzSimple1);
    TzSimple1(void);

    int setTrialStrain(double y, double yRate); 
    double getStress(void) const;
    double getDampTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);

   
  };
} // end of XC namespace


#endif
