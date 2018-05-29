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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite XC::Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              Sept2003
//# UPDATE HISTORY:    28May2004, Zhao put all Ks & Rs in the integration cycle and
//#                          and use minor symmetries to make concise and efficient
//#                    April2005 Zhao adds new output options
//#                    Sept2005  Zhao shortens the codes
//===============================================================================

#include <domain/mesh/element/volumetric/TotalLagrangianFD20NodeBrick/TotalLagrangianFD20NodeBrick.h>
#include <utility/matrix/nDarray/basics.h>
#include <utility/matrix/nDarray/nDarray.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/nDarray/BJtensor.h>
#include <utility/matrix/nDarray/stresst.h>
#include <utility/matrix/nDarray/straint.h>

#include <material/nD/NDMaterial.h>

#include <domain/mesh/element/utils/Information.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/recorder/response/ElementResponse.h>
#include <domain/load/ElementalLoad.h>
#include <domain/mesh/node/Node.h>
#include <cstring>

const int XC::TotalLagrangianFD20NodeBrick::NumIntegrationPts = 3;
const int XC::TotalLagrangianFD20NodeBrick::NumTotalGaussPts = 27;
const int XC::TotalLagrangianFD20NodeBrick::NumNodes = 20;
const int XC::TotalLagrangianFD20NodeBrick::NumDof = 3;
const int XC::TotalLagrangianFD20NodeBrick::NumElemDof = NumNodes*NumDof;

 XC::Matrix XC::TotalLagrangianFD20NodeBrick::K(NumElemDof, NumElemDof);
 XC::Matrix XC::TotalLagrangianFD20NodeBrick::M(NumElemDof, NumElemDof);
 XC::Vector XC::TotalLagrangianFD20NodeBrick::P(NumElemDof);
const double XC::TotalLagrangianFD20NodeBrick::pts[3] = {-0.774596669241483, 0.0, +0.774596669241483};
const double XC::TotalLagrangianFD20NodeBrick::wts[3] = {+0.55555555555555556, +0.88888888888888889, +0.55555555555555556};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

XC::TotalLagrangianFD20NodeBrick::TotalLagrangianFD20NodeBrick(int tag,
int node_numb_1,  int node_numb_2,  int node_numb_3,  int node_numb_4,
int node_numb_5,  int node_numb_6,  int node_numb_7,  int node_numb_8,
int node_numb_9,  int node_numb_10, int node_numb_11, int node_numb_12,
int node_numb_13, int node_numb_14, int node_numb_15, int node_numb_16,
int node_numb_17, int node_numb_18, int node_numb_19, int node_numb_20,

XC::NDMaterial &m, const BodyForces3D &bForces)
  :ElemWithMaterial<20,NDMaterialPhysicalProperties>(tag, ELE_TAG_TotalLagrangianFD20NodeBrick,NDMaterialPhysicalProperties(27,&m)),
  bf(bForces), Ki(nullptr)
  {
    theNodes.set_id_nodes(node_numb_1,node_numb_2,node_numb_3,node_numb_4,node_numb_5,node_numb_6,node_numb_7,node_numb_8,node_numb_9,node_numb_10,node_numb_11,node_numb_12,node_numb_13,node_numb_14,node_numb_15,node_numb_16,node_numb_17,node_numb_18,node_numb_19,node_numb_20);

      rho = m.getRho();
  }

//-------------------------------------------------------------------------------------------
XC::TotalLagrangianFD20NodeBrick::TotalLagrangianFD20NodeBrick(void)
  :ElemWithMaterial<20,NDMaterialPhysicalProperties>(0, ELE_TAG_TotalLagrangianFD20NodeBrick,NDMaterialPhysicalProperties(27,nullptr)),bf(NumDof), Ki(nullptr)
  {
     bf(0) = 0.0;
     bf(1) = 0.0;
     bf(2) = 0.0;
     rho = 0.0;
  }
//! @brief Virtual constructor.
XC::Element* XC::TotalLagrangianFD20NodeBrick::getCopy(void) const
  { return new TotalLagrangianFD20NodeBrick(*this); }


//-------------------------------------------------------------------------------------------------
XC::TotalLagrangianFD20NodeBrick::~TotalLagrangianFD20NodeBrick(void)
  {
    if(Ki) delete Ki;
  }


//=============================================================================
int XC::TotalLagrangianFD20NodeBrick::getNumDOF(void) const
  { return NumElemDof; }

//=============================================================================
void XC::TotalLagrangianFD20NodeBrick::setDomain(Domain *theDomain)
  {
    ElemWithMaterial<20,NDMaterialPhysicalProperties>::setDomain(theDomain);
    theNodes.checkNumDOF(NumDof,getTag());
  }


//=============================================================================
int XC::TotalLagrangianFD20NodeBrick::update(void)
{
    int ret = 0;
    BJtensor dh;
    BJtensor dH_dX;
    int where = 0;
    int GP_c_r, GP_c_s, GP_c_t;
    double r = 0.0;
    double s = 0.0;
    double t = 0.0;
    BJtensor I_ij("I", 2, def_dim_2);
    BJtensor currentF;
    BJtensor updatedF;

    BJtensor InitialNodesCrds = this->getNodesCrds();
    BJtensor CurrentNodesDisp = this->getNodesDisp();

    for( GP_c_r = 0 ; GP_c_r < NumIntegrationPts ; GP_c_r++ ) {
      r = pts[GP_c_r ];
      for( GP_c_s = 0 ; GP_c_s < NumIntegrationPts ; GP_c_s++ ) {
        s = pts[GP_c_s ];
        for( GP_c_t = 0 ; GP_c_t < NumIntegrationPts ; GP_c_t++ ) {
          t = pts[GP_c_t ];
          where =(GP_c_r * NumIntegrationPts + GP_c_s) * NumIntegrationPts + GP_c_t;
          dh = shapeFunctionDerivative(r,s,t);
          dH_dX = this->dh_Global(dh);
          currentF = CurrentNodesDisp("Ia") * dH_dX("Ib");
            currentF.null_indices();
          updatedF = currentF + I_ij;
          ret += physicalProperties[where]->setTrialF(updatedF);
        }
      }
    }
    return ret;
}

//======================================================================
 XC::BJtensor XC::TotalLagrangianFD20NodeBrick::Jacobian_3D(BJtensor dh) const
{
     BJtensor N_C = this->getNodesCrds();
     BJtensor J3D = dh("ij") * N_C("ik");
       J3D.null_indices();
     return J3D;
}

//======================================================================
 XC::BJtensor XC::TotalLagrangianFD20NodeBrick::Jacobian_3Dinv(BJtensor dh) const
{
     BJtensor N_C = this->getNodesCrds();
     BJtensor J3D = dh("ij") * N_C("ik");
       J3D.null_indices();
     BJtensor J3Dinv = J3D.inverse();
     return J3Dinv;
}

//======================================================================
XC::BJtensor XC::TotalLagrangianFD20NodeBrick::dh_Global(BJtensor dh) const
  {
     BJtensor  JacobianINV0 = this->Jacobian_3Dinv(dh);
     BJtensor  dhGlobal_0 = dh("ij") * JacobianINV0("kj");
       dhGlobal_0.null_indices();
     return dhGlobal_0;
  }

//======================================================================
XC::BJtensor XC::TotalLagrangianFD20NodeBrick::getStiffnessTensor(void) const
  {
    BJtensor tI2("I", 2, def_dim_2);

    int K_dim[] = {NumNodes, NumDof, NumDof, NumNodes};
    BJtensor Kk(4,K_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    int where = 0;
    int GP_c_r, GP_c_s, GP_c_t;
    double weight = 0.0;

    int dh_dim[] = {NumNodes, NumDof};
    BJtensor dh(2, dh_dim, 0.0);
    stresstensor PK2Stress;
    BJtensor L2;

    double det_of_Jacobian = 0.0;

    BJtensor Jacobian;
    BJtensor dhGlobal;
    BJtensor nodesDisp;
    BJtensor F;
    //BJtensor temp01;
    BJtensor temp02;
    BJtensor temp03;
    BJtensor temp04;
    BJtensor temp05;
    BJtensor temp06;

    nodesDisp = this->getNodesDisp( );

    for(GP_c_r = 0 ; GP_c_r < NumIntegrationPts ; GP_c_r++ )
      {
        r = pts[GP_c_r ];
        rw = wts[GP_c_r ];
        for( GP_c_s = 0 ; GP_c_s < NumIntegrationPts ; GP_c_s++ )
          {
            s = pts[GP_c_s ];
            sw = wts[GP_c_s ];
            for( GP_c_t = 0 ; GP_c_t < NumIntegrationPts ; GP_c_t++ )
              {
                t = pts[GP_c_t ];
                tw = wts[GP_c_t ];
                where =(GP_c_r * NumIntegrationPts + GP_c_s) * NumIntegrationPts + GP_c_t;
                dh = shapeFunctionDerivative(r,s,t);
                Jacobian = this->Jacobian_3D(dh);
                det_of_Jacobian  = Jacobian.determinant();
                dhGlobal = this->dh_Global(dh);
                weight = rw * sw * tw * det_of_Jacobian;
                PK2Stress = physicalProperties[where]->getStressTensor();
                L2= physicalProperties[where]->getTangentTensor();
                F= physicalProperties[where]->getF();

                //K1
                temp04 = dhGlobal("Pb") * tI2("mn");
                temp04.null_indices();
                temp02 = PK2Stress("bd") * dhGlobal("Qd");
                temp02.null_indices();
                temp06 = temp04("Pbmn") * temp02("bQ") * weight;
                temp06.null_indices();
                Kk += temp06;

                //K2
                temp03 =  dhGlobal("Pb") * F("ma");
                temp03.null_indices();
                temp04 = F("nc") * L2("abcd");
                temp04.null_indices();
                temp05 = temp04("nabd") * dhGlobal("Qd");
                temp05.null_indices();
                temp06 = temp03("Pbma") * temp05("nabQ") * weight;
                temp06.null_indices();
                Kk += temp06;
              }
            }
          }
    return Kk;
  }

//======================================================================
 XC::BJtensor XC::TotalLagrangianFD20NodeBrick::getRtensor(void) const
{
    int R_dim[] = {NumNodes, NumDof};
    BJtensor Rr(2,R_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    int where = 0;
    int GP_c_r, GP_c_s, GP_c_t;
    double weight = 0.0;

    int dh_dim[] = {NumNodes,NumDof};
    BJtensor dh(2, dh_dim, 0.0);

    double det_of_Jacobian = 0.0;

    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;
    BJtensor currentF;
    BJtensor nodesDisp;
    stresstensor PK2Stress;
    BJtensor temp01;
    BJtensor temp02;
    BJtensor F;

    nodesDisp = this->getNodesDisp( );

    for( GP_c_r = 0 ; GP_c_r < NumIntegrationPts ; GP_c_r++ ) {
          r = pts[GP_c_r ];
      rw = wts[GP_c_r ];
      for( GP_c_s = 0 ; GP_c_s < NumIntegrationPts ; GP_c_s++ ) {
        s = pts[GP_c_s ];
        sw = wts[GP_c_s ];
        for( GP_c_t = 0 ; GP_c_t < NumIntegrationPts ; GP_c_t++ ) {
          t = pts[GP_c_t ];
          tw = wts[GP_c_t ];
          where =(GP_c_r * NumIntegrationPts + GP_c_s) * NumIntegrationPts + GP_c_t;
          dh = shapeFunctionDerivative(r,s,t);
          Jacobian = this->Jacobian_3D(dh);
          det_of_Jacobian  = Jacobian.determinant();
          dhGlobal = this->dh_Global(dh);
          weight = rw * sw * tw * det_of_Jacobian;
          PK2Stress = physicalProperties[where]->getStressTensor();
          F = physicalProperties[where]->getF();
          temp01 = PK2Stress("ik") * F("jk");
            temp01.null_indices();
          temp02 = dhGlobal("Pj") * temp01("ij") * weight;
            temp02.null_indices();
          Rr += temp02;
        }
      }
    }

    return Rr;
}

//======================================================================
XC::BJtensor XC::TotalLagrangianFD20NodeBrick::getBodyForce(void) const
  {
    int B_dim[] = {NumNodes, NumDof};
    BJtensor Bb(2,B_dim,0.0);

    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

    int where = 0;
    int GP_c_r, GP_c_s, GP_c_t;
    double weight = 0.0;

    int h_dim[] = {20};
    BJtensor h(1, h_dim, 0.0);
    int dh_dim[] = {NumNodes,NumDof};
    BJtensor dh(2, dh_dim, 0.0);
    int bodyforce_dim[] = {3};
    BJtensor bodyforce(1, bodyforce_dim, 0.0);

    double det_of_Jacobian = 0.0;

    BJtensor Jacobian;
    BJtensor JacobianINV;

    bodyforce.val(1) = bf(0);
    bodyforce.val(2) = bf(1);
    bodyforce.val(3) = bf(2);

    for( GP_c_r = 0 ; GP_c_r < NumIntegrationPts ; GP_c_r++ ) {
      r = pts[GP_c_r ];
      rw = wts[GP_c_r ];
      for( GP_c_s = 0 ; GP_c_s < NumIntegrationPts ; GP_c_s++ ) {
        s = pts[GP_c_s ];
        sw = wts[GP_c_s ];
        for( GP_c_t = 0 ; GP_c_t < NumIntegrationPts ; GP_c_t++ ) {
          t = pts[GP_c_t ];
          tw = wts[GP_c_t ];
          where =(GP_c_r * NumIntegrationPts + GP_c_s) * NumIntegrationPts + GP_c_t;
          h = shapeFunction(r,s,t);
          dh = shapeFunctionDerivative(r,s,t);
          Jacobian = this->Jacobian_3D(dh);
          det_of_Jacobian  = Jacobian.determinant();
          weight = rw * sw * tw * det_of_Jacobian;
          Bb = Bb +  h("P") * bodyforce("i") * rho *weight;
            Bb.null_indices();
        }
      }
    }
    return Bb;
  }

//======================================================================
XC::BJtensor XC::TotalLagrangianFD20NodeBrick::getSurfaceForce(void) const
  {
    int S_dim[] = {NumNodes, NumDof};
    BJtensor Ss(2,S_dim,0.0);
    // Need Work Here!
    return Ss;
  }

//============================================================================
XC::BJtensor XC::TotalLagrangianFD20NodeBrick::getForces(void) const
  {
    int F_dim[] = {NumNodes,NumDof};
    BJtensor Ff(2,F_dim,0.0);
    Ff = this->getBodyForce( ) + this->getSurfaceForce( );
    return Ff;
  }

//=============================================================================
const XC::Matrix &XC::TotalLagrangianFD20NodeBrick::getTangentStiff(void) const
{
     K.Zero();

     BJtensor stifftensor = this->getStiffnessTensor();

     int kki=0;
     int kkj=0;

     int i, j, k, l;
     for(i=1 ; i<=NumNodes ; i++ ) {
        for(j=1 ; j<=NumNodes ; j++ ) {
           for(k=1 ; k<=NumDof ; k++ ) {
              for(l=1 ; l<=NumDof ; l++ ) {
                 kki = k + NumDof*(i-1);
                 kkj = l + NumDof*(j-1);
                 K(kki-1 , kkj-1) = stifftensor.cval(i,k,l,j);
              }
           }
        }
     }
    if(isDead())
      K*=dead_srf;
    return K;
  }

//=============================================================================
const XC::Matrix &XC::TotalLagrangianFD20NodeBrick::getInitialStiff(void) const
  {
    if(Ki != 0) return *Ki;

    K.Zero();
    K = this->getTangentStiff ();

    Ki = new Matrix(K);
    if(isDead())
      K*=dead_srf;
    return K;
  }

//=============================================================================
const XC::Matrix &XC::TotalLagrangianFD20NodeBrick::getMass(void) const
  {
    // Need Work Here
    M.Zero();
    return M;
  }

//======================================================================
 XC::BJtensor XC::TotalLagrangianFD20NodeBrick::getNodesCrds(void) const
{
    const int dimensions[] = {NumNodes, NumDof};
    BJtensor N_coord(2, dimensions, 0.0);

    int i, j;
    for(i=0; i<NumNodes; i++) {
          const XC::Vector &TNodesCrds = theNodes[i]->getCrds();
      for(j=0; j<NumDof; j++) {
        N_coord.val(i+1, j+1) = TNodesCrds(j);
          }
    }

    return N_coord;
}

//=============================================================================================
 XC::BJtensor XC::TotalLagrangianFD20NodeBrick::getNodesDisp(void) const
{
    int i, j;
    int dimU[] = {NumNodes, NumDof};
    BJtensor total_disp(2, dimU, 0.0);

    for(i=0; i<NumNodes; i++) {
      const XC::Vector &TNodesDisp = theNodes[i]->getTrialDisp();
      for(j=0; j<NumDof; j++) {
        total_disp.val(i+1, j+1) = TNodesDisp(j);
      }
    }

    return total_disp;
}

//=============================================================================
int
XC::TotalLagrangianFD20NodeBrick::addLoad(ElementalLoad *theLoad, double loadFactor)
{
    std::cerr<<"XC::TotalLagrangianFD20NodeBrick::addLoad - load type unknown for ele with tag: "<<this->getTag();
    return -1;
}

//=============================================================================
int XC::TotalLagrangianFD20NodeBrick::addInertiaLoadToUnbalance(const XC::Vector &accel)
{
    // Check for a quick return
    if(rho == 0.0) return 0;

    static XC::Vector ra(NumElemDof);
    int i, j;

    for(i=0; i<NumNodes; i++) {
      const XC::Vector &RA = theNodes[i]->getRV(accel);
      if( RA.Size() != NumDof ) {
        std::cerr << "XC::TotalLagrangianFD20NodeBrick::addInertiaLoadToUnbalance(): matrix and vector sizes are incompatable \n";
        return (-1);
      }

      for(j=0; j<NumDof; j++) {
        ra(i*NumDof +j) = RA(j);
      }

    }

    this->getMass();

    if(load.isEmpty())
      load.reset(NumElemDof);

    load.addMatrixVector(1.0, M, ra, -1.0);

    return 0;

}


//=============================================================================
const XC::Vector &XC::TotalLagrangianFD20NodeBrick::getResistingForce(void) const
  {
    int i, j;
    int f_dim[] = {NumNodes, NumDof};
    BJtensor NodalForces_in(2, f_dim, 0.0);
    NodalForces_in = this->getRtensor() - this->getForces();

    for(i=0; i<NumNodes; i++)
      {
        for(j=0; j<NumDof; j++)
          { P(i*NumDof +j) = NodalForces_in.cval(i+1, j+1); }
      }
    if(!load.isEmpty())
      P.addVector(1.0, load, -1.0);
    if(isDead())
      P*=dead_srf;
    return P;
  }

//=============================================================================
const XC::Vector &XC::TotalLagrangianFD20NodeBrick::getResistingForceIncInertia(void) const
  {
    int i, j;
    Vector a(NumElemDof);

    this->getResistingForce();

    if(rho != 0.0)
    {
      for(i=0; i<NumNodes; i++) {
        const XC::Vector &acc = theNodes[i]->getTrialAccel();
        if( acc.Size() != NumDof ) {
          std::cerr << "XC::TotalLagrangianFD20NodeBrick::getResistingForceIncInertia matrix and vector sizes are incompatable \n";
          exit(-1);
        }
       for(j=0; j<NumDof; j++) {
         a(i*NumDof +j) = acc(j);
      }
    }

    this->getMass();
    P.addMatrixVector(1.0, M, a, 1.0);

  }
    if(isDead())
      P*=dead_srf;
    return P;
  }

//=============================================================================
int XC::TotalLagrangianFD20NodeBrick::sendSelf(CommParameters &cp)
{
     // Not implemtented yet
     return 0;
}

//=============================================================================
int XC::TotalLagrangianFD20NodeBrick::recvSelf(const CommParameters &cp)
{
     // Not implemtented yet
     return 0;
}


//=============================================================================
void XC::TotalLagrangianFD20NodeBrick::Print(std::ostream &s, int flag)
{
    s << "\nTotalLagrangianFD20NodeBrick, element id:  " << this->getTag() << std::endl;
    s << "\nConnected external nodes:  " << theNodes;
    s << "\nBody forces:  " << bf << std::endl;

    physicalProperties[0]->Print(s,flag);

    BJtensor sigma;
    Vector P00(6);

    int i;
    for(i=0; i<NumTotalGaussPts; i++)
    {
      sigma = physicalProperties[i]->getCauchyStressTensor();
      P00(0) = sigma.val(1,1);
      P00(1) = sigma.val(2,2);
      P00(2) = sigma.val(3,3);
      P00(3) = sigma.val(2,3);
      P00(4) = sigma.val(3,1);
      P00(5) = sigma.val(1,2);

      s << "\n where = " << i << std::endl;
      s << " Stress (Cauchy): xx yy zz yz zx xy) " << P00 << std::endl;
    }
}

//=============================================================================
 XC::Response * XC::TotalLagrangianFD20NodeBrick::setResponse(const std::vector<std::string> &argv, Information &eleInfo)
{
    if(argv[0] == "force" || argv[0] == "forces")
      return new ElementResponse(this, 1, Vector(NumElemDof));

    else if(argv[0] == "stiff" || argv[0] == "stiffness")
      return new ElementResponse(this, 2, Matrix(NumElemDof, NumElemDof));

    else if(argv[0] == "CauchyStress" || argv[0] == "stress")
      return new ElementResponse(this, 3, Vector(NumTotalGaussPts*6));

    else if(argv[0] == "PK2Stress" || argv[0] == "PK2stress")
      return new ElementResponse(this, 4, Vector(NumTotalGaussPts*6));

    // Added ZC 01/18/2005 to output strains
    else if(argv[0] == "EulerianStrain" || argv[0] == "strain")
      return new ElementResponse(this, 5, Vector(NumTotalGaussPts*6));

    else if(argv[0] == "LagrangianStrain" || argv[0] == "iniStrain")
      return new ElementResponse(this, 6, Vector(NumTotalGaussPts*6));

    else
      return 0;
}

//=============================================================================
int XC::TotalLagrangianFD20NodeBrick::getResponse (int responseID, Information &eleInfo)
{
    int i;
        static XC::Vector P0(NumTotalGaussPts*6);

     switch (responseID) {

     case 1:
          return eleInfo.setVector(this->getResistingForce() );

     case 2:
          return eleInfo.setMatrix(this->getTangentStiff() );

     case 3: {
        Vector P0(NumTotalGaussPts*6);
        BJtensor sigma;
        for(i=0; i<NumTotalGaussPts; i++) {
          sigma = physicalProperties[i]->getCauchyStressTensor();
          P0(i*6 +0 ) = sigma.val(1,1);
          P0(i*6 +1 ) = sigma.val(2,2);
          P0(i*6 +2 ) = sigma.val(3,3);
          P0(i*6 +3 ) = sigma.val(2,3);
          P0(i*6 +4 ) = sigma.val(3,1);
          P0(i*6 +5 ) = sigma.val(1,2);
        }
        return eleInfo.setVector(P0);
     }

     case 4: {
        Vector P0(NumTotalGaussPts*6);
        BJtensor sigma;
        for(i=0; i<NumTotalGaussPts; i++) {
          sigma = physicalProperties[i]->getStressTensor();
          P0(i*6 +0 ) = sigma.val(1,1);
          P0(i*6 +1 ) = sigma.val(2,2);
          P0(i*6 +2 ) = sigma.val(3,3);
          P0(i*6 +3 ) = sigma.val(2,3);
          P0(i*6 +4 ) = sigma.val(3,1);
          P0(i*6 +5 ) = sigma.val(1,2);
        }
        return eleInfo.setVector(P0);
     }

    // Added ZC 01/18/2005 to output strains
     case 5: {
        Vector P0(NumTotalGaussPts*6);
        BJtensor e;
        BJtensor E;
        BJtensor F;
        BJtensor tI2("I", 2, def_dim_2);
        for(i=0; i<NumTotalGaussPts; i++) {
          E = physicalProperties[i]->getStrainTensor();
          F = physicalProperties[i]->getF();
          F = F.inverse();
          e = F("ki")*F("kj"); e.null_indices();
          e = (tI2-e) *0.5;
          P0(i*6 +0 ) = e.val(1,1);
          P0(i*6 +1 ) = e.val(2,2);
          P0(i*6 +2 ) = e.val(3,3);
          P0(i*6 +3 ) = e.val(2,3);
          P0(i*6 +4 ) = e.val(3,1);
          P0(i*6 +5 ) = e.val(1,2);
        }
        return eleInfo.setVector(P0);
     }

     case 6: {
        Vector P0(NumTotalGaussPts*6);
        BJtensor E;
        for(i=0; i<NumTotalGaussPts; i++) {
          E = physicalProperties[i]->getStrainTensor();
          P0(i*6 +0 ) = E.val(1,1);
          P0(i*6 +1 ) = E.val(2,2);
          P0(i*6 +2 ) = E.val(3,3);
          P0(i*6 +3 ) = E.val(2,3);
          P0(i*6 +4 ) = E.val(3,1);
          P0(i*6 +5 ) = E.val(1,2);
        }
        return eleInfo.setVector(P0);
     }

     default:
        return -1;
   }
}


//#############################################################################
//===================================================================
XC::BJtensor XC::TotalLagrangianFD20NodeBrick::shapeFunction(double r1, double r2, double r3)
  {

    int dimension[] = {NumNodes};
    BJtensor h(1, dimension, 0.0);

    // influence of the node number 20
        h.val(20)=(1.0+r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 19
        h.val(19)=(1.0-r1)*(1.0-r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 18
        h.val(18)=(1.0-r1)*(1.0+r2)*(1.0-r3*r3)*0.25;
    // influence of the node number 17
        h.val(17)=(1.0+r1)*(1.0+r2)*(1.0-r3*r3)*0.25;

    // influence of the node number 16
        h.val(16)=(1.0+r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
    // influence of the node number 15
        h.val(15)=(1.0-r1*r1)*(1.0-r2)*(1.0-r3)*0.25;
    // influence of the node number 14
        h.val(14)=(1.0-r1)*(1.0-r2*r2)*(1.0-r3)*0.25;
    // influence of the node number 13
        h.val(13)=(1.0-r1*r1)*(1.0+r2)*(1.0-r3)*0.25;

    // influence of the node number 12
        h.val(12)=(1.0+r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
    // influence of the node number 11
        h.val(11)=(1.0-r1*r1)*(1.0-r2)*(1.0+r3)*0.25;
    // influence of the node number 10
        h.val(10)=(1.0-r1)*(1.0-r2*r2)*(1.0+r3)*0.25;
    // influence of the node number 9
        h.val( 9)=(1.0-r1*r1)*(1.0+r2)*(1.0+r3)*0.25;

      // influence of the node number 8
    h.val(8)=(1.0+r1)*(1.0-r2)*(1.0-r3)*0.125 - (h.val(15)+h.val(16)+h.val(20))*0.5;
      // influence of the node number 7
    h.val(7)=(1.0-r1)*(1.0-r2)*(1.0-r3)*0.125 - (h.val(14)+h.val(15)+h.val(19))*0.5;
      // influence of the node number 6
    h.val(6)=(1.0-r1)*(1.0+r2)*(1.0-r3)*0.125 - (h.val(13)+h.val(14)+h.val(18))*0.5;
      // influence of the node number 5
    h.val(5)=(1.0+r1)*(1.0+r2)*(1.0-r3)*0.125 - (h.val(13)+h.val(16)+h.val(17))*0.5;

      // influence of the node number 4
    h.val(4)=(1.0+r1)*(1.0-r2)*(1.0+r3)*0.125 - (h.val(11)+h.val(12)+h.val(20))*0.5;
      // influence of the node number 3
    h.val(3)=(1.0-r1)*(1.0-r2)*(1.0+r3)*0.125 - (h.val(10)+h.val(11)+h.val(19))*0.5;
      // influence of the node number 2
    h.val(2)=(1.0-r1)*(1.0+r2)*(1.0+r3)*0.125 - (h.val(10)+h.val(18)+h.val( 9))*0.5;
      // influence of the node number 1
    h.val(1)=(1.0+r1)*(1.0+r2)*(1.0+r3)*0.125 - (h.val(12)+h.val(17)+h.val( 9))*0.5;

    return h;
}


//==============================================================
 XC::BJtensor XC::TotalLagrangianFD20NodeBrick::shapeFunctionDerivative(double r1, double r2, double r3)
{

    int dimensions[] = {NumNodes, NumDof};
    BJtensor dh(2, dimensions, 0.0);

    // influence of the node number 20
        dh.val(20,1) =   (1.0-r2)*(1.0-r3*r3)*0.25;
        dh.val(20,2) = - (1.0+r1)*(1.0-r3*r3)*0.25;
        dh.val(20,3) = - (1.0+r1)*(1.0-r2)*r3*0.50;
    // influence of the node number 19
        dh.val(19,1) = - (1.0-r2)*(1.0-r3*r3)*0.25;
        dh.val(19,2) = - (1.0-r1)*(1.0-r3*r3)*0.25;
        dh.val(19,3) = - (1.0-r1)*(1.0-r2)*r3*0.50;
    // influence of the node number 18
        dh.val(18,1) = - (1.0+r2)*(1.0-r3*r3)*0.25;
        dh.val(18,2) =   (1.0-r1)*(1.0-r3*r3)*0.25;
        dh.val(18,3) = - (1.0-r1)*(1.0+r2)*r3*0.50;
    // influence of the node number 17
        dh.val(17,1) =   (1.0+r2)*(1.0-r3*r3)*0.25;
        dh.val(17,2) =   (1.0+r1)*(1.0-r3*r3)*0.25;
        dh.val(17,3) = - (1.0+r1)*(1.0+r2)*r3*0.50;

    // influence of the node number 16
        dh.val(16,1) =   (1.0-r2*r2)*(1.0-r3)*0.25;
        dh.val(16,2) = - (1.0+r1)*r2*(1.0-r3)*0.50;
        dh.val(16,3) = - (1.0+r1)*(1.0-r2*r2)*0.25;
    // influnce of the node number 15
        dh.val(15,1) = - r1*(1.0-r2)*(1.0-r3)*0.50;
        dh.val(15,2) = - (1.0-r1*r1)*(1.0-r3)*0.25;
        dh.val(15,3) = - (1.0-r1*r1)*(1.0-r2)*0.25;
    // influence of the node number 14
        dh.val(14,1) = - (1.0-r2*r2)*(1.0-r3)*0.25;
        dh.val(14,2) = - (1.0-r1)*r2*(1.0-r3)*0.50;
        dh.val(14,3) = - (1.0-r1)*(1.0-r2*r2)*0.25;
    // influence of the node number 13
        dh.val(13,1) = - r1*(1.0+r2)*(1.0-r3)*0.50;
        dh.val(13,2) =   (1.0-r1*r1)*(1.0-r3)*0.25;
        dh.val(13,3) = - (1.0-r1*r1)*(1.0+r2)*0.25;

    // influence of the node number 12
        dh.val(12,1) =   (1.0-r2*r2)*(1.0+r3)*0.25;
        dh.val(12,2) = - (1.0+r1)*r2*(1.0+r3)*0.50;
        dh.val(12,3) =   (1.0+r1)*(1.0-r2*r2)*0.25;
    // influence of the node number 11
        dh.val(11,1) = - r1*(1.0-r2)*(1.0+r3)*0.50;
        dh.val(11,2) = - (1.0-r1*r1)*(1.0+r3)*0.25;
        dh.val(11,3) =   (1.0-r1*r1)*(1.0-r2)*0.25;
    // influence of the node number 10
        dh.val(10,1) = - (1.0-r2*r2)*(1.0+r3)*0.25;
        dh.val(10,2) = - (1.0-r1)*r2*(1.0+r3)*0.50;
        dh.val(10,3) =   (1.0-r1)*(1.0-r2*r2)*0.25;
    // influence of the node number 9
        dh.val(9,1)  = - r1*(1.0+r2)*(1.0+r3)*0.50;
        dh.val(9,2)  =   (1.0-r1*r1)*(1.0+r3)*0.25;
        dh.val(9,3)  =   (1.0-r1*r1)*(1.0+r2)*0.25;

      // influence of the node number 8
    dh.val(8,1)= (1.0-r2)*(1.0-r3)*0.125 - (dh.val(15,1)+dh.val(16,1)+dh.val(20,1))*0.5;
    dh.val(8,2)=-(1.0+r1)*(1.0-r3)*0.125 - (dh.val(15,2)+dh.val(16,2)+dh.val(20,2))*0.5;
    dh.val(8,3)=-(1.0+r1)*(1.0-r2)*0.125 - (dh.val(15,3)+dh.val(16,3)+dh.val(20,3))*0.5;
      // influence of the node number 7
    dh.val(7,1)=-(1.0-r2)*(1.0-r3)*0.125 - (dh.val(14,1)+dh.val(15,1)+dh.val(19,1))*0.5;
    dh.val(7,2)=-(1.0-r1)*(1.0-r3)*0.125 - (dh.val(14,2)+dh.val(15,2)+dh.val(19,2))*0.5;
    dh.val(7,3)=-(1.0-r1)*(1.0-r2)*0.125 - (dh.val(14,3)+dh.val(15,3)+dh.val(19,3))*0.5;
      // influence of the node number 6
    dh.val(6,1)=-(1.0+r2)*(1.0-r3)*0.125 - (dh.val(13,1)+dh.val(14,1)+dh.val(18,1))*0.5;
    dh.val(6,2)= (1.0-r1)*(1.0-r3)*0.125 - (dh.val(13,2)+dh.val(14,2)+dh.val(18,2))*0.5;
    dh.val(6,3)=-(1.0-r1)*(1.0+r2)*0.125 - (dh.val(13,3)+dh.val(14,3)+dh.val(18,3))*0.5;
      // influence of the node number 5
    dh.val(5,1)= (1.0+r2)*(1.0-r3)*0.125 - (dh.val(13,1)+dh.val(16,1)+dh.val(17,1))*0.5;
    dh.val(5,2)= (1.0+r1)*(1.0-r3)*0.125 - (dh.val(13,2)+dh.val(16,2)+dh.val(17,2))*0.5;
    dh.val(5,3)=-(1.0+r1)*(1.0+r2)*0.125 - (dh.val(13,3)+dh.val(16,3)+dh.val(17,3))*0.5;

      // influence of the node number 4
    dh.val(4,1)= (1.0-r2)*(1.0+r3)*0.125 - (dh.val(11,1)+dh.val(12,1)+dh.val(20,1))*0.5;
    dh.val(4,2)=-(1.0+r1)*(1.0+r3)*0.125 - (dh.val(11,2)+dh.val(12,2)+dh.val(20,2))*0.5;
    dh.val(4,3)= (1.0+r1)*(1.0-r2)*0.125 - (dh.val(11,3)+dh.val(12,3)+dh.val(20,3))*0.5;
      // influence of the node number 3
    dh.val(3,1)=-(1.0-r2)*(1.0+r3)*0.125 - (dh.val(10,1)+dh.val(11,1)+dh.val(19,1))*0.5;
    dh.val(3,2)=-(1.0-r1)*(1.0+r3)*0.125 - (dh.val(10,2)+dh.val(11,2)+dh.val(19,2))*0.5;
    dh.val(3,3)= (1.0-r1)*(1.0-r2)*0.125 - (dh.val(10,3)+dh.val(11,3)+dh.val(19,3))*0.5;
      // influence of the node number 2
    dh.val(2,1)=-(1.0+r2)*(1.0+r3)*0.125 - (dh.val(10,1)+dh.val(18,1)+dh.val( 9,1))*0.5;
    dh.val(2,2)= (1.0-r1)*(1.0+r3)*0.125 - (dh.val(10,2)+dh.val(18,2)+dh.val( 9,2))*0.5;
    dh.val(2,3)= (1.0-r1)*(1.0+r2)*0.125 - (dh.val(10,3)+dh.val(18,3)+dh.val( 9,3))*0.5;
      // influence of the node number 1
    dh.val(1,1)= (1.0+r2)*(1.0+r3)*0.125 - (dh.val(12,1)+dh.val(17,1)+dh.val( 9,1))*0.5;
    dh.val(1,2)= (1.0+r1)*(1.0+r3)*0.125 - (dh.val(12,2)+dh.val(17,2)+dh.val( 9,2))*0.5;
    dh.val(1,3)= (1.0+r1)*(1.0+r2)*0.125 - (dh.val(12,3)+dh.val(17,3)+dh.val( 9,3))*0.5;

    return dh;
}


