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
// PeriodSeries.h
                                                                        
#ifndef PeriodSeries_h
#define PeriodSeries_h

#include "PulseBaseSeries.h"

namespace XC {
//! @ingroup TSeries
//
//! @brief Función del tiempo de tipo pulso.
class PeriodSeries: public PulseBaseSeries
  {
  protected:
    double period; //!< period of pulse series (sec)
    double shift;  //!< phase shift of pulse series (sec)

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    // constructors
    PeriodSeries(int classTag,double tStart,double tFinish,double period,double shift,double cFactor = 1.0);
    PeriodSeries(int classTag);

    inline double getPeriod(void) const
      { return period; }
    inline void setPeriod(const double &d)
      { period= d; }
    inline double getShift(void) const
      { return shift; }
    inline void setShift(const double &d)
      { shift= d; }
    
    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif
