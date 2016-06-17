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
//ShellLinearCrdTransf3d.cc

#include "ShellLinearCrdTransf3d.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>

#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"
#include "domain/mesh/element/NodePtrs.h"
#include "domain/mesh/element/plane/shell/R3vectors.h"

//! @brief Constructor por defecto
XC::ShellLinearCrdTransf3d::ShellLinearCrdTransf3d(void)
  : ShellCrdTransf3dBase() {}

//! @brief Constructor.
XC::ShellLinearCrdTransf3d::ShellLinearCrdTransf3d(const Vector &v1,const Vector &v2,const Vector &v3)
  : ShellCrdTransf3dBase(v1,v2,v3) {}

//! @brief Calcula los vectores básicos a partir de las coordenadas de los nodos.
XC::ShellLinearCrdTransf3d::ShellLinearCrdTransf3d(const NodePtrs &theNodes)
  : ShellCrdTransf3dBase()
  { initialize(theNodes); }


//! @brief Constructor virtual.
XC::ShellCrdTransf3dBase *XC::ShellLinearCrdTransf3d::getCopy(void) const
  { return new ShellLinearCrdTransf3d(*this); }

//! @brief Establece la transformación a partir de las posiciones de los nodos.
int XC::ShellLinearCrdTransf3d::initialize(const NodePtrs &ptrs)
  {
    ShellCrdTransf3dBase::initialize(ptrs);
    //could compute derivatives \frac{ \partial {\bf x} }{ \partial L_1 }
    //                     and  \frac{ \partial {\bf x} }{ \partial L_2 }
    //and use those as basis vectors but this is easier
    //and the shell is flat anyway.

    static Vector temp(3);

    static Vector v1(3);
    static Vector v2(3);
    static Vector v3(3);

    //get two vectors (v1, v2) in plane of shell by
    // nodal coordinate differences

    const Vector &coor0= (*theNodes)[0]->getCrds();
    const Vector &coor1= (*theNodes)[1]->getCrds();
    const Vector &coor2= (*theNodes)[2]->getCrds();
    const Vector &coor3= (*theNodes)[3]->getCrds();

    v1.Zero( );
    //v1= 0.5 * ( coor2 + coor1 - coor3 - coor0 );
    v1= coor2;
    v1+= coor1;
    v1-= coor3;
    v1-= coor0;
    v1*= 0.50;

    v2.Zero( );
    //v2= 0.5 * ( coor3 + coor2 - coor1 - coor0 );
    v2= coor3;
    v2+= coor2;
    v2-= coor1;
    v2-= coor0;
    v2*= 0.50;

    //normalize v1
    //double length= LovelyNorm( v1 );
    double length= v1.Norm( );
    v1/= length;


    //Gram-Schmidt process for v2

    //double alpha= LovelyInnerProduct( v2, v1 );
    const double alpha= v2^v1;

    //v2 -= alpha*v1;
    temp= v1;
    temp *= alpha;
    v2-= temp;


    //normalize v2
    //length= LovelyNorm( v2 );
    length= v2.Norm( );
    v2/= length;


    //cross product for v3
    v3= LovelyCrossProduct( v1, v2 );

    g1= v1; g2= v2; g3= v3;
    return 0;
  }

//! @brief Actualiza la transformación a partir de las posiciones de los nodos.
int XC::ShellLinearCrdTransf3d::update(void)
  { return 0; }

//! @brief Consuma la transformación de coordenadas de acuerdo con el estado actual.
int XC::ShellLinearCrdTransf3d::commitState(void)
  { return 0; }


//! @brief Devuelve el estado de la transformación al último commit.
int XC::ShellLinearCrdTransf3d::revertToLastCommit(void)
  { return 0; }


//! @brief Devuelve el estado de la transformación al inicial.
int XC::ShellLinearCrdTransf3d::revertToStart(void)
  { return 0; }

//! @brief Devuelve el vector de desplazamientos expresado en el sistema básico.
XC::Vector XC::ShellLinearCrdTransf3d::getBasicTrialDisp(const int &i) const
  { return (*theNodes)[i]->getTrialDisp(); }

//! @brief Devuelve el vector de velocidades expresado en el sistema básico.
XC::Vector XC::ShellLinearCrdTransf3d::getBasicTrialVel(const int &i) const
  { return (*theNodes)[i]->getTrialVel(); }

//! @brief Devuelve el vector de aceleraciones expresado en el sistema básico.
XC::Vector XC::ShellLinearCrdTransf3d::getBasicTrialAccel(const int &i) const
  { return (*theNodes)[i]->getTrialAccel(); }

//! @brief Transformación a globales del vector de cargas.
const XC::Vector &XC::ShellLinearCrdTransf3d::local_to_global_resisting_force(const Vector &pl) const
  {
    // transform resisting forces  from local to global coordinates
    static Vector pg(24);
    const Matrix &R= getTrfMatrix();
    pg= local_to_global(R,pl);

    return pg;
  }

//! @brief Transformación a globales de la matriz de rigidez (NO SE USA).
const XC::Matrix &XC::ShellLinearCrdTransf3d::local_to_global_stiff_matrix(const Matrix &kl) const
  {
    static Matrix kg(24,24);
    const Matrix &R= getTrfMatrix();

    kg= local_to_global(R,kl);
    return kg;
  }

//! @brief Devuelve el vector de cargas expresado en el sistema global del elemento.
const XC::Vector &XC::ShellLinearCrdTransf3d::getGlobalResistingForce(const Vector &p0) const
  { return local_to_global_resisting_force(p0); }

//! @brief Envia el objeto through the channel being passed as parameter.
int XC::ShellLinearCrdTransf3d::sendSelf(CommParameters &cp)
  {
    inicComm(3);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ShellLinearCrdTransf3d::sendSelf() - data could not be sent\n" ;
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::ShellLinearCrdTransf3d::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    int res= cp.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << "ShellLinearCrdTransf3d::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }


