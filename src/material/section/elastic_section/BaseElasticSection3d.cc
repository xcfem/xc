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
//BaseElasticSection3d.cpp

#include <material/section/elastic_section/BaseElasticSection3d.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>



XC::BaseElasticSection3d::BaseElasticSection3d(int tag, int classTag, const size_t &dim, MaterialLoader *mat_ldr)
  : BaseElasticSection(tag, classTag,dim,mat_ldr), ctes_scc() {}

XC::BaseElasticSection3d::BaseElasticSection3d(int tag,int classTag,const size_t &dim,const CrossSectionProperties3d &ctes,MaterialLoader *mat_ldr)
  : BaseElasticSection(tag, classTag,dim,mat_ldr), ctes_scc(ctes) {}

void XC::BaseElasticSection3d::sectionGeometry(const std::string &cod_geom)
  {
    const MaterialLoader *ldr= getMaterialLoader();
    if(ldr)
      {
        const GeomSection *geom= ldr->find_ptr_geom_section(cod_geom);
        if(geom)
          {
            if(ctes_scc.E()==0.0)
               std::cerr << "El módulo elástico para homogeneización debe ser no nulo." << std::endl;
            ctes_scc= geom->getCrossSectionProperties3d(ctes_scc);
          }
        else
          std::cerr << "No se encontró la definición geométrica denominada: '"
                        << cod_geom << "'" << std::endl;
      }
    else
      std::cerr << "El puntero al cargador de materiales es nulo." << std::endl;
  }

//! \brief Devuelve los parámetros estáticos de la sección.
const XC::CrossSectionProperties3d &XC::BaseElasticSection3d::getCrossSectionProperties(void) const
  { return ctes_scc; }

//! @brief Returns strain at position being passed as parameter.
double XC::BaseElasticSection3d::getStrain(const double &y,const double &z) const
  {
    const Vector &def= getSectionDeformation();
    return (def(0) + y*def(1) + z*def(2));
  }

void XC::BaseElasticSection3d::setCrossSectionProperties(const CrossSectionProperties3d &cs)  
  { ctes_scc= cs; }


//! @brief Send members del objeto through the channel being passed as parameter.
int XC::BaseElasticSection3d::sendData(CommParameters &cp)
  {
    int res= BaseElasticSection::sendData(cp);
    res+= cp.sendMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::BaseElasticSection3d::recvData(const CommParameters &cp)
  {
    int res= BaseElasticSection::recvData(cp);
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

void XC::BaseElasticSection3d::Print(std::ostream &s, int flag) const
  {
    s << "BaseElasticSection3d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

