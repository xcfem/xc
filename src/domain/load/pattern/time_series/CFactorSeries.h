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
// CFactorSeries.h

#ifndef CFactorSeries_h
#define CFactorSeries_h

#include <domain/load/pattern/TimeSeries.h>

namespace XC {
//! @ingroup TSeries
//
//! @brief Función del tiempo que tiene un factor que multiplica
//! o se suma al valor devuelto (clase abstracta).
class CFactorSeries: public TimeSeries
  {
  protected:
    double cFactor;  // factor = cFactor

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    CFactorSeries(int classTag,double cFactor = 1.0);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    double getFactor(double pseudoTime) const {return cFactor;}
    void setFactor(const double &d)
      { cFactor= d; }

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif
