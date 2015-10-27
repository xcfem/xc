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
//SisRef.h
//Objeto sistema de referencia del modelador

#ifndef SISREF_H
#define SISREF_H

#include "preprocessor/EntMdlrBase.h"

class Vector3d;
class Pos3d;

namespace XC {

//! @ingroup Cad
//
//! @defgroup CadSR Sistemas de referencia.
//
//! @ingroup CadSR
//!
//! @brief Sistema de referencia a emplear durante
//! la definición de los objetos geométricos.
class SisRef: public EntMdlrBase
  {
  protected:
    virtual bool procesa_comando(CmdStatus &status);
  public:
    //! @brief Constructor.
    SisRef(Preprocessor *m)
      : EntMdlrBase("",m) {}
    SisRef(const std::string &nombre= "",Preprocessor *m= NULL)
      : EntMdlrBase(nombre,m) {}
    inline virtual ~SisRef(void)
      {}
    //! @brief Devuelve el vector unitario en la dirección del eje que se
    //! pasa como parámetro expresado en el sistema global 
    //! para la posición que se pasa como parámetro.
    virtual Vector3d GetVDirEje(const size_t &,const Pos3d &) const= 0;
    Vector3d GetI(const Pos3d &) const;
    Vector3d GetJ(const Pos3d &) const;
    Vector3d GetK(const Pos3d &) const;

    //! @brief Devuelve la posicion del punto p expresado en locales
    //! expresada en coordenadas globales.
    virtual Pos3d GetPosGlobal(const Pos3d &p) const= 0;
    //! @brief Devuelve el vector v expresado en locales
    //! expresado en coordenadas globales.
    virtual Vector3d GetCooGlobales(const Vector3d &v) const= 0;
    //! @brief Devuelve la posicion del punto p expresado en globales
    //! expresada en coordenadas locales.
    virtual Pos3d GetPosLocal(const Pos3d &p) const= 0;
    //! @brief Devuelve el vector v expresado en globales
    //! expresado en coordenadas locales.
    virtual Vector3d GetCooLocales(const Vector3d &v) const= 0;

    any_const_ptr GetProp(const std::string &cod) const;
  };

} //fin namespace XC

#endif
