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
//SisRefSccCartesianas2d.cc

#include "SisRefSccCartesianas2d.h"
#include "Spot.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Lee un objeto SisRefSccCartesianas2d desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - pos: Lee las coordenadas del punto.
bool XC::SisRefSccCartesianas2d::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(SisRefSccCartesianas2d) Procesando comando: " << cmd << std::endl;
    if(cmd == "coo_org")
      {
        ref.Org().LeeCmd(status);
        return true;
      }
    else if(cmd == "spot_org")
      {
        const size_t id_spot= interpretaSize_t(status.GetString());
        const Spot *p= BuscaSpot(id_spot);
        if(p)
          ref.Org()= p->GetPos();
        else
	  std::cerr << "SisRefSccCartesianas2d::procesa_comando: '"
                    << cmd << "'; no se encontró el spot: " << id_spot
                    << std::endl;
        return true;
      }
    else if(cmd == "dos_puntos")
      {
        const std::vector<int> indices= crea_vector_int(status.GetString());
        if(indices.size()>1)
          {
            const Spot *p1= BuscaSpot(indices[0]);
            const Spot *p2= BuscaSpot(indices[1]);
            if(p1 && p2)
              ref= Ref2d2d(p1->GetPos(),p2->GetPos());
            else
              {
                std::cerr << "SisRefSccCartesianas2d::procesa_comando: '"
                          << cmd << "'; no se encontró el punto: ";
                if(!p1)
	          std::cerr << indices[0];
                else
	          std::cerr << indices[1];
	        std::cerr << std::endl;
              }
          }
        else
	  std::cerr << "SisRefSccCartesianas2d::procesa_comando: '"
		    << cmd << "' se requieren dos identificadores de punto." << std::endl;
        return true;
      }
    else
      return SisRefScc::procesa_comando(status);
  }

//! @brief Devuelve el vector unitario en la dirección del eje que se
//! pasa como parámetro expresado en el sistema global 
//! para la posición que se pasa como parámetro.
Vector2d XC::SisRefSccCartesianas2d::GetVDirEje(const size_t &i,const Pos2d &) const
  { return ref.GetVDirEje(i); }

//! @brief Devuelve la posicion del punto p expresado en locales
//! expresada en coordenadas globales.
Pos2d XC::SisRefSccCartesianas2d::GetPosGlobal(const Pos2d &p) const
  { return ref.GetPosGlobal(p); }
//! @brief Devuelve el vector v expresado en locales
//! expresado en coordenadas globales.
Vector2d XC::SisRefSccCartesianas2d::GetCooGlobales(const Vector2d &v) const
  { return ref.GetCooGlobales(v); }
//! @brief Devuelve la posicion del punto p expresado en globales
//! expresada en coordenadas locales.
Pos2d XC::SisRefSccCartesianas2d::GetPosLocal(const Pos2d &p) const
  { return ref.GetPosLocal(p); }
//! @brief Devuelve el vector v expresado en globales
//! expresado en coordenadas locales.
Vector2d XC::SisRefSccCartesianas2d::GetCooLocales(const Vector2d &v) const
  { return ref.GetCooLocales(v); }



//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
//!
//! Soporta las propiedades:
//! -nlineas: Devuelve el número de líneas que empiezan o acaban en este punto.
any_const_ptr XC::SisRefSccCartesianas2d::GetProp(const std::string &cod) const
  {
    if(cod == "org")
      return any_const_ptr(ref.Org());
    if(cod == "sis_coo")
      return any_const_ptr(ref.Trf());
    else
      return SisRefScc::GetProp(cod);
  }

