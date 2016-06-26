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
//NMPointCloudBase.h
//Internal forces point cloud container base

#ifndef NMPOINTCLOUDBASE_H
#define NMPOINTCLOUDBASE_H

namespace XC {


//! @ingroup MATSCCDiagInt
//
//! @brief (N,M) point set base
class NMPointCloudBase
  {
  protected:
    double umbral;
  public:
    NMPointCloudBase(const double &u= 0.0);
    inline void setUmbral(const double &u)
      { umbral= u; }
    inline const double &getUmbral(void)
      { return umbral; }
    inline virtual void clear(void)
      { umbral= 0.0; }
  };

} // end of XC namespace

#endif
