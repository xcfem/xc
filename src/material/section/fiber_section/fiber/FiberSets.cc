//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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

//! @brief Crea un conjunto de fibras cuyo material tiene el tag being passed as parameter.
XC::FiberSets::iterator XC::FiberSets::sel_mat_tag(DqFibras &fibras, const std::string &nmb_set,const int &matTag)
  {
    iterator i= get_set_fibras(nmb_set);
    fibras.SelMatTag(matTag,(*i).second);
    return i;
  }

//! @brief Crea un conjunto de fibras que perteneciendo al conjunto nmb_set_org, cumplen que el material tiene el tag being passed as parameter.
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


