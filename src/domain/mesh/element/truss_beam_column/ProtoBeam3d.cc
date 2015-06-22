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
//ProtoBeam3d.cc

#include "ProtoBeam3d.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "material/section/elastic_section/BaseElasticSection3d.h"

//! @brief Asigna valores a los parámetros estáticos de la sección.
void XC::ProtoBeam3d::set_material(const Material *m)
  {
    if(m)
      {
        const BaseElasticSection3d *scc= dynamic_cast<const BaseElasticSection3d *>(m);
	
        if(scc)
          ctes_scc= scc->getConstantesSeccion();
        else
          std::cerr << "ProtoBeam3d::ProtoBeam3d -- el material no es del tipo adecuado.\n";
      }
    else
      if(verborrea>0)
        std::cerr << "ProtoBeam3d::set_material; se pasó un puntero nulo." << std::endl;
  }

//! @brief Constructor por defecto.
XC::ProtoBeam3d::ProtoBeam3d(int tag,int class_tag,const Material *m)
  :Element1D(tag,class_tag,0,0)
  { set_material(m); }

//! @brief Constructor.
XC::ProtoBeam3d::ProtoBeam3d(int tag, int class_tag, int Nd1, int Nd2)
  :Element1D(tag,class_tag,Nd1,Nd2), ctes_scc() {}

//! @brief Constructor.
XC::ProtoBeam3d::ProtoBeam3d(int tag, int class_tag, double a, double e, double g, double jx, double iy, double iz,int Nd1, int Nd2)
  :Element1D(tag,class_tag,Nd1,Nd2), ctes_scc(e,a,iz,iy,g,jx) {}

//! @brief Lee un objeto XC::ProtoBeam3d desde archivo
bool XC::ProtoBeam3d::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ProtoBeam3d) Procesando comando: " << cmd << std::endl;
    if((cmd == "seccion") || (cmd == "for_each_section"))
      {
	ctes_scc.LeeCmd(status);
        return true;
      }
    else
      return Element1D::procesa_comando(status);
  }

int XC::ProtoBeam3d::getNumDOF(void) const
  { return 12; }

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::ProtoBeam3d::sendData(CommParameters &cp)
  {
    DbTagData &dt= getDbTagData();
    int res= Element1D::sendData(cp);
    res+= cp.sendMovable(ctes_scc,dt,CommMetaData(7));
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::ProtoBeam3d::recvData(const CommParameters &cp)
  {
    int res= Element1D::recvData(cp);
    res+= cp.receiveMovable(ctes_scc,getDbTagData(),CommMetaData(7));
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::ProtoBeam3d::GetProp(const std::string &cod) const
  {
    if(cod=="getG")
      return any_const_ptr(ctes_scc.G());
    if(cod=="getJx")
      return any_const_ptr(ctes_scc.J());
    if(cod=="getIy")
      return any_const_ptr(ctes_scc.Iy());
    if(cod=="getIz")
      return any_const_ptr(ctes_scc.Iz());
    else
      return Element1D::GetProp(cod);
  }
