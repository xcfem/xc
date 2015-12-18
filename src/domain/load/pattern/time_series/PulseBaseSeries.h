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
//PulseBaseSeries.h
                                                                        
#ifndef PulseBaseSeries_h
#define PulseBaseSeries_h

#include "CFactorSeries.h"

namespace XC {
//! @ingroup TSeries
//
//! @brief Clase base para las funciones del tiempo de tipo pulso.
class PulseBaseSeries: public CFactorSeries
  {
  protected:
    double tStart;    // start time of time series (sec)
    double tFinish;   // end time of time series (sec)

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors
    PulseBaseSeries(int classTag,const double &tS=0.0,const double &tF= 0.0,const double &factor=1.0);
    
    inline double getDuration(void) const {return tFinish-tStart;}
    inline double getPeakFactor(void) const {return cFactor;}
    inline double getTimeIncr(double pseudoTime) const {return tFinish-tStart;}

    inline double getStart(void) const
      { return tStart; }
    inline void setStart(const double &d)
      { tStart= d; }
    inline double getFinish(void) const
      { return tFinish; }
    inline void setFinish(const double &d)
      { tFinish= d; }
    
    // methods for output    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;    
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif
