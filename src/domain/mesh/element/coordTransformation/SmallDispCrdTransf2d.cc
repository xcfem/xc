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
//SmallDispCrdTransf2d.cpp

#include "SmallDispCrdTransf2d.h"
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"

//! @brief Constructor por defecto
XC::SmallDispCrdTransf2d::SmallDispCrdTransf2d(int tag, int classTag)
  : XC::CrdTransf2d(tag, classTag) {}


//! @brief Transform resisting forces from the basic system to local coordinates
XC::Vector &XC::SmallDispCrdTransf2d::basic_to_local_resisting_force(const XC::Vector &pb, const XC::Vector &p0) const
  {
    static Vector pl(6);

    const double &q0= pb(0);
    const double &q1= pb(1);
    const double &q2= pb(2);

    const double oneOverL = 1.0/L;
    const double V= oneOverL*(q1+q2);

    pl[0] = -q0;
    pl[1] =  V;
    pl[2] =  q1;
    pl[3] =  q0;
    pl[4] = -V;
    pl[5] =  q2;
    
    // add end forces due to element p0 loads
    pl[0]+= p0(0);
    pl[1]+= p0(1);
    pl[4]+= p0(2);

    return pl;
  }

//! @brief Transform resisting forces from local to global coordinates
const XC::Vector &XC::SmallDispCrdTransf2d::local_to_global_resisting_force(const XC::Vector &pl) const
  {
    static XC::Vector pg(6);

    pg(0) = cosTheta*pl[0] - sinTheta*pl[1];
    pg(1) = sinTheta*pl[0] + cosTheta*pl[1];
    
    pg(3) = cosTheta*pl[3] - sinTheta*pl[4];
    pg(4) = sinTheta*pl[3] + cosTheta*pl[4];
    
    pg(2) = pl[2];
    pg(5) = pl[5];        
    
    const double &t02= T02();
    const double &t12= T12();
        
    pg(2) += t02*pl[0] + t12*pl[1];

    const double &t35= T35();
    const double &t45= T45();
       
    pg(5) += t35*pl[3] + t45*pl[4];

    return pg;
  }

//! @brief Devuelve las coordenadas globales del punto a partir de las locales.
const XC::Vector &XC::SmallDispCrdTransf2d::getPointGlobalCoordFromLocal(const XC::Vector &xl) const
  {
    static Vector xg(2);
    
    const Vector &nodeICoords = nodeIPtr->getCrds();
    xg(0)= nodeICoords(0);
    xg(1)= nodeICoords(1);
    
    xg(0)+= nodeIOffset(0);
    xg(1)+= nodeIOffset(1);
    
    // xg = xg + Rlj'*xl
    xg(0) += cosTheta*xl(0) - sinTheta*xl(1);
    xg(1) += sinTheta*xl(0) + cosTheta*xl(1);
    
    return xg;  
  }

//! @brief Devuelve el vector desplazamiento expresado en coordenadas globales
const XC::Vector &XC::SmallDispCrdTransf2d::getPointGlobalDisplFromBasic(double xi, const Vector &uxb) const
  {
    // determine global displacements
    const Vector &disp1 = nodeIPtr->getTrialDisp();
    const Vector &disp2 = nodeJPtr->getTrialDisp();
    
    static Vector ug(6);
    for(int i = 0; i < 3; i++)
      {
        ug(i)   = disp1(i);
        ug(i+3) = disp2(i);
      }
    
    if(!nodeIInitialDisp.empty())
      {
        for(int j=0; j<3; j++)
          ug[j]-= nodeIInitialDisp[j];
      }
    
    if(!nodeJInitialDisp.empty())
      {
        for(int j=0; j<3; j++)
          ug[j+3]-= nodeJInitialDisp[j];
      }
    
    // transform global end displacements to local coordinates
    static Vector ul(6);      // total displacements
    
    ul(0)=  cosTheta*ug(0) + sinTheta*ug(1);
    ul(1)= -sinTheta*ug(0) + cosTheta*ug(1);
    ul(2)=  ug(2);
    ul(3)=  cosTheta*ug(3) + sinTheta*ug(4);
    ul(4)= -sinTheta*ug(3) + cosTheta*ug(4);
    ul(5)=  ug(5);
    

    const double &t02= T02();
    const double &t12= T12();
        
    ul(0)+= t02*ug(2);
    ul(1)+= t12*ug(2);

    const double &t35= T35();
    const double &t45= T45();
        
    ul(3)+= t35*ug(5);
    ul(4)+= t45*ug(5);
    
    // compute displacements at point xi, in local coordinates
    static Vector uxl(2), uxg(2);
    
    uxl(0)= uxb(0) +        ul(0);
    uxl(1)= uxb(1) + (1-xi)*ul(1) + xi*ul(4);
    
    // rotate displacements to global coordinates
    // uxg = RljT*uxl
    uxg(0) = cosTheta*uxl(0) - sinTheta*uxl(1);
    uxg(1) = sinTheta*uxl(0) + cosTheta*uxl(1);
    
    return uxg;
  }
//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::SmallDispCrdTransf2d::getDbTagData(void) const
  {
    static DbTagData retval(10);
    return retval;
  }

//! @brief Envia el objeto through the channel being passed as parameter.
int XC::SmallDispCrdTransf2d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::SmallDispCrdTransf2d::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
