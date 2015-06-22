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
//FiberSets.cc

#include "FiberSets.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

//! @brief Constructor.
XC::FiberSets::FiberSets(void)
  : std::map<std::string,DqFibras>()
  {}

//! @brief Crea un nuevo conjunto de fibras.
XC::DqFibras &XC::FiberSets::crea_set_fibras(const std::string &nmb)
  {
    if(find(nmb) == end()) //Set doesn't exists
      (*this)[nmb]= DqFibras();
    else
      std::cerr << "Fiber set: '" << nmb
                << "' already exists. Command ignored." << std::endl;
    return (*this)[nmb];
  }

//! @brief Crea un conjunto de fibras cuyo nombre se pasa como parámetro.
XC::FiberSets::iterator XC::FiberSets::get_set_fibras(const std::string &nmb_set)
  {
    iterator i= find(nmb_set);
    if(i == end())
      {
        if(verborrea>1)
          std::clog << "Fiber set: '" << nmb_set
                    << "' doesn't exists; it is created." << std::endl;
        crea_set_fibras(nmb_set);
        i= find(nmb_set);
      }
    return i;
  }

//! @brief Crea un conjunto de fibras cuyo material tiene el tag que se pasa como parámetro.
XC::FiberSets::iterator XC::FiberSets::sel_mat_tag(DqFibras &fibras, const std::string &nmb_set,const int &matTag)
  {
    iterator i= get_set_fibras(nmb_set);
    fibras.SelMatTag(matTag,(*i).second);
    return i;
  }

//! @brief Crea un conjunto de fibras que perteneciendo al conjunto nmb_set_org, cumplen que el material tiene el tag que se pasa como parámetro.
XC::FiberSets::iterator XC::FiberSets::resel_mat_tag(const std::string &nmb_set,const std::string &nmb_set_org,const int &matTag)
  {
    iterator i= end();
    if(nmb_set != nmb_set_org)
      {
        i= get_set_fibras(nmb_set);
        iterator j= find(nmb_set_org);
        if(j == end())
          {
            std::clog << "Origin fiber set: '" << nmb_set_org
                      << "' doesn't exists; command ignored." << std::endl;
          }
        else
          (*j).second.SelMatTag(matTag,(*i).second);
      }
    return i;
  }


