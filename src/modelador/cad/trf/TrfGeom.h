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
//TrfGeom.h
//Transformaciones geométricas susceptibles de aplicarse
//sobre un conjunto de puntos.

#ifndef TRFGEOM_H
#define TRFGEOM_H

#include "modelador/EntMdlrBase.h"

class Vector3d;
class Pos3d;
class MatrizPos3d;
class TritrizPos3d;

namespace XC {


//! @ingroup Cad
//
//! @defgroup CadTrf Transformaciónes geométricas.
//
//!  \ingroup CadTrf
//! 
//!  Transformaciones geométricas susceptibles de aplicarse
//!  sobre objetos del cad.
class TrfGeom: public EntMdlrBase
  {
  protected:
    virtual bool procesa_comando(CmdStatus &status);
  public:
    //! @brief Constructor.
    TrfGeom(Modelador *m)
      : EntMdlrBase("",m) {}
    inline virtual ~TrfGeom(void)
      {}
    //! @brief Aplica la transformación al punto.
    virtual Pos3d Transforma(const Pos3d &p) const= 0;
    virtual Vector3d Transforma(const Vector3d &v) const= 0;
    const MatrizPos3d &Transforma(const MatrizPos3d &m) const;
    const TritrizPos3d &Transforma(const TritrizPos3d &m) const;
  };
} //fin namespace XC

#endif
