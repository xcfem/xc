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
//SisRefSccCartesianas2d.h
//Sistema de coordenadas cartesianas.

#ifndef SISREFSCCCARTESIANAS2D_H
#define SISREFSCCCARTESIANAS2D_H

#include "SisRefScc.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"

namespace XC {

//! @ingroup MATSCCEntGeom
//
//! @brief Sistema de referencia bidimensional para una sección.
class SisRefSccCartesianas2d: public SisRefScc
  {
    Ref2d2d ref;//!< Sistema de referencia.
  protected:

  public:
    //! @brief Constructor.
    SisRefSccCartesianas2d(GeomSection *m)
      : SisRefScc(m) {}
    SisRefSccCartesianas2d(const std::string &nombre= "",GeomSection *m= NULL)
      : SisRefScc(nombre,m) {}

    Vector2d GetVDirEje(const size_t &,const Pos2d &) const;


    Pos2d GetPosGlobal(const Pos2d &p) const;
    Vector2d GetCooGlobales(const Vector2d &v) const;
    Pos2d GetPosLocal(const Pos2d &p) const;
    Vector2d GetCooLocales(const Vector2d &v) const;


  };

} //end of XC namespace

#endif
