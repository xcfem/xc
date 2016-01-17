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
//ShellCorotCrdTransf3d.cc

#include "ShellCorotCrdTransf3d.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>

#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"
#include "domain/mesh/element/NodePtrs.h"
#include "domain/mesh/element/plane/shell/R3vectors.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor por defecto
XC::ShellCorotCrdTransf3d::ShellCorotCrdTransf3d(void)
  : ShellCrdTransf3dBase() {}

//! @brief Constructor.
XC::ShellCorotCrdTransf3d::ShellCorotCrdTransf3d(const Vector &v1,const Vector &v2,const Vector &v3)
  : ShellCrdTransf3dBase(v1,v2,v3) {}

//! @brief Calcula los vectores básicos a partir de las coordenadas de los nodos.
XC::ShellCorotCrdTransf3d::ShellCorotCrdTransf3d(const NodePtrs &theNodes)
  : ShellCrdTransf3dBase()
  { initialize(theNodes); }


//! @brief Constructor virtual.
XC::ShellCrdTransf3dBase *XC::ShellCorotCrdTransf3d::getCopy(void) const
  { return new ShellCorotCrdTransf3d(*this); }

//! @brief Establece la transformación a partir de las posiciones de los nodos.
int XC::ShellCorotCrdTransf3d::initialize(const NodePtrs &ptrs)
  {
    ShellCrdTransf3dBase::initialize(ptrs);

    const Vector &coor1= (*theNodes)[0]->getCrds();
    const Vector &coor2= (*theNodes)[1]->getCrds();
    const Vector &coor3= (*theNodes)[2]->getCrds();
    const Vector &coor4= (*theNodes)[3]->getCrds();

    v130= coor3-coor1;
    v240= coor4-coor2;

    const Vector c130= normalize(v130);
    const Vector c240= normalize(v240);

    g1= normalize(c130-c240);
    g2= normalize(c130+c240);
    g3= LovelyCrossProduct(g1,g2);
    g1trial= g1; g2trial= g2; g3trial= g3;
    g1commit= g1; g2commit= g2; g3commit= g3;
    return 0;
  }

inline XC::Vector get_traslational_disp(const XC::Vector &disp)
  { return XC::Vector(disp[0],disp[1],disp[2]); }

inline XC::Vector get_rotational_disp(const XC::Vector &disp)
  { return XC::Vector(disp[3],disp[4],disp[5]); }

//! @brief Actualiza la transformación a partir de las posiciones de los nodos.
int XC::ShellCorotCrdTransf3d::update(void)
  {
    const Vector d1= get_traslational_disp((*theNodes)[0]->getTrialDisp());
    const Vector d2= get_traslational_disp((*theNodes)[1]->getTrialDisp());
    const Vector d3= get_traslational_disp((*theNodes)[2]->getTrialDisp());
    const Vector d4= get_traslational_disp((*theNodes)[3]->getTrialDisp());

    const Vector v13= v130+d3-d1;
    const Vector v24= v240+d4-d2;

    const Vector c13= normalize(v13);
    const Vector c24= normalize(v24);

    g1trial= normalize(c13-c24);
    g2trial= normalize(c13+c24);
    g3trial= LovelyCrossProduct(g1trial,g2trial);

    return 0;
  }

//! @brief Devuelve la matriz de transformación correspondiente
//! a la configuración inicial del elemento.
XC::Matrix XC::ShellCorotCrdTransf3d::getR0(void) const
  { return getTrfMatrix(); }

//! @brief Devuelve la traspuesta de la matriz de transformación correspondiente
//! a la configuración inicial del elemento.
XC::Matrix XC::ShellCorotCrdTransf3d::getR0T(void) const
  {
    Matrix R(3,3);
    // Fill in transformation matrix
    R(0,0)= g1(0); R(1,0)= g1(1); R(2,0)= g1(2);
    R(0,1)= g2(0); R(1,1)= g2(1); R(2,1)= g2(2);
    R(0,2)= g3(0); R(1,2)= g3(1); R(2,2)= g3(2);
    return R;
  }

//! @brief Devuelve la matriz de transformación correspondiente
//! a la configuración actual del elemento.
XC::Matrix XC::ShellCorotCrdTransf3d::getR(void) const
  {
    Matrix retval(3,3);
    // Fill in transformation matrix
    retval(0,0)= g1trial(0); retval(0,1)= g1trial(1); retval(0,2)= g1trial(2);
    retval(1,0)= g2trial(0); retval(1,1)= g2trial(1); retval(1,2)= g2trial(2);
    retval(2,0)= g3trial(0); retval(2,1)= g3trial(1); retval(2,2)= g3trial(2);
    return retval;
  }

//! @brief Devuelve el vector expresado en coordenadas
//! locales a partir del desplazamiento expresado en globales.
XC::Vector XC::ShellCorotCrdTransf3d::global_to_local(const Vector &di,const Vector &vi0) const
  {
    const Matrix R= getR();
    const Matrix R0= getR0();
    const Matrix R0T= getR0T();
    const Matrix Rd= R*R0T;
    Vector retval(6);
    const Vector tdiA= R*get_traslational_disp(di);
    const Vector tdiB= (R-R0)*vi0;
    // std::cout << "vi0= " << vi0 << std::endl;
    // std::cout << "tdiA= " << tdiA << std::endl;
    // std::cout << "tdiB= " << tdiB << std::endl;
    const Vector tdi= tdiA+tdiB;
    const Vector rdi= Rd*get_rotational_disp(di);
    retval[0]= tdi(0);retval[1]= tdi(1);retval[2]= tdi(2);
    retval[3]= rdi(0);retval[4]= rdi(1);retval[5]= rdi(2);
    return retval;
  }

//! @brief Devuelve la traslación del nodo expresada en coordenadas
//! locales a partir del desplazamiento expresado en globales.
XC::Vector XC::ShellCorotCrdTransf3d::global_to_local_disp_nod(const int &i) const
  {
    const Vector di= (*theNodes)[i]->getTrialDisp();
    const Vector vi0= (*theNodes)[i]->getCrds()-vpos_centroide;
    return global_to_local(di,vi0);
  }

//! @brief Devuelve las componentes traslacionales de la
//! velocidad del nodo expresada en coordenadas locales a
//! partir del desplazamiento expresado en globales.
XC::Vector XC::ShellCorotCrdTransf3d::global_to_local_vel_nod(const int &i) const
  {
    const Vector di= (*theNodes)[i]->getTrialVel();
    const Vector vi0= (*theNodes)[i]->getCrds()-vpos_centroide;
    return global_to_local(di,vi0);
  }

//! @brief Devuelve las componentes traslacionales de la
//! aceleración del nodo expresada en coordenadas locales a
//! partir del desplazamiento expresado en globales.
XC::Vector XC::ShellCorotCrdTransf3d::global_to_local_accel_nod(const int &i) const
  {
    const Vector di= (*theNodes)[i]->getTrialAccel();
    const Vector vi0= (*theNodes)[i]->getCrds()-vpos_centroide;
    return global_to_local(di,vi0);
  }

//! @brief Transformación a globales del vector de cargas.
const XC::Vector &XC::ShellCorotCrdTransf3d::local_to_global_resisting_force(const Vector &pl) const
  {
    // transform resisting forces  from local to global coordinates
    static Vector pg(24);
    const Matrix R= getR();
    const Matrix Rd= R*getR0T();
    pg= local_to_global(R,Rd,pl);

    return pg;
  }

//! @brief Consuma la transformación de coordenadas de acuerdo con el estado actual.
int XC::ShellCorotCrdTransf3d::commitState(void)
  {
    g1commit= g1trial;
    g2commit= g2trial;
    g3commit= g3trial;
    return 0;
  }


//! @brief Devuelve el estado de la transformación al último consumado.
int XC::ShellCorotCrdTransf3d::revertToLastCommit(void)
  { 
    g1trial= g1commit;
    g2trial= g2commit;
    g3trial= g3commit;
    return 0;
  }


//! @brief Devuelve el estado de la transformación al inicial.
int XC::ShellCorotCrdTransf3d::revertToStart(void)
  {
    g1trial= g1; g2trial= g2; g3trial= g3;
    g1commit= g1; g2commit= g2; g3commit= g3;
    return 0;
  }

//! @brief Devuelve el vector de desplazamientos expresado en el sistema básico.
XC::Vector XC::ShellCorotCrdTransf3d::getBasicTrialDisp(const int &iNod) const
  { return global_to_local_disp_nod(iNod); }

// ! @brief Devuelve el vector de velocidades expresado en el sistema básico.
XC::Vector XC::ShellCorotCrdTransf3d::getBasicTrialVel(const int &iNod) const
  { return global_to_local_vel_nod(iNod); }

//! @brief Devuelve el vector de aceleraciones expresado en el sistema básico.
XC::Vector XC::ShellCorotCrdTransf3d::getBasicTrialAccel(const int &iNod) const
  { return global_to_local_accel_nod(iNod); }

//! @brief Transformación a globales de un vector desplazamiento.
XC::Vector XC::ShellCorotCrdTransf3d::local_to_global(const Matrix &R,const Matrix &Rd,const Vector &pl) const
  {
    Vector retval(24);
    //Nodo 1.
    retval(0)= R(0,0)*pl[0] + R(1,0)*pl[1] + R(2,0)*pl[2];
    retval(1)= R(0,1)*pl[0] + R(1,1)*pl[1] + R(2,1)*pl[2];
    retval(2)= R(0,2)*pl[0] + R(1,2)*pl[1] + R(2,2)*pl[2];

    retval(3)= Rd(0,0)*pl[3] + Rd(1,0)*pl[4] + Rd(2,0)*pl[5];
    retval(4)= Rd(0,1)*pl[3] + Rd(1,1)*pl[4] + Rd(2,1)*pl[5];
    retval(5)= Rd(0,2)*pl[3] + Rd(1,2)*pl[4] + Rd(2,2)*pl[5];

    //Nodo 2.
    retval(6)= R(0,0)*pl[6] + R(1,0)*pl[7] + R(2,0)*pl[8];
    retval(7)= R(0,1)*pl[6] + R(1,1)*pl[7] + R(2,1)*pl[8];
    retval(8)= R(0,2)*pl[6] + R(1,2)*pl[7] + R(2,2)*pl[8];

    retval(9)= Rd(0,0)*pl[9] + Rd(1,0)*pl[10] + Rd(2,0)*pl[11];
    retval(10)= Rd(0,1)*pl[9] + Rd(1,1)*pl[10] + Rd(2,1)*pl[11];
    retval(11)= Rd(0,2)*pl[9] + Rd(1,2)*pl[10] + Rd(2,2)*pl[11];

    //Nodo 3.
    retval(12)= R(0,0)*pl[12] + R(1,0)*pl[13] + R(2,0)*pl[14];
    retval(13)= R(0,1)*pl[12] + R(1,1)*pl[13] + R(2,1)*pl[14];
    retval(14)= R(0,2)*pl[12] + R(1,2)*pl[13] + R(2,2)*pl[14];

    retval(15)= Rd(0,0)*pl[15] + Rd(1,0)*pl[16] + Rd(2,0)*pl[17];
    retval(16)= Rd(0,1)*pl[15] + Rd(1,1)*pl[16] + Rd(2,1)*pl[17];
    retval(17)= Rd(0,2)*pl[15] + Rd(1,2)*pl[16] + Rd(2,2)*pl[17];

    //Nodo 4.
    retval(18)= R(0,0)*pl[18] + R(1,0)*pl[19] + R(2,0)*pl[20];
    retval(19)= R(0,1)*pl[18] + R(1,1)*pl[19] + R(2,1)*pl[20];
    retval(20)= R(0,2)*pl[18] + R(1,2)*pl[19] + R(2,2)*pl[20];

    retval(21)= Rd(0,0)*pl[21] + Rd(1,0)*pl[22] + Rd(2,0)*pl[23];
    retval(22)= Rd(0,1)*pl[21] + Rd(1,1)*pl[22] + Rd(2,1)*pl[23];
    retval(23)= Rd(0,2)*pl[21] + Rd(1,2)*pl[22] + Rd(2,2)*pl[23];

    return retval;
  }

//! @brief Transformación a globales de una matriz.
XC::Matrix XC::ShellCorotCrdTransf3d::local_to_global(const Matrix &R,const Matrix &Rd,const Matrix &kl) const
  {
    static Matrix tmp(24,24);

    // Transform local matrix to global system
    // First compute kl*T_{lg}
    for(int m = 0; m < 24; m++)
      {
        tmp(m,0)= kl(m,0)*R(0,0) + kl(m,1)*R(1,0)  + kl(m,2)*R(2,0);
        tmp(m,1)= kl(m,0)*R(0,1) + kl(m,1)*R(1,1)  + kl(m,2)*R(2,1);
        tmp(m,2)= kl(m,0)*R(0,2) + kl(m,1)*R(1,2)  + kl(m,2)*R(2,2);

        tmp(m,3)= kl(m,3)*Rd(0,0) + kl(m,4)*Rd(1,0)  + kl(m,5)*Rd(2,0);
        tmp(m,4)= kl(m,3)*Rd(0,1) + kl(m,4)*Rd(1,1)  + kl(m,5)*Rd(2,1);
        tmp(m,5)= kl(m,3)*Rd(0,2) + kl(m,4)*Rd(1,2)  + kl(m,5)*Rd(2,2);

        tmp(m,6)= kl(m,6)*R(0,0) + kl(m,7)*R(1,0)  + kl(m,8)*R(2,0);
        tmp(m,7)= kl(m,6)*R(0,1) + kl(m,7)*R(1,1)  + kl(m,8)*R(2,1);
        tmp(m,8)= kl(m,6)*R(0,2) + kl(m,7)*R(1,2)  + kl(m,8)*R(2,2);

        tmp(m,9)= kl(m,9)*Rd(0,0) + kl(m,10)*Rd(1,0) + kl(m,11)*Rd(2,0);
        tmp(m,10)= kl(m,9)*Rd(0,1) + kl(m,10)*Rd(1,1) + kl(m,11)*Rd(2,1);
        tmp(m,11)= kl(m,9)*Rd(0,2) + kl(m,10)*Rd(1,2) + kl(m,11)*Rd(2,2);

        tmp(m,12)= kl(m,12)*R(0,0) + kl(m,13)*R(1,0) + kl(m,14)*R(2,0);
        tmp(m,13)= kl(m,12)*R(0,1) + kl(m,13)*R(1,1) + kl(m,14)*R(2,1);
        tmp(m,14)= kl(m,12)*R(0,2) + kl(m,13)*R(1,2) + kl(m,14)*R(2,2);

        tmp(m,15)= kl(m,15)*Rd(0,0) + kl(m,16)*Rd(1,0) + kl(m,17)*Rd(2,0);
        tmp(m,16)= kl(m,15)*Rd(0,1) + kl(m,16)*Rd(1,1) + kl(m,17)*Rd(2,1);
        tmp(m,17)= kl(m,15)*Rd(0,2) + kl(m,16)*Rd(1,2) + kl(m,17)*Rd(2,2);

        tmp(m,18)= kl(m,18)*R(0,0) + kl(m,19)*R(1,0) + kl(m,20)*R(2,0);
        tmp(m,19)= kl(m,18)*R(0,1) + kl(m,19)*R(1,1) + kl(m,20)*R(2,1);
        tmp(m,20)= kl(m,18)*R(0,2) + kl(m,19)*R(1,2) + kl(m,20)*R(2,2);

        tmp(m,21)= kl(m,21)*Rd(0,0) + kl(m,22)*Rd(1,0) + kl(m,23)*Rd(2,0);
        tmp(m,22)= kl(m,21)*Rd(0,1) + kl(m,22)*Rd(1,1) + kl(m,23)*Rd(2,1);
        tmp(m,23)= kl(m,21)*Rd(0,2) + kl(m,22)*Rd(1,2) + kl(m,23)*Rd(2,2);
      }

    Matrix retval(24,24); // Global matrix for return
    // Now compute T'_{lg}*(kl*T_{lg})
    for(int m = 0;m<24;m++)
      {
        retval(0,m) = R(0,0)*tmp(0,m) + R(1,0)*tmp(1,m)  + R(2,0)*tmp(2,m);
        retval(1,m) = R(0,1)*tmp(0,m) + R(1,1)*tmp(1,m)  + R(2,1)*tmp(2,m);
        retval(2,m) = R(0,2)*tmp(0,m) + R(1,2)*tmp(1,m)  + R(2,2)*tmp(2,m);

        retval(3,m) = Rd(0,0)*tmp(3,m) + Rd(1,0)*tmp(4,m) + Rd(2,0)*tmp(5,m);
        retval(4,m) = Rd(0,1)*tmp(3,m) + Rd(1,1)*tmp(4,m) + Rd(2,1)*tmp(5,m);
        retval(5,m) = Rd(0,2)*tmp(3,m) + Rd(1,2)*tmp(4,m) + Rd(2,2)*tmp(5,m);

        retval(6,m) = R(0,0)*tmp(6,m) + R(1,0)*tmp(7,m)  + R(2,0)*tmp(8,m);
        retval(7,m) = R(0,1)*tmp(6,m) + R(1,1)*tmp(7,m)  + R(2,1)*tmp(8,m);
        retval(8,m) = R(0,2)*tmp(6,m) + R(1,2)*tmp(7,m)  + R(2,2)*tmp(8,m);

        retval(9,m)  = Rd(0,0)*tmp(9,m) + Rd(1,0)*tmp(10,m) + Rd(2,0)*tmp(11,m);
        retval(10,m) = Rd(0,1)*tmp(9,m) + Rd(1,1)*tmp(10,m) + Rd(2,1)*tmp(11,m);
        retval(11,m) = Rd(0,2)*tmp(9,m) + Rd(1,2)*tmp(10,m) + Rd(2,2)*tmp(11,m);

        retval(12,m) = R(0,0)*tmp(12,m) + R(1,0)*tmp(13,m) + R(2,0)*tmp(14,m);
        retval(13,m) = R(0,1)*tmp(12,m) + R(1,1)*tmp(13,m) + R(2,1)*tmp(14,m);
        retval(14,m) = R(0,2)*tmp(12,m) + R(1,2)*tmp(13,m) + R(2,2)*tmp(14,m);

        retval(15,m) = Rd(0,0)*tmp(15,m) + Rd(1,0)*tmp(16,m) + Rd(2,0)*tmp(17,m);
        retval(16,m) = Rd(0,1)*tmp(15,m) + Rd(1,1)*tmp(16,m) + Rd(2,1)*tmp(17,m);
        retval(17,m) = Rd(0,2)*tmp(15,m) + Rd(1,2)*tmp(16,m) + Rd(2,2)*tmp(17,m);

        retval(18,m) = R(0,0)*tmp(18,m) + R(1,0)*tmp(19,m) + R(2,0)*tmp(20,m);
        retval(19,m) = R(0,1)*tmp(18,m) + R(1,1)*tmp(19,m) + R(2,1)*tmp(20,m);
        retval(20,m) = R(0,2)*tmp(18,m) + R(1,2)*tmp(19,m) + R(2,2)*tmp(20,m);

        retval(21,m) = Rd(0,0)*tmp(21,m) + Rd(1,0)*tmp(22,m) + Rd(2,0)*tmp(23,m);
        retval(22,m) = Rd(0,1)*tmp(21,m) + Rd(1,1)*tmp(22,m) + Rd(2,1)*tmp(23,m);
        retval(23,m) = Rd(0,2)*tmp(21,m) + Rd(1,2)*tmp(22,m) + Rd(2,2)*tmp(23,m);
      }
    return retval;
  }

//! @brief Devuelve la matriz de rigidez tangente expresada en globales.
void XC::ShellCorotCrdTransf3d::getGlobalTangent(Matrix &stiff) const
  {
    const Matrix R= getR();
    const Matrix Rd= R*getR0T();
    stiff= local_to_global(R,Rd,stiff);
  }

//! @brief Devuelve el vector residuo expresado en globales.
void XC::ShellCorotCrdTransf3d::getGlobalResidAndTangent(Vector &resid,Matrix &stiff) const
  {
    //std::cout << "antes resid= " << resid << std::endl;
    const Matrix R= getR();
    const Matrix Rd= R*getR0T();
    //std::cout << "R= " << R << std::endl;   
    //std::cout << "Rd= " << Rd << std::endl;   
    resid= local_to_global(R,Rd,resid);
    //std::cout << "después resid= " << resid << std::endl;
    stiff= local_to_global(R,Rd,stiff);
  }

//! @brief Devuelve el vector de cargas expresado en el sistema global del elemento.
const XC::Vector &XC::ShellCorotCrdTransf3d::getGlobalResistingForce(const Vector &p0) const
  { return local_to_global_resisting_force(p0); }

//! @brief Envia el objeto a través del canal que se pasa como parámetro.
int XC::ShellCorotCrdTransf3d::sendSelf(CommParameters &cp)
  {
    inicComm(3);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ShellCorotCrdTransf3d::sendSelf() - data could not be sent\n" ;
    return res;
  }


//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::ShellCorotCrdTransf3d::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    int res= cp.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << "ShellCorotCrdTransf3d::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }
