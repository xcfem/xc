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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/TrigSeries.h,v $
                                                                        
                                                                        
#ifndef TrigSeries_h
#define TrigSeries_h

// File: ~/domain/load/pattern/TrigSeries.h
//
// Written: MHS 
// Created: July 2000
// Revision: A
//
// Purpose: This file contains the class definition for TrigSeries.
// TrigSeries is a concrete class. A TrigSeries object provides
// a sine time series. the factor is given by the pseudoTime (x),
// pulse period (T), phase shift (phi), i.e. by sin(2*PI*(x-xo)/T + phi),
// and a constant factor provided in the constructor,
// the duration by tStart and tFinal;
//
// What: "@(#) TrigSeries.h, revA"

#include "PeriodSeries.h"

namespace XC {
//! @ingroup TSeries
//
//! @brief Variación en el tiempo según una función trigonométrica.
class TrigSeries: public PeriodSeries
  {
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors
    TrigSeries(double tStart, double tFinish,
		double period, double shift, double cFactor = 1.0);
    TrigSeries(void);
    
    TimeSeries *getCopy(void) const
      { return new TrigSeries(*this); }
    
    // method to get load factor
    double getFactor(double pseudoTime) const;
    
    // methods for output    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;        
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif
