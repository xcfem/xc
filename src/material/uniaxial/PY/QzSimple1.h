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
**    Module:	QzSimple1.h 
**
**    Purpose:	Provide a simple Q-z material for OpenSees
**
**
**    Developed by Ross W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/1/22
// $Source: /OpenSees/SRC/material/uniaxial/QzSimple1.h

#ifndef QZSIMPLE1_H
#define QZSIMPLE1_H

// Written: RWB
// Created: Jan 2002
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class definition for QzSimple1.
// 

#include "material/uniaxial/PY/PQyzBase.h"


namespace XC {
//! @ingroup MatUnx
//
//! @brief Material que representa la resistencia por fuste de un pilote.
class QzSimple1 : public PQyzBase
  {
  private:
    // Material parameters
    double suction; // ratio of max suction force to Qult
    double maxElast; // max size of elastic range (in terms of dQ/Qult)

    InternalParamsIn CSuction; // Committed internal parameters for the Suction component
    InternalParamsIn TSuction; // Trial internal parameters for the Suction component

    InternalParamsA CClose; //!< Committed internal parameters for the Closure component
    InternalParamsA TClose; //!< Trial internal parameters for the Closure component

    // Functions to get Q & z for each component individually
    void getGap(double zlast, double dz, double dz_old);
    void getClosure(double zlast, double dz);
    void getSuction(double zlast, double zy);
    void getNearField(double zlast, double dz, double dz_old);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    QzSimple1(int tag, int qzType, double Qult, double z50, double suction,double dashpot);
    QzSimple1(int tag, int classtag= MAT_TAG_QzSimple1);
    QzSimple1(void);

    int setTrialStrain(double z, double zRate); 
    double getStress(void) const;
    double getDampTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
    virtual any_const_ptr GetProp(const std::string &cod) const;
   
    

  };
} // end of XC namespace

#endif
