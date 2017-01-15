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
//BaseElasticSection2d.cpp

#include <material/section/elastic_section/BaseElasticSection2d.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>



XC::BaseElasticSection2d::BaseElasticSection2d(int tag, int classTag, const size_t &dim, MaterialLoader *mat_ldr)
  : BaseElasticSection(tag, classTag,dim,mat_ldr), ctes_scc() {}

XC::BaseElasticSection2d::BaseElasticSection2d(int classTag,const size_t &dim)
  : BaseElasticSection(0, classTag,dim), ctes_scc() {}

XC::BaseElasticSection2d::BaseElasticSection2d(int tag, int classTag, const size_t &dim, const double &E, const double &A, const double &I,double G, const double &alpha)
  : BaseElasticSection(tag, classTag,dim ), ctes_scc(E,A,I,G,alpha) {}

void XC::BaseElasticSection2d::sectionGeometry(const std::string &cod_geom)
  {
    const MaterialLoader *ldr= getMaterialLoader();
    if(ldr)
      {
        const GeomSection *geom= ldr->find_ptr_geom_section(cod_geom);
        if(geom)
          {
            if(ctes_scc.E()==0.0)
               std::cerr << "El módulo elástico para homogeneización debe ser no nulo." << std::endl;
            ctes_scc= geom->getCrossSectionProperties2d(ctes_scc);
          }
        else
          std::cerr << "No se encontró la definición geométrica denominada: '"
                        << cod_geom << "'" << std::endl;
      }
    else
      std::cerr << "El puntero al cargador de materiales es nulo." << std::endl;
  }

//! brief Returns strain at position being passed as parameter.
double XC::BaseElasticSection2d::getStrain(const double &y,const double &z) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1));
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::BaseElasticSection2d::sendData(CommParameters &cp)
  {
    int res= BaseElasticSection::sendData(cp);
    res+= cp.sendMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::BaseElasticSection2d::recvData(const CommParameters &cp)
  {
    int res= BaseElasticSection::recvData(cp);
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! \brief Devuelve los parámetros estáticos de la sección.
const XC::CrossSectionProperties2d &XC::BaseElasticSection2d::getCrossSectionProperties(void) const
  { return ctes_scc; }

void XC::BaseElasticSection2d::setCrossSectionProperties(const CrossSectionProperties2d &cs)  
  { ctes_scc= cs; }

void XC::BaseElasticSection2d::Print(std::ostream &s, int flag) const
  {
    s << "BaseElasticSection2d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

