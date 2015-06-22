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
//DiscretBase.cpp

#include "material/section/repres/DiscretBase.h" 
#include "material/Material.h" 
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

XC::DiscretBase::DiscretBase(Material *m)
  : mat(m) {}

//! @brief Lee un objeto XC::DiscretBase desde archivo
bool XC::DiscretBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(DiscretBase) Procesando comando: " << cmd << std::endl;

    if(cmd == "material")
      {
        if(mat)
          mat->LeeCmd(status);
        else
	  std::cerr << "(DiscretBase) Procesando comando: " 
                    << cmd << " no se ha asignado un material. "
                    << std::endl;
        return true;
      }
    else
      return EntConNmb::procesa_comando(status);
  }

void XC::DiscretBase::setMaterialPtr(Material *m)
  { mat = m; }

XC::Material *XC::DiscretBase::getMaterialPtr(void) const
  { return mat; }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::DiscretBase::GetProp(const std::string &cod) const
  {
    if(cod=="ymin")
      {
        tmp_gp_dbl= getMinY();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="zmin")
      {
        tmp_gp_dbl= getMinZ();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="ymax")
      {
        tmp_gp_dbl= getMaxY();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="zmax")
      {
        tmp_gp_dbl= getMaxZ();
        return any_const_ptr(tmp_gp_dbl);
      }
    return EntConNmb::GetProp(cod);
  }
