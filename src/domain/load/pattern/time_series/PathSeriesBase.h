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
//PathSeriesBase.h

#ifndef PathSeriesBase_h
#define PathSeriesBase_h

#include <domain/load/pattern/time_series/CFactorSeries.h>
#include "utility/matrix/Vector.h"

namespace XC {
class Vector;

//! @ingroup TSeries
//
//! @brief Función del tiempo expresada mediante una seríe de puntos (ti,fi).
class PathSeriesBase: public CFactorSeries
  {
  protected:
    Vector thePath; //!< vector containg the data points
    int lastSendCommitTag;

    static size_t load_vector_from_file(Vector &v,std::istream &is);

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors
    PathSeriesBase(int classTag,const double &cfactor= 1.0);
    PathSeriesBase(int classTag,const Vector &thePath,const double &cfactor= 1.0);

    size_t getNumDataPoints(void) const;
    size_t getNumDataPointsOnFile(const std::string &) const;
    double getPeakFactor(void) const;

    inline Vector getPath(void) const
      { return thePath; }
    inline void setPath(const Vector &d)
      { thePath= d; }
    inline int getLastSendCommitTag(void) const
      { return lastSendCommitTag; }
    inline void setLastSendCommitTag(const int &d)
      { lastSendCommitTag= d; }

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif

