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
//BarraSuelta.cpp

#include <cmath>
#include <utility/matrix/Matrix.h>

#include <material/section/repres/geom_section/reinfLayer/BarraSuelta.h>
#include <material/section/repres/geom_section/reinfBar/ReinfBar.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_basic/src/texto/cadena_carac.h"


XC::BarraSuelta::BarraSuelta(ListReinfLayer *owr,Material *mat)
  : ReinfLayer(owr,mat,0,0.0,0.0),
    posit(2) {}

XC::BarraSuelta::BarraSuelta(ListReinfLayer *owr,Material *mat, double reinfBarArea,const Vector &position)
  : ReinfLayer(owr,mat,1,0.0,reinfBarArea),posit(position) {}

XC::BarraSuelta::BarraSuelta(const ReinfBar &bar)
  : ReinfLayer(nullptr,bar.getMaterialPtr(),1,bar.getDiameter(),bar.getArea()),posit(getPosition()) 
  { set_owner(const_cast<EntCmd *>(bar.Owner())); }

//! @brief Lee un objeto XC::BarraSuelta desde archivo
bool XC::BarraSuelta::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(BarraSuelta) Procesando comando: " << cmd << std::endl;
    if(cmd == "y")
      {
        posit(0)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "z")
      {
        posit(1)= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "pos")
      {
	std::deque<std::string> str_coord= separa_cadena(status.GetString(),",");
        if(str_coord.size()<2)
	  std::cerr << "Error; " << cmd 
		    << " se requieren dos coordenadas." << std::endl;
        else
          {
            posit(0)= interpretaDouble(str_coord[0]);
            posit(1)= interpretaDouble(str_coord[1]);
	  }
        return true;
      }
    else
      return ReinfLayer::procesa_comando(status);
  }


void XC::BarraSuelta::setPosition(const Vector &Position)
  { posit= Position; }

double XC::BarraSuelta::getMaxY(void) const
  { return posit(0); }
double XC::BarraSuelta::getMaxZ(void) const
  { return posit(1); }
double XC::BarraSuelta::getMinY(void) const
  { return posit(0); }
double XC::BarraSuelta::getMinZ(void) const
  { return posit(1); }

//! @brief Devuelve un array de barras de refuerzo.
const XC::VectorReinfBar &XC::BarraSuelta::getReinfBars(void) const
  {
    reinfBars.resize(1);
    reinfBars.put(1,ReinfBar(getReinfBarArea(),getMaterialPtr(),posit));
    return reinfBars;         
  }

const XC::Vector &XC::BarraSuelta::getPosition(void) const
  { return posit; }

XC::ReinfLayer *XC::BarraSuelta::getCopy (void) const
  { return new BarraSuelta(*this); }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::BarraSuelta::GetProp(const std::string &cod) const
  {
    if(cod=="y")
      return any_const_ptr(posit(0));
    if(cod=="z")
      return any_const_ptr(posit(1));
    else
      return ReinfLayer::GetProp(cod);
  }

void XC::BarraSuelta::Print(std::ostream &s, int flag) const
  {
     s << "\nReinforcing Layer type:  Suelta";
     ReinfLayer::Print(s,flag);
     s << "\n Position: " << posit;
  }

