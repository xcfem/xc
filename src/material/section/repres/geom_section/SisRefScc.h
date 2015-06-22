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
//SisRefScc.h
//Objeto sistema de referencia del GeomSection

#ifndef SISREFSCC_H
#define SISREFSCC_H

#include "EntGeomSection.h"

class Vector2d;
class Pos2d;

namespace XC {

//! @ingroup MATSCCEntGeom
//
//! @brief Sistema de referencia empleado para definir
//! las entidades geométricas.
class SisRefScc: public EntGeomSection
  {
  protected:
    virtual bool procesa_comando(CmdStatus &status);
  public:
    //! @brief Constructor.
    SisRefScc(GeomSection *m)
      : EntGeomSection("",m) {}
    SisRefScc(const std::string &nombre= "",GeomSection *m= NULL)
      : EntGeomSection(nombre,m) {}

    //! @brief Devuelve el vector unitario en la dirección del eje que se
    //! pasa como parámetro expresado en el sistema global 
    //! para la posición que se pasa como parámetro.
    virtual Vector2d GetVDirEje(const size_t &,const Pos2d &) const= 0;
    Vector2d GetI(const Pos2d &) const;
    Vector2d GetJ(const Pos2d &) const;

    //! @brief Devuelve la posicion del punto p expresado en locales
    //! expresada en coordenadas globales.
    virtual Pos2d GetPosGlobal(const Pos2d &p) const= 0;
    //! @brief Devuelve el vector v expresado en locales
    //! expresado en coordenadas globales.
    virtual Vector2d GetCooGlobales(const Vector2d &v) const= 0;
    //! @brief Devuelve la posicion del punto p expresado en globales
    //! expresada en coordenadas locales.
    virtual Pos2d GetPosLocal(const Pos2d &p) const= 0;
    //! @brief Devuelve el vector v expresado en globales
    //! expresado en coordenadas locales.
    virtual Vector2d GetCooLocales(const Vector2d &v) const= 0;

    any_const_ptr GetProp(const std::string &cod) const;
  };

} //fin namespace XC

#endif
