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
//DatosDiagInteraccion.cc

#include "DatosDiagInteraccion.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

XC::DatosDiagInteraccion::DatosDiagInteraccion(void)
  : umbral(10), inc_eps(0.0), inc_t(M_PI/4), agot_pivotes(),
    nmb_set_hormigon("hormigon"), tag_hormigon(0),
    nmb_set_armadura("armadura"), tag_armadura(0)
  {
    inc_eps= agot_pivotes.getIncEpsAB(); //Incremento de tensiones.
    if(inc_eps<=1e-6)
      std::cerr << "El incremento para deformaciones es muy pequeño o negativo: " << inc_eps << std::endl; 
  }

XC::DatosDiagInteraccion::DatosDiagInteraccion(const double &u,const double &inc_e,const double &inc_theta,const DefAgotPivotes &agot)
  : umbral(u), inc_eps(inc_e), inc_t(inc_theta), agot_pivotes(agot),
    nmb_set_hormigon("hormigon"), tag_hormigon(0),
    nmb_set_armadura("armadura"), tag_armadura(0) {}

//! @brief Lee un objeto DatosDiagInteraccion desde archivo
bool XC::DatosDiagInteraccion::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string cabecera_error= "(DatosDiagInteraccion) Procesando comando: '"+cmd+"'";
    if(verborrea>2)
      std::clog << cabecera_error << std::endl;

    if(cmd == "umbral")
      {
        umbral= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "inc_eps")
      {
        inc_eps= interpretaDouble(status.GetString());
        if(inc_eps<=0)
          std::cerr << "El incremento para deformaciones el nulo o negativo: " << inc_eps << std::endl; 
        return true;
      }
    else if(cmd == "inc_theta")
      {
        inc_t= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "def_agot_pivotes")
      {
        agot_pivotes.LeeCmd(status);
        return true;
      }
    else if(cmd == "nmb_set_hormigon")
      {
        nmb_set_hormigon= interpretaString(status.GetString());
        return true;
      }
    else if(cmd == "tag_hormigon")
      {
        tag_hormigon= interpretaInt(status.GetString());
        return true;
      }
    else if(cmd == "nmb_set_armadura")
      {
        nmb_set_armadura= interpretaString(status.GetString());
        return true;
      }
    else if(cmd == "tag_armadura")
      {
        tag_armadura= interpretaInt(status.GetString());
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

//! @brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::DatosDiagInteraccion::GetProp(const std::string &cod) const
  {
    if(cod=="getUmbral")
      return any_const_ptr(umbral);
    else if(cod=="getIncEps")
      return any_const_ptr(inc_eps);
    else if(cod=="getIncTheta")
      return any_const_ptr(inc_t);
    else if(cod=="getDefsAgotPivotes")
      return any_const_ptr(&agot_pivotes);
    else
      return EntCmd::GetProp(cod);
  }
