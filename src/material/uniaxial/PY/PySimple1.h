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
**    Module:        PySimple1.h
**
**    Purpose:        Provide a simple p-y spring for OpenSees
**
**
**    Developed by Ross W. Boulanger
**    (C) Copyright 2001, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2001/10/15
// $Source: /OpenSees/SRC/material/uniaxial/PySimple1.h

#ifndef PYSIMPLE1_H
#define PYSIMPLE1_H

// Written: RWB
// Created: Oct 2001
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class definition for PySimple1.
//

#include <material/uniaxial/PY/PQyzBase.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Material que representa una curva p-y.
class PySimple1: public PQyzBase
  {
  protected:
    // Material parameters
    double drag; // ratio of max gap drag force to spring capacity
  private:
    InternalParamsIn CDrag; // Committed internal parameters for the Drag component
    InternalParamsIn TDrag; // Trial internal parameters for the Drag component

    InternalParamsLR CClose; // Committed internal parameters for the Closure component
    InternalParamsLR TClose; // Trial internal parameters for the Closure component

    // Functions to get p & y for each component individually
    void getGap(double ylast, double dy, double dy_old);
    void getClosure(double ylast, double dy);
    void getDrag(double ylast, double dy);
    void getNearField(double ylast, double dy, double dy_old);
    
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);


  public:
    PySimple1(int tag, int classtag, int soilType, double pult, double y50,double drag, double dashpot);
    PySimple1(int tag, int classtag= MAT_TAG_PySimple1);
    PySimple1(void);

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
