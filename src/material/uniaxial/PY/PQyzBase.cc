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
**    Module:        PQyzBase.cpp 
**
**    Purpose:        Provide a simple p-y spring for OpenSees.
**
**    Developed by Ross XC::W. Boulanger
**    (C) Copyright 2001, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2001/12/15
// $Source: /OpenSees/SRC/material/uniaxial/PQyzBase.cpp

// Written: RWB
// Created: Dec 2001
// Revision: A
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class implementation for XC::PQyzBase


#include "material/uniaxial/PY/PQyzBase.h"


// Controls on internal iteration between spring components
const int PYmaxIterations = 20;
const double PYtolerance = 1.0e-12;

/////////////////////////////////////////////////////////////////////
//! @brief Constructor with data
XC::PQyzBase::PQyzBase(int tag, int classtag, int soil, double rult, double v50,double dash_pot)
  :PYBase(tag,classtag,soil,rult,v50,dash_pot)
  {}

//! @brief Constructor.
XC::PQyzBase::PQyzBase(int tag,int classtag)
  :PYBase(tag,classtag) {}

/////////////////////////////////////////////////////////////////////
//! @brief Default constructor.
XC::PQyzBase::PQyzBase(void)
  :PYBase(0,0) {}

/////////////////////////////////////////////////////////////////////
int XC::PQyzBase::commitState(void)
  {
    PYBase::commitState();
    CNF= TNF; // Commit trial history variables for Near Field component
    CGap= TGap; // Commit trial history variables for the Gap
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::PQyzBase::revertToLastCommit(void)
  {
    // Reset to committed values
    PYBase::revertToLastCommit();
    TNF= CNF;
    TGap= CGap;
    return 0;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::PQyzBase::sendData(CommParameters &cp)
  {
    int res= PYBase::sendData(cp);
    res+= cp.sendDoubles(NFkrig,Elast,nd,getDbTagData(),CommMetaData(9));
    res+= cp.sendMovable(CNF,getDbTagData(),CommMetaData(10));
    res+= cp.sendMovable(TNF,getDbTagData(),CommMetaData(11));
    res+= cp.sendMovable(CGap,getDbTagData(),CommMetaData(12));
    res+= cp.sendMovable(TGap,getDbTagData(),CommMetaData(13));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::PQyzBase::recvData(const CommParameters &cp)
  {
    int res= PYBase::recvData(cp);
    res+= cp.receiveDoubles(NFkrig,Elast,nd,getDbTagData(),CommMetaData(9));
    res+= cp.receiveMovable(CNF,getDbTagData(),CommMetaData(10));
    res+= cp.receiveMovable(TNF,getDbTagData(),CommMetaData(11));
    res+= cp.receiveMovable(CGap,getDbTagData(),CommMetaData(12));
    res+= cp.receiveMovable(TGap,getDbTagData(),CommMetaData(13));
    return res;
  }

/////////////////////////////////////////////////////////////////////

