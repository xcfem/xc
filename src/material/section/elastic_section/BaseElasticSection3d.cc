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
#include "modelador/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"

XC::BaseElasticSection3d::BaseElasticSection3d(int tag, int classTag, const size_t &dim, MaterialLoader *mat_ldr)
  : BaseElasticSection(tag, classTag,dim,mat_ldr), ctes_scc() {}

XC::BaseElasticSection3d::BaseElasticSection3d(int tag,int classTag,const size_t &dim,const ConstantesSecc3d &ctes,MaterialLoader *mat_ldr)
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
            ctes_scc= geom->getConstantesSecc3d(ctes_scc);
          }
        else
          std::cerr << "No se encontró la definición geométrica denominada: '"
                        << cod_geom << "'" << std::endl;
      }
    else
      std::cerr << "El puntero al cargador de materiales es nulo." << std::endl;
  }

//! @brief Lee un objeto XC::BaseElasticSection3d desde archivo
bool XC::BaseElasticSection3d::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(BaseElasticSection3d) Procesando comando: " << cmd << std::endl;
    if((cmd == "paramSeccion"))
      {
	ctes_scc.LeeCmd(status);
        return true;
      }
    else if(cmd == "geomSeccion")
      {
        const std::string cod_geom= interpretaString(status.GetString()); //Código de la geometría.
        sectionGeometry(cod_geom);
        return true;
      }
    else if(cmd == "checkValues")
      {
        status.GetString();
        ctes_scc.check_values();
        return true;
      }
    else
      return BaseElasticSection::procesa_comando(status);
  }

//! \brief Devuelve los parámetros estáticos de la sección.
const XC::ConstantesSecc3d &XC::BaseElasticSection3d::getConstantesSeccion(void) const
  { return ctes_scc; }


void XC::BaseElasticSection3d::setConstantesSeccion(const ConstantesSecc3d &cs)  
  { ctes_scc= cs; }


//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::BaseElasticSection3d::sendData(CommParameters &cp)
  {
    int res= BaseElasticSection::sendData(cp);
    res+= cp.sendMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::BaseElasticSection3d::recvData(const CommParameters &cp)
  {
    int res= BaseElasticSection::recvData(cp);
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(8));
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::BaseElasticSection3d::GetProp(const std::string &cod) const
  {
    if(cod=="paramSeccion")
      return any_const_ptr(&ctes_scc);
    else
      return BaseElasticSection::GetProp(cod);
  }
 
void XC::BaseElasticSection3d::Print(std::ostream &s, int flag) const
  {
    s << "BaseElasticSection3d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

