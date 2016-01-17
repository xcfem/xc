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
//BaseElasticSection2d.cpp

#include <material/section/elastic_section/BaseElasticSection2d.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"

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
            ctes_scc= geom->getConstantesSecc2d(ctes_scc);
          }
        else
          std::cerr << "No se encontró la definición geométrica denominada: '"
                        << cod_geom << "'" << std::endl;
      }
    else
      std::cerr << "El puntero al cargador de materiales es nulo." << std::endl;
  }


//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::BaseElasticSection2d::sendData(CommParameters &cp)
  {
    int res= BaseElasticSection::sendData(cp);
    res+= cp.sendMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::BaseElasticSection2d::recvData(const CommParameters &cp)
  {
    int res= BaseElasticSection::recvData(cp);
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! \brief Devuelve los parámetros estáticos de la sección.
const XC::ConstantesSecc2d &XC::BaseElasticSection2d::getConstantesSeccion(void) const
  { return ctes_scc; }

void XC::BaseElasticSection2d::setConstantesSeccion(const ConstantesSecc2d &cs)  
  { ctes_scc= cs; }

void XC::BaseElasticSection2d::Print(std::ostream &s, int flag) const
  {
    s << "BaseElasticSection2d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

