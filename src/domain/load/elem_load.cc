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
//elem_load.cc

#include "elem_load.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "boost/any.hpp"
#include "utility/matrix/Vector.h"

//! @brief Crea una nueva carga sobre elemento
XC::ElementalLoad *XC::procesa_element_load(XC::LoadPattern *lp,int &tag_el,const std::string &cmd)
  {
    ElementalLoad *retval= nullptr;
    if(cmd == "beam2d_uniform_load")
      retval= new_elem_load<Beam2dUniformLoad>(lp,tag_el);
    else if(cmd == "beam2d_point_load")
      retval= new_elem_load<Beam2dPointLoad>(lp,tag_el);
    else if(cmd == "beam_strain_load")
      retval= new_elem_load<BeamStrainLoad>(lp,tag_el);
    else if(cmd == "beam3d_point_load")
      retval= new_elem_load<Beam3dPointLoad>(lp,tag_el);
    else if(cmd == "beam3d_uniform_load")
      retval= new_elem_load<Beam3dUniformLoad>(lp,tag_el);
    else if(cmd == "brick_self_weight")
      retval= new_elem_load<BrickSelfWeight>(lp,tag_el);
    else if(cmd == "shell_uniform_load")
      retval= new_elem_load<ShellUniformLoad>(lp,tag_el);
    else if(cmd == "bidim_strain_load")
      retval= new_elem_load<BidimStrainLoad>(lp,tag_el);
    else if(cmd == "shell_strain_load")
      retval= new_elem_load<ShellStrainLoad>(lp,tag_el);
    else if(cmd == "truss_temp_load")
      retval= new_elem_load<TrussStrainLoad>(lp,tag_el);
    return retval;
  }


bool XC::procesa_comando_element_load(XC::LoadPattern *lp,int &tag_el,const std::string &cmd,CmdStatus &status)
  {
    if(cmd == "beam2d_uniform_load")
      {
        procesa_elem_load<Beam2dUniformLoad>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "beam2d_point_load")
      {
        procesa_elem_load<Beam2dPointLoad>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "beam_strain_load")
      {
        procesa_elem_load<BeamStrainLoad>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "beam3d_point_load")
      {
        procesa_elem_load<Beam3dPointLoad>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "beam3d_uniform_load")
      {
        procesa_elem_load<Beam3dUniformLoad>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "brick_self_weight")
      {
        procesa_elem_load<BrickSelfWeight>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "shell_uniform_load")
      {
        procesa_elem_load<ShellUniformLoad>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "bidim_strain_load")
      {
        procesa_elem_load<BidimStrainLoad>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "shell_strain_load")
      {
        procesa_elem_load<ShellStrainLoad>(lp,tag_el,cmd,status);
        return true;
      }
    else if(cmd == "truss_temp_load")
      {
        procesa_elem_load<TrussStrainLoad>(lp,tag_el,cmd,status);
        return true;
      }
    return false;
  }

//! @brief OBSOLETA Procesa los comandos correspondientes a cargas sobre elemento
bool XC::procesa_comando_element_edge_load(XC::LoadPattern *lp,int &tag_el,const std::string &cmd,CmdStatus &status)
  {
    if(cmd == "edge_load_3d")
      {
        procesa_elem_edge_load<ElementEdge3dUniformLoad>(lp,tag_el,cmd,status);
        return true;
      }
    return false;
  }
