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
//SisRefCartesianas3d.h
//Sistema de coordenadas cartesianas.

#ifndef SISREFCARTESIANAS3D_H
#define SISREFCARTESIANAS3D_H

#include "SisRef.h"
#include "xc_utils/src/geom/sis_ref/Ref3d3d.h"

namespace XC {

//! @ingroup CadSR
//!
//! @brief Sistema de coordenadas cartesianas tridimensional.
class SisRefCartesianas3d: public SisRef
  {
    Ref3d3d ref;//!< Sistema de referencia.
  protected:
    virtual bool procesa_comando(CmdStatus &status);
  public:
    //! @brief Constructor.
    SisRefCartesianas3d(Preprocessor *m)
      : SisRef(m) {}
    SisRefCartesianas3d(const std::string &nombre= "",Preprocessor *m= NULL)
      : SisRef(nombre,m) {}

    Vector3d GetVDirEje(const size_t &,const Pos3d &) const;

    Pos3d getOrg(void) const;
    void setOrg(const Pos3d &);
    void TresPuntos(const Pos3d &,const Pos3d &, const Pos3d &);

    Pos3d GetPosGlobal(const Pos3d &p) const;
    Vector3d GetCooGlobales(const Vector3d &v) const;
    Pos3d GetPosLocal(const Pos3d &p) const;
    Vector3d GetCooLocales(const Vector3d &v) const;

    any_const_ptr GetProp(const std::string &cod) const;
  };

} //fin namespace XC

#endif
