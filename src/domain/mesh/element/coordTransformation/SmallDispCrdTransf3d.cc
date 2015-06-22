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
//SmallDispCrdTransf3d.cc

#include "SmallDispCrdTransf3d.h"
#include <utility/matrix/Vector.h>
#include <domain/mesh/node/Node.h>
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"

//! @brief Constructor por defecto
XC::SmallDispCrdTransf3d::SmallDispCrdTransf3d(int tag, int classTag)
  : CrdTransf3d(tag, classTag) {}

//! @brief Constructor:
XC::SmallDispCrdTransf3d::SmallDispCrdTransf3d(int tag, int class_tag, const XC::Vector &vecInLocXZPlane)
  : CrdTransf3d(tag, class_tag,vecInLocXZPlane) {}

int XC::SmallDispCrdTransf3d::computeElemtLengthAndOrient(void) const
  {
    // element projection
    static Vector dx(3);
    
    const Vector &ndICoords = nodeIPtr->getCrds();
    const Vector &ndJCoords = nodeJPtr->getCrds();
    
    dx(0)= ndJCoords(0) - ndICoords(0);
    dx(1)= ndJCoords(1) - ndICoords(1);
    dx(2)= ndJCoords(2) - ndICoords(2);
    
    if(!nodeIInitialDisp.empty())
      {
        dx(0) -= nodeIInitialDisp[0];
        dx(1) -= nodeIInitialDisp[1];
        dx(2) -= nodeIInitialDisp[2];
      }
    
    if(!nodeJInitialDisp.empty())
      {
        dx(0) += nodeJInitialDisp[0];
        dx(1) += nodeJInitialDisp[1];
        dx(2) += nodeJInitialDisp[2];
      }
    
    dx(0) += nodeJOffset(0);
    dx(1) += nodeJOffset(1);
    dx(2) += nodeJOffset(2);
    
    dx(0) -= nodeIOffset(0);
    dx(1) -= nodeIOffset(1);
    dx(2) -= nodeIOffset(2);
    
    // calculate the element length
    L = dx.Norm();
    
    if(L==0.0)
      {
        std::cerr << "SmallDispCrdTransf3d::computeElemtLengthAndOrien: 0 length\n";
        return -2;  
      }
    
    // calculate the element local x axis components (direction cossines)
    // wrt to the global coordinates
    R(0,0)= dx(0)/L;
    R(0,1)= dx(1)/L;
    R(0,2)= dx(2)/L;
    
    return 0;
  }

int XC::SmallDispCrdTransf3d::calculaEjesLocales(void) const
  {
    // Compute y = v cross x
    // Note: v(i) is stored in R(2,i)
    static Vector vAxis(3);
    vAxis(0)= R(2,0); vAxis(1)= R(2,1); vAxis(2)= R(2,2);
    
    vectorI(0) = R(0,0); vectorI(1) = R(0,1); vectorI(2) = R(0,2);
    
    vectorJ(0) = vAxis(1)*vectorI(2) - vAxis(2)*vectorI(1);
    vectorJ(1) = vAxis(2)*vectorI(0) - vAxis(0)*vectorI(2);
    vectorJ(2) = vAxis(0)*vectorI(1) - vAxis(1)*vectorI(0);
    
    double ynorm = vectorJ.Norm();
    
    if(ynorm == 0)
      {
        std::cerr << "SmallDispCrdTransf3d::getLocalAxes";
        std::cerr << "\nvector v that defines plane xz is parallel to x axis\n";
        return -3;
      }
    
    vectorJ /= ynorm;
    
    
    // Compute z = x cross y
    
    vectorK(0) = vectorI(1)*vectorJ(2) - vectorI(2)*vectorJ(1);
    vectorK(1) = vectorI(2)*vectorJ(0) - vectorI(0)*vectorJ(2);
    vectorK(2) = vectorI(0)*vectorJ(1) - vectorI(1)*vectorJ(0);
    
    // Fill in transformation matrix
    R(1,0)= vectorJ(0);
    R(1,1)= vectorJ(1);
    R(1,2)= vectorJ(2);
    
    R(2,0)= vectorK(0);
    R(2,1)= vectorK(1);
    R(2,2)= vectorK(2);
    
    return 0;
  }

//! @brief Devuelve la longitud inicial del elemento.
double  XC::SmallDispCrdTransf3d::getInitialLength(void) const
  { return L; }


//! @brief Devuelve la longitud del elemento deformado.
double XC::SmallDispCrdTransf3d::getDeformedLength(void) const
  { return L; }

//! @brief Devuelve los desplazamientos del elemento al que pertenece la transformación
//! Devuelve las siguientes magnitudes en el vector ub (ver calc_ub):
//! -ub(0)= dx2-dx1: Elongación sufrida por el elemento.
//! -ub(1)= (dy1-dy2)/L+gz1: Giro en torno a z del nodo 1.
//! -ub(2)= (dy1-dy2)/L+gz2: Giro en torno a z del nodo 2.
//! -ub(3)= (dz2-dz1)/L+gy1: Giro en torno a y del nodo 1.
//! -ub(4)= (dz2-dz1)/L+gy2: Giro en torno a y del nodo 2.
//! -ub(5)= dx2-dx1: Torsión sufrida por el elemento.
const XC::Vector &XC::SmallDispCrdTransf3d::getBasicTrialDisp(void) const
  {
    // determine global displacements
    const Vector &disp1 = nodeIPtr->getTrialDisp();
    const Vector &disp2 = nodeJPtr->getTrialDisp();

    static double ug[12]; //Desplazamiento de los nodos en coordenadas globales.
    inic_ug(disp1,disp2,ug);
    modif_ug_init_disp(ug);

    static double ul[12]; //Desplazamiento de los nodos en coordenadas locales.
    global_to_local(ug,ul);

    static double Wu[3];
    calc_Wu(ug,ul,Wu);

    static Vector ub(6);
    return calc_ub(ul,ub);
  }

const XC::Vector &XC::SmallDispCrdTransf3d::getBasicIncrDisp(void) const
  {
    // determine global displacements
    const Vector &disp1 = nodeIPtr->getIncrDisp();
    const Vector &disp2 = nodeJPtr->getIncrDisp();

    static double ug[12];
    inic_ug(disp1,disp2,ug);

    static double ul[12];
    global_to_local(ug,ul);

    static double Wu[3];
    calc_Wu(ug,ul,Wu);

    static Vector ub(6);
    return calc_ub(ul,ub);
  }

const XC::Vector &XC::SmallDispCrdTransf3d::getBasicIncrDeltaDisp(void) const
  {
    // determine global displacements
    const Vector &disp1 = nodeIPtr->getIncrDeltaDisp();
    const Vector &disp2 = nodeJPtr->getIncrDeltaDisp();

    static double ug[12];
    inic_ug(disp1,disp2,ug);

    static double ul[12];
    global_to_local(ug,ul);

    static double Wu[3];
    calc_Wu(ug,ul,Wu);

    static Vector ub(6);
    return calc_ub(ul,ub);
  }

const XC::Vector &XC::SmallDispCrdTransf3d::getBasicTrialVel(void) const
  {
    // determine global velocities
    const Vector &vel1 = nodeIPtr->getTrialVel();
    const Vector &vel2 = nodeJPtr->getTrialVel();

    static double vg[12];
    inic_ug(vel1,vel2,vg);

    static double vl[12];
    global_to_local(vg,vl);

    static double Wu[3];
    calc_Wu(vg,vl,Wu);

    static Vector vb(6);
    return calc_ub(vl,vb);
  }


const XC::Vector &XC::SmallDispCrdTransf3d::getBasicTrialAccel(void) const
  {
    // determine global accelerations
    const Vector &accel1 = nodeIPtr->getTrialAccel();
    const Vector &accel2 = nodeJPtr->getTrialAccel();

    static double ag[12];
    inic_ug(accel1,accel2,ag);

    static double al[12];
    global_to_local(ag,al);

    static double Wu[3];
    calc_Wu(ag,al,Wu);

    static Vector ab(6);
    return calc_ub(al,ab);
  }

//! @brief Transform resisting forces from the basic system to local coordinates
XC::Vector &XC::SmallDispCrdTransf3d::basic_to_local_resisting_force(const Vector &pb, const Vector &p0) const
  {
    static Vector pl(12);

    const double &q0= pb(0);
    const double &q1= pb(1);
    const double &q2= pb(2);
    const double &q3= pb(3);
    const double &q4= pb(4);
    const double &q5= pb(5);

    const double oneOverL = 1.0/L;

    pl[0]= -q0;
    pl[1]=  oneOverL*(q1+q2);
    pl[2]= -oneOverL*(q3+q4);
    pl[3]= -q5;
    pl[4]=  q3;
    pl[5]=  q1;
    pl[6]=  q0;
    pl[7]= -pl[1];
    pl[8]= -pl[2];
    pl[9]=  q5;
    pl[10]=  q4;
    pl[11]=  q2;

    pl[0]+= p0(0);
    pl[1]+= p0(1);
    pl[7]+= p0(2);
    pl[2]+= p0(3);
    pl[8]+= p0(4);

    return pl;
  }

const XC::Vector &XC::SmallDispCrdTransf3d::local_to_global_resisting_force(const Vector &pl) const
  {
    // transform resisting forces  from local to global coordinates
    static Vector pg(12);

    pg(0)= R(0,0)*pl[0] + R(1,0)*pl[1] + R(2,0)*pl[2];
    pg(1)= R(0,1)*pl[0] + R(1,1)*pl[1] + R(2,1)*pl[2];
    pg(2)= R(0,2)*pl[0] + R(1,2)*pl[1] + R(2,2)*pl[2];

    pg(3)= R(0,0)*pl[3] + R(1,0)*pl[4] + R(2,0)*pl[5];
    pg(4)= R(0,1)*pl[3] + R(1,1)*pl[4] + R(2,1)*pl[5];
    pg(5)= R(0,2)*pl[3] + R(1,2)*pl[4] + R(2,2)*pl[5];

    pg(6)= R(0,0)*pl[6] + R(1,0)*pl[7] + R(2,0)*pl[8];
    pg(7)= R(0,1)*pl[6] + R(1,1)*pl[7] + R(2,1)*pl[8];
    pg(8)= R(0,2)*pl[6] + R(1,2)*pl[7] + R(2,2)*pl[8];

    pg(9)= R(0,0)*pl[9] + R(1,0)*pl[10] + R(2,0)*pl[11];
    pg(10)= R(0,1)*pl[9] + R(1,1)*pl[10] + R(2,1)*pl[11];
    pg(11)= R(0,2)*pl[9] + R(1,2)*pl[10] + R(2,2)*pl[11];

    pg(3)+= -nodeIOffset(2)*pg(1) + nodeIOffset(1)*pg(2);
    pg(4)+=  nodeIOffset(2)*pg(0) - nodeIOffset(0)*pg(2);
    pg(5)+= -nodeIOffset(1)*pg(0) + nodeIOffset(0)*pg(1);

    pg(9)+= -nodeJOffset(2)*pg(7) + nodeJOffset(1)*pg(8);
    pg(10)+=  nodeJOffset(2)*pg(6) - nodeJOffset(0)*pg(8);
    pg(11)+= -nodeJOffset(1)*pg(6) + nodeJOffset(0)*pg(7);

    return pg;
  }

XC::Matrix &XC::SmallDispCrdTransf3d::basic_to_local_stiff_matrix(const XC::Matrix &KB) const
  {
    static Matrix kl(12,12); // Local stiffness
    static Matrix tmp(12,12); // Temporary storage

    const double oneOverL = 1.0/L;

    // Transform basic stiffness to local system
    // First compute kb*T_{bl}
    for(int i = 0; i < 6; i++)
      {
        tmp(i,0)  = -KB(i,0);
        tmp(i,1)  =  oneOverL*(KB(i,1)+KB(i,2));
        tmp(i,2)  = -oneOverL*(KB(i,3)+KB(i,4));
        tmp(i,3)  = -KB(i,5);
        tmp(i,4)  =  KB(i,3);
        tmp(i,5)  =  KB(i,1);
        tmp(i,6)  =  KB(i,0);
        tmp(i,7)  = -tmp(i,1);
        tmp(i,8)  = -tmp(i,2);
        tmp(i,9)  =  KB(i,5);
        tmp(i,10) =  KB(i,4);
        tmp(i,11) =  KB(i,2);
      }

    // Now compute T'_{bl}*(kb*T_{bl})
    for(int i = 0; i < 12; i++)
      {
        kl(0,i)  = -tmp(0,i);
        kl(1,i)  =  oneOverL*(tmp(1,i)+tmp(2,i));
        kl(2,i)  = -oneOverL*(tmp(3,i)+tmp(4,i));
        kl(3,i)  = -tmp(5,i);
        kl(4,i)  =  tmp(3,i);
        kl(5,i)  =  tmp(1,i);
        kl(6,i)  =  tmp(0,i);
        kl(7,i)  = -kl(1,i);
        kl(8,i)  = -kl(2,i);
        kl(9,i)  =  tmp(5,i);
        kl(10,i) =  tmp(4,i);
        kl(11,i) =  tmp(2,i);
      }
    return kl;
  }

const XC::Matrix &XC::SmallDispCrdTransf3d::computeRW(const Vector &nodeOffset) const
  {
    static Matrix RW(3,3);

    // Compute RW
    RW(0,0) = -R(0,1)*nodeOffset(2) + R(0,2)*nodeOffset(1);
    RW(1,0) = -R(1,1)*nodeOffset(2) + R(1,2)*nodeOffset(1);
    RW(2,0) = -R(2,1)*nodeOffset(2) + R(2,2)*nodeOffset(1);

    RW(0,1) =  R(0,0)*nodeOffset(2) - R(0,2)*nodeOffset(0);
    RW(1,1) =  R(1,0)*nodeOffset(2) - R(1,2)*nodeOffset(0);
    RW(2,1) =  R(2,0)*nodeOffset(2) - R(2,2)*nodeOffset(0);

    RW(0,2) = -R(0,0)*nodeOffset(1) + R(0,1)*nodeOffset(0);
    RW(1,2) = -R(1,0)*nodeOffset(1) + R(1,1)*nodeOffset(0);
    RW(2,2) = -R(2,0)*nodeOffset(1) + R(2,1)*nodeOffset(0);
    return RW;
  }

const XC::Matrix &XC::SmallDispCrdTransf3d::local_to_global_stiff_matrix(const Matrix &kl) const
  {
    static Matrix tmp(12,12); // Temporary storage

    const Matrix &RWI= computeRW(nodeIOffset);
    const Matrix &RWJ= computeRW(nodeJOffset);

    // Transform local stiffness to global system
    // First compute kl*T_{lg}
    int m;
    for(m = 0; m < 12; m++)
      {
        tmp(m,0) = kl(m,0)*R(0,0) + kl(m,1)*R(1,0)  + kl(m,2)*R(2,0);
        tmp(m,1) = kl(m,0)*R(0,1) + kl(m,1)*R(1,1)  + kl(m,2)*R(2,1);
        tmp(m,2) = kl(m,0)*R(0,2) + kl(m,1)*R(1,2)  + kl(m,2)*R(2,2);

        tmp(m,3) = kl(m,3)*R(0,0) + kl(m,4)*R(1,0)  + kl(m,5)*R(2,0);
        tmp(m,4) = kl(m,3)*R(0,1) + kl(m,4)*R(1,1)  + kl(m,5)*R(2,1);
        tmp(m,5) = kl(m,3)*R(0,2) + kl(m,4)*R(1,2)  + kl(m,5)*R(2,2);

        tmp(m,3)  += kl(m,0)*RWI(0,0)  + kl(m,1)*RWI(1,0)  + kl(m,2)*RWI(2,0);
        tmp(m,4)  += kl(m,0)*RWI(0,1)  + kl(m,1)*RWI(1,1)  + kl(m,2)*RWI(2,1);
        tmp(m,5)  += kl(m,0)*RWI(0,2)  + kl(m,1)*RWI(1,2)  + kl(m,2)*RWI(2,2);

        tmp(m,6) = kl(m,6)*R(0,0) + kl(m,7)*R(1,0)  + kl(m,8)*R(2,0);
        tmp(m,7) = kl(m,6)*R(0,1) + kl(m,7)*R(1,1)  + kl(m,8)*R(2,1);
        tmp(m,8) = kl(m,6)*R(0,2) + kl(m,7)*R(1,2)  + kl(m,8)*R(2,2);

        tmp(m,9)  = kl(m,9)*R(0,0) + kl(m,10)*R(1,0) + kl(m,11)*R(2,0);
        tmp(m,10) = kl(m,9)*R(0,1) + kl(m,10)*R(1,1) + kl(m,11)*R(2,1);
        tmp(m,11) = kl(m,9)*R(0,2) + kl(m,10)*R(1,2) + kl(m,11)*R(2,2);

        tmp(m,9)   += kl(m,6)*RWJ(0,0)  + kl(m,7)*RWJ(1,0)  + kl(m,8)*RWJ(2,0);
        tmp(m,10)  += kl(m,6)*RWJ(0,1)  + kl(m,7)*RWJ(1,1)  + kl(m,8)*RWJ(2,1);
        tmp(m,11)  += kl(m,6)*RWJ(0,2)  + kl(m,7)*RWJ(1,2)  + kl(m,8)*RWJ(2,2);
      }

    static Matrix kg(12,12); // Global stiffness for return
    // Now compute T'_{lg}*(kl*T_{lg})
    for(m = 0; m < 12; m++)
      {
        kg(0,m) = R(0,0)*tmp(0,m) + R(1,0)*tmp(1,m)  + R(2,0)*tmp(2,m);
        kg(1,m) = R(0,1)*tmp(0,m) + R(1,1)*tmp(1,m)  + R(2,1)*tmp(2,m);
        kg(2,m) = R(0,2)*tmp(0,m) + R(1,2)*tmp(1,m)  + R(2,2)*tmp(2,m);

        kg(3,m) = R(0,0)*tmp(3,m) + R(1,0)*tmp(4,m)  + R(2,0)*tmp(5,m);
        kg(4,m) = R(0,1)*tmp(3,m) + R(1,1)*tmp(4,m)  + R(2,1)*tmp(5,m);
        kg(5,m) = R(0,2)*tmp(3,m) + R(1,2)*tmp(4,m)  + R(2,2)*tmp(5,m);

        kg(3,m) += RWI(0,0)*tmp(0,m)  + RWI(1,0)*tmp(1,m) + RWI(2,0)*tmp(2,m);
        kg(4,m) += RWI(0,1)*tmp(0,m)  + RWI(1,1)*tmp(1,m) + RWI(2,1)*tmp(2,m);
        kg(5,m) += RWI(0,2)*tmp(0,m)  + RWI(1,2)*tmp(1,m) + RWI(2,2)*tmp(2,m);

        kg(6,m) = R(0,0)*tmp(6,m) + R(1,0)*tmp(7,m)  + R(2,0)*tmp(8,m);
        kg(7,m) = R(0,1)*tmp(6,m) + R(1,1)*tmp(7,m)  + R(2,1)*tmp(8,m);
        kg(8,m) = R(0,2)*tmp(6,m) + R(1,2)*tmp(7,m)  + R(2,2)*tmp(8,m);

        kg(9,m)  = R(0,0)*tmp(9,m) + R(1,0)*tmp(10,m) + R(2,0)*tmp(11,m);
        kg(10,m) = R(0,1)*tmp(9,m) + R(1,1)*tmp(10,m) + R(2,1)*tmp(11,m);
        kg(11,m) = R(0,2)*tmp(9,m) + R(1,2)*tmp(10,m) + R(2,2)*tmp(11,m);

        kg(9,m)  += RWJ(0,0)*tmp(6,m)  + RWJ(1,0)*tmp(7,m) + RWJ(2,0)*tmp(8,m);
        kg(10,m) += RWJ(0,1)*tmp(6,m)  + RWJ(1,1)*tmp(7,m) + RWJ(2,1)*tmp(8,m);
        kg(11,m) += RWJ(0,2)*tmp(6,m)  + RWJ(1,2)*tmp(7,m) + RWJ(2,2)*tmp(8,m);
      }
    return kg;
  }

const XC::Matrix &XC::SmallDispCrdTransf3d::getInitialGlobalStiffMatrix(const XC::Matrix &KB) const
  {
    const Matrix &kl= basic_to_local_stiff_matrix(KB); // Local stiffness
    return local_to_global_stiff_matrix(kl);
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::SmallDispCrdTransf3d::getDbTagData(void) const
  {
    static DbTagData retval(10);
    return retval;
  }

//! @brief Envia los miembros del objeto a través del canal que se pasa como parámetro.
int XC::SmallDispCrdTransf3d::sendSelf(CommParameters &cp)
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


//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::SmallDispCrdTransf3d::recvSelf(const CommParameters &cp)
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
