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
// $Date: 2005/12/15 00:35:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PeriodSeries.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/04
// Revision: A
//
// Purpose: This file contains the class implementation of XC::PeriodSeries.
//
// What: "@(#) PeriodSeries.cpp, revA"


#include "PeriodSeries.h"
#include <utility/matrix/Vector.h>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"



//! @brief Constructor.
XC::PeriodSeries::PeriodSeries(int classTag,double startTime, double finishTime,
    double T, double phi, double theFactor)
  : PulseBaseSeries(classTag,startTime,finishTime,theFactor), period(T),shift(phi)
  {
    if(period == 0.0)
      {
        std::cerr << "PeriodSeries::PeriodSeries -- input period is zero, setting period to 1\n";
        period = 1;
      }
  }

//! @brief Constructor por defecto.
XC::PeriodSeries::PeriodSeries(int classTag)
  : PulseBaseSeries(classTag,1.0), period(1.0),shift(0.0) {}

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PeriodSeries::sendData(CommParameters &cp)
  {
    int res= PulseBaseSeries::sendData(cp);
    res+= cp.sendDoubles(period,shift,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PeriodSeries::recvData(const CommParameters &cp)
  {
    int res= PulseBaseSeries::recvData(cp);
    res+= cp.receiveDoubles(period,shift,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Imprime el objeto.
void XC::PeriodSeries::Print(std::ostream &s, int flag) const
  {
    s << "Period Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
    s << "\tPeriod: " << period << std::endl;
    s << "\tPhase Shift: " << shift << std::endl;
  }
