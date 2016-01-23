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
//ProtoBeam2d.cc

#include "ProtoBeam2d.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableVector.h"
#include "material/section/elastic_section/BaseElasticSection2d.h"

//! @brief Asigna valores a los parámetros estáticos de la sección.
void XC::ProtoBeam2d::set_material(const Material *m)
  {
    if(m)
      {
        const BaseElasticSection2d *scc= dynamic_cast<const BaseElasticSection2d *>(m);
        if(scc)
          ctes_scc= scc->getCrossSectionProperties();
        else
          {
            std::cerr << "ProtoBeam2d::ProtoBeam2d -- el material no es del tipo adecuado.\n";
          }
      }
    else
      if(verborrea>0)
        std::cerr << "ProtoBeam2d::set_material; se pasó un puntero nulo." << std::endl;
  }

//! @brief Constructor por defecto.
XC::ProtoBeam2d::ProtoBeam2d(int tag,int class_tag,const Material *m)
  :Element1D(tag,class_tag,0,0)
  { set_material(m); }

//! @brief Constructor.
XC::ProtoBeam2d::ProtoBeam2d(int tag, int class_tag, double a, double e, double i, int Nd1, int Nd2)
  :Element1D(tag,class_tag,Nd1,Nd2), ctes_scc(e,a,i) {}

int XC::ProtoBeam2d::getNumDOF(void) const
  { return 6; }

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::ProtoBeam2d::sendData(CommParameters &cp)
  {
    DbTagData &dt= getDbTagData();
    int res= Element1D::sendData(cp);
    res+= cp.sendMovable(ctes_scc,dt,CommMetaData(7));
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::ProtoBeam2d::recvData(const CommParameters &cp)
  {
    int res= Element1D::recvData(cp);
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(7));
    return res;
  }
