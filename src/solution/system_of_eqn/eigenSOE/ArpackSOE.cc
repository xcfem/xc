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
// File: ~/system_of_eqn/eigenSOE/ArpackSOE.C
//
// Written: Jun Peng 
// Created: Febuary 1999
// Revision: A
//
// Description: This file contains the class definition for XC::ArpackSOE
// ArpackSOE is a subclass of XC::EigenSOE. It uses the LAPACK storage
// scheme to store the components of the K, M matrix, which is a full matrix.
// It uses the ARPACK to do eigenvalue analysis.

#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::ArpackSOE::ArpackSOE(SoluMethod *owr,int classTag, double theShift)
  :EigenSOE(owr,classTag), shift(theShift) {}


//! @brief Lee un objeto XC::ArpackSOE desde archivo
bool XC::ArpackSOE::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ArpackSOE) Procesando comando: " << cmd << std::endl;
    if(cmd == "shift")
      {
        shift= interpretaDouble(status.GetString());
        return true;
      }
    else
      return EigenSOE::procesa_comando(status);
  }

//! @brief Devuelve el valor del parámetro shift.
const double &XC::ArpackSOE::getShift(void) const
  { return shift; }

void XC::ArpackSOE::setShift(const double &s)
  { shift= s; }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::ArpackSOE::GetProp(const std::string &cod) const
  {
    if(cod=="shift")
      return any_const_ptr(getShift());
    else
      return EigenSOE::GetProp(cod);
  }
