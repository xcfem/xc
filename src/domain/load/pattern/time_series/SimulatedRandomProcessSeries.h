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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2003/03/04 00:49:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/SimulatedRandomProcessSeries.h,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu), February 2002
// Revised: 
//

#ifndef SimulatedRandomProcessSeries_h
#define SimulatedRandomProcessSeries_h

#include <domain/load/pattern/TimeSeries.h>
#include "utility/matrix/Vector.h"

namespace XC {
  class Spectrum;
  class RandomNumberGenerator;
  class Vector;

//! @ingroup TSeries
//
//! @brief ??.
class SimulatedRandomProcessSeries : public TimeSeries
  {
  private:
    RandomNumberGenerator *theRandomNumberGenerator;
    Spectrum *theSpectrum;
    int numFreqIntervals;
    double mean;
    double deltaW;
    Vector theta;
    Vector A;
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    SimulatedRandomProcessSeries(RandomNumberGenerator *theRandNumGenerator, Spectrum *theSpectrum, int numFreqIntervals, double mean);

    // method to get load factor
    double getFactor(double pseudoTime) const;

    // None of the following functions should be invoked on this type
    // of object
    double getDuration(void) const {return 0.0;} // dummy function
    double getPeakFactor(void) const {return 0.0;} // dummy function
    double getTimeIncr(double pseudoTime) const {return 1.0;} // dummy function
    
    // methods for output    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
