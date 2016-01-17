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

// $Revision: 1.1 $
// $Date: 2005/12/15 00:35:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TriangleSeries.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/04
// Revision: A
//
// Purpose: This file contains the class implementation of XC::TriangleSeries.
//
// What: "@(#) TriangleSeries.cpp, revA"


#include <domain/load/pattern/time_series/TriangleSeries.h>
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cmath>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"


XC::TriangleSeries::TriangleSeries(double startTime, double finishTime,
    double T, double phi, double theFactor)
  : PeriodSeries(TSERIES_TAG_TriangleSeries,startTime,finishTime,T,phi,theFactor) {}


XC::TriangleSeries::TriangleSeries()
  : PeriodSeries(TSERIES_TAG_TriangleSeries) {}

double XC::TriangleSeries::getFactor(double pseudoTime) const
  {	
    if(tStart <= pseudoTime && pseudoTime <= tFinish)
      {
	const double slope= cFactor/(period/4);
	const double k= (pseudoTime+shift)/period - floor((pseudoTime+shift)/period);
        if(k < 0.25)
	  return slope*k*period;
	else if(k < 0.75)
	  return cFactor-slope*(k-0.25)*period;
	else if(k < 1.00)
	  return -cFactor + slope*(k-0.75)*period;
	else
	  return 0;
      }
    else
      return 0;
  }

int XC::TriangleSeries::sendSelf(CommParameters &cp)
  {
    static ID data(3);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "PeriodSeries::sendSelf() - ch failed to send data\n";
    return result;
  }


int XC::TriangleSeries::recvSelf(const CommParameters &cp)
  {
    static ID data(3);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "PeriodSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;
  }



void XC::TriangleSeries::Print(std::ostream &s, int flag) const
  {
    s << "Triangle Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
    s << "\tPeriod: " << period << std::endl;
    s << "\tPhase Shift: " << shift << std::endl;
  }
