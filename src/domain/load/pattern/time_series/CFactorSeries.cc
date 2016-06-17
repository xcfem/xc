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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/CFactorSeries.cpp,v $

// File: ~/domain/load/pattern/CFactorSeries.C
//
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for XC::CFactorSeries.
// CFactorSeries is a concrete class. A XC::CFactorSeries object provides
// a linear time series. the factor is given by the pseudoTime and 
// a constant factor provided in the constructor.
//
// What: "@(#) CFactorSeries.C, revA"


#include "CFactorSeries.h"

#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::CFactorSeries::CFactorSeries(int classTag,double theFactor)
  :TimeSeries(classTag), cFactor(theFactor) {}

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::CFactorSeries::getDbTagData(void) const
  {
    static DbTagData retval(1);
    return retval;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::CFactorSeries::sendData(CommParameters &cp)
  { return cp.sendDouble(cFactor,getDbTagData(),CommMetaData(0)); }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::CFactorSeries::recvData(const CommParameters &cp)
  { return cp.receiveDouble(cFactor,getDbTagData(),CommMetaData(0)); }

//! @brief Sends object through the channel being passed as parameter.
int XC::CFactorSeries::sendSelf(CommParameters &cp)
  {
    inicComm(1);
    int result= sendData(cp);

    const int dataTag= getDbTag(cp);
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "CFactorSeries::sendSelf() - ch failed to send data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::CFactorSeries::recvSelf(const CommParameters &cp)
  {
    inicComm(1);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "CFactorSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }
