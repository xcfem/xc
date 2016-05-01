//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MEDFieldInfo

#include "MEDFieldInfo.h"
#include "MEDGaussModel.h"

#include "xc_basic/src/med_xc/MEDMEM_Group.hxx"
#include "xc_basic/src/med_xc/MEDMEM_Field.hxx"

#include "domain/mesh/element/gauss_models/GaussModel.h"

//! @brief Constructor.
XC::MEDFieldInfo::MEDFieldInfo(const FieldInfo &fi,MEDGroupInfo *grp)
  : field_info(fi) 
  { set_owner(grp); }

//! @brief Devuelve el grupo sobre el que está definido el campo.
const XC::MEDGroupInfo &XC::MEDFieldInfo::getGrupo(void) const
  { 
    const MEDGroupInfo *retval= dynamic_cast<const MEDGroupInfo *>(Owner());
    return *retval;
  }

MEDMEM::GROUP *XC::MEDFieldInfo::getGrupoMED(void) const
  { return getGrupo().getGrupoMED(); }
