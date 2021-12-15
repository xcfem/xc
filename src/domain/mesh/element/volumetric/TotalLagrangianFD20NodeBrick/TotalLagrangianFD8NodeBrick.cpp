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
//# PROJECT:           Object Oriented Finite Element Program
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
//# DATE:              Sept2005              
//# UPDATE HISTORY:   
//#                     
//#                    
//#
//===============================================================================

#ifndef TOTALLAGRANGIANFD8NODEBRICK_CPP

#define TOTALLAGRANGIANFD8NODEBRICK_CPP



#include "TotalLagrangianFD8NodeBrick.h"
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/ElementResponse.h>
#include <utility/matrix/nDarray/BJtensor.h>
#include <utility/matrix/nDarray/stresst.h>
#include <utility/matrix/nDarray/straint.h>
#include <material/nD/FiniteDeformation/FiniteDeformationMaterial.h>
#include <domain/mesh/node/Node.h>

const int XC::TotalLagrangianFD8NodeBrick::NumIntegrationPts = 2;
const int XC::TotalLagrangianFD8NodeBrick::NumTotalGaussPts = 8;
const int XC::TotalLagrangianFD8NodeBrick::NumNodes = 8;
const int XC::TotalLagrangianFD8NodeBrick::NumDof = 3;
const int XC::TotalLagrangianFD8NodeBrick::NumElemDof= NumNodes*NumDof;



XC::Matrix XC::TotalLagrangianFD8NodeBrick::K(NumElemDof, NumElemDof);

XC::Matrix XC::TotalLagrangianFD8NodeBrick::M(NumElemDof, NumElemDof);

XC::Vector XC::TotalLagrangianFD8NodeBrick::P(NumElemDof);

const double XC::TotalLagrangianFD8NodeBrick::pts[2] = {-0.577350269189626, +0.577350269189626};

const double XC::TotalLagrangianFD8NodeBrick::wts[2] = {1.0, 1.0};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//! @brief Constructor.
XC::TotalLagrangianFD8NodeBrick::TotalLagrangianFD8NodeBrick(int tag,
int node_numb_1,  int node_numb_2,  int node_numb_3,  int node_numb_4,
int node_numb_5,  int node_numb_6,  int node_numb_7,  int node_numb_8,
NDMaterial &m, const BodyForces3D &bForces)
  : ElemWithMaterial<8,NDMaterialPhysicalProperties>(tag, ELE_TAG_TotalLagrangianFD8NodeBrick, NDMaterialPhysicalProperties(8,&m)),
  Q(nullptr), bf(bForces), Ki(nullptr)
  {
    theNodes.set_id_nodes(node_numb_1,node_numb_2,node_numb_3,node_numb_4,node_numb_5,node_numb_6,node_numb_7,node_numb_8);
  }


//-------------------------------------------------------------------------------------------

XC::TotalLagrangianFD8NodeBrick::TotalLagrangianFD8NodeBrick ()

:ElemWithMaterial<8,NDMaterialPhysicalProperties>(0, ELE_TAG_TotalLagrangianFD8NodeBrick,NDMaterialPhysicalProperties(27,nullptr)), Q(0),bf(NumDof), Ki(nullptr)
  {    

     bf(0) = 0.0;
     bf(1) = 0.0;
     bf(2) = 0.0;
  }

//! @brief Virtual constructor.
XC::Element* XC::TotalLagrangianFD8NodeBrick::getCopy(void) const
  { return new TotalLagrangianFD8NodeBrick(*this); }

//-------------------------------------------------------------------------------------------------
XC::TotalLagrangianFD8NodeBrick::~TotalLagrangianFD8NodeBrick(void)
  {
    if(Ki) delete Ki;
  }

int XC::TotalLagrangianFD8NodeBrick::getNumDOF(void) const
  { return NumElemDof; }

//=============================================================================
void XC::TotalLagrangianFD8NodeBrick::setDomain(Domain *theDomain)
  {
    ElemWithMaterial<8,NDMaterialPhysicalProperties>::setDomain(theDomain);
    theNodes.checkNumDOF(NumDof,getTag());
  }


//=============================================================================

int XC::TotalLagrangianFD8NodeBrick::commitState(void)

{

  int retVal = 0;



  if ((retVal = this->Element::commitState()) != 0)

    std::cerr << "XC::TotalLagrangianFD8NodeBrick::commitState () - failed in base class";



  int i;

  for (i=0; i<NumTotalGaussPts; i++)

    retVal += physicalProperties[i]->commitState();



  return retVal;

}


//=============================================================================

int XC::TotalLagrangianFD8NodeBrick::revertToLastCommit(void)

{

    int retVal = 0;

    

    int i;

    for (i=0; i<NumTotalGaussPts; i++)

       retVal += physicalProperties[i]->revertToLastCommit();



    return retVal;

}


//=============================================================================

int XC::TotalLagrangianFD8NodeBrick::revertToStart(void)

{

    int retVal = 0;

    

    int i;

    for (i=0; i<NumTotalGaussPts; i++)

       retVal += physicalProperties[i]->revertToStart();



    return retVal;



}


//=============================================================================

int XC::TotalLagrangianFD8NodeBrick::update(void)
  {

    int ret = 0;
    BJtensor dh;
    BJtensor dH_dX;
    int where = 0;

    int GP_c_r, GP_c_s, GP_c_t;

    double r = 0.0;
    double s = 0.0;
    double t = 0.0;

    BJtensor I_ij("I", def_dim_2);
    BJtensor currentF;
    BJtensor updatedF;

    BJtensor InitialNodesCrds = this->getNodesCrds();
    BJtensor CurrentNodesDisp = this->getNodesDisp();


    for( GP_c_r = 0 ; GP_c_r < NumIntegrationPts ; GP_c_r++ )
      {
        r = pts[GP_c_r ];
        for( GP_c_s = 0 ; GP_c_s < NumIntegrationPts ; GP_c_s++ )
	  {
            s = pts[GP_c_s ];
            for( GP_c_t = 0 ; GP_c_t < NumIntegrationPts ; GP_c_t++ )
	      {
                t = pts[GP_c_t ];
                where =(GP_c_r * NumIntegrationPts + GP_c_s) * NumIntegrationPts + GP_c_t;

		//dh = shapeFunctionDerivative(r,s,t);

		dH_dX = this->dh_Global(r,s,t);

		currentF = CurrentNodesDisp("Ia") * dH_dX("Ib");

		  currentF.null_indices();

		updatedF = currentF + I_ij;
		FiniteDeformationMaterial *fdMaterial= dynamic_cast<FiniteDeformationMaterial *>(physicalProperties[where]);
		const straintensor &cast_updatedF= static_cast<const straintensor &>(updatedF);

		ret += fdMaterial->setTrialF(cast_updatedF);
	      }
	  }

      }
    return ret;
  }


//======================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::Jacobian_3D(double x, double y, double z) const
  {

     BJtensor N_C = this->getNodesCrds();
     BJtensor dh = this->shapeFunctionDerivative(x, y, z);

     

     BJtensor J3D = N_C("ki") * dh("kj");

       J3D.null_indices();

     return J3D;

  }


//======================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::Jacobian_3Dinv(double x, double y, double z) const
  {

     BJtensor J = this->Jacobian_3D(x,y,z);

     return J.inverse();

  }


//======================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::dh_Global(double x, double y, double z) const
  {

     BJtensor JacobianINV0 = this->Jacobian_3Dinv(x, y, z);

     BJtensor dh = this->shapeFunctionDerivative(x, y, z);

     BJtensor  dhGlobal = dh("ik") * JacobianINV0("kj");

       dhGlobal.null_indices();

     return dhGlobal;

  }


//======================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::getStiffnessTensor(void) const
  {

    BJtensor tI2("I", def_dim_2);

	  

	std::vector<int> K_dim({NumNodes,NumDof,NumDof,NumNodes});

    BJtensor Kk(K_dim,0.0);



    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;

   

    int where = 0;
    int GP_c_r, GP_c_s, GP_c_t;
    double weight = 0.0;



    std::vector<int> dh_dim({NumNodes,NumDof});

    BJtensor dh(dh_dim, 0.0);

    stresstensor PK2Stress;

    BJtensor L2;



    double det_of_Jacobian = 0.0;



    BJtensor Jacobian;
    BJtensor dhGlobal;
    BJtensor nodesDisp;
    BJtensor F;

    //tensor temp01;

    BJtensor temp02;
    BJtensor temp03;
    BJtensor temp04; 
    BJtensor temp05;
    BJtensor temp06;



    nodesDisp = this->getNodesDisp( );



    for( GP_c_r = 0 ; GP_c_r < NumIntegrationPts ; GP_c_r++ )
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

	        //dh = shapeFunctionDerivative(r,s,t);

	        Jacobian= this->Jacobian_3D(r,s,t);
	        det_of_Jacobian= Jacobian.determinant();

	        dhGlobal= this->dh_Global(r,s,t);
	        weight= rw * sw * tw * det_of_Jacobian;
	        const FiniteDeformationMaterial *fdMaterial= dynamic_cast<const FiniteDeformationMaterial *>(physicalProperties[where]);

	        PK2Stress= fdMaterial->getStressTensor();
  	        L2 = fdMaterial->getTangentTensor();
	        F = fdMaterial->getF();


  	        //K1
                temp04 = dhGlobal("Pb") * tI2("mn");
		temp04.null_indices(); 
	        temp02 = PK2Stress("bd") * dhGlobal("Qd");
		temp02.null_indices();
	        temp06 = temp04("Pbmn") * temp02("bQ") * weight;
		temp06.null_indices(); 
  	        Kk += temp06;



	        //K2
	        temp03=  dhGlobal("Pb") * F("ma");
		temp03.null_indices(); 
	        temp04= F("nc") * L2("abcd");
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

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::getRtensor(void) const
  {

    std::vector<int> R_dim({NumNodes,NumDof});

    BJtensor Rr(R_dim,0.0);



    double r  = 0.0;
    double rw = 0.0;
    double s  = 0.0;
    double sw = 0.0;
    double t  = 0.0;
    double tw = 0.0;



    int where = 0;
    int GP_c_r, GP_c_s, GP_c_t;
    double weight = 0.0;



    std::vector<int> dh_dim({NumNodes,NumDof});
    BJtensor dh(dh_dim, 0.0);


    double det_of_Jacobian = 0.0;
    BJtensor Jacobian;
    BJtensor JacobianINV;
    BJtensor dhGlobal;
    BJtensor currentF;
    BJtensor nodesDisp;

    //stresstensor PK2Stress;

    BJtensor temp01;
    BJtensor temp02;
    BJtensor F;



    nodesDisp = this->getNodesDisp( );



    for( GP_c_r = 0 ; GP_c_r < NumIntegrationPts ; GP_c_r++ )
      {
	r= pts[GP_c_r ];
        rw= wts[GP_c_r ];

        for( GP_c_s = 0 ; GP_c_s < NumIntegrationPts ; GP_c_s++ )
	  {
            s= pts[GP_c_s ];
            sw= wts[GP_c_s ];

            for(GP_c_t = 0 ; GP_c_t < NumIntegrationPts ; GP_c_t++ )
	      {
                t= pts[GP_c_t ];
                tw = wts[GP_c_t ];

                where =(GP_c_r * NumIntegrationPts + GP_c_s) * NumIntegrationPts + GP_c_t;

                //dh = shapeFunctionDerivative(r,s,t);
                Jacobian= this->Jacobian_3D(r,s,t);
                det_of_Jacobian= Jacobian.determinant();
                dhGlobal= this->dh_Global(r,s,t);

                weight= rw * sw * tw * det_of_Jacobian;
	        const FiniteDeformationMaterial *fdMaterial= dynamic_cast<const FiniteDeformationMaterial *>(physicalProperties[where]);

                //PK2Stress = physicalProperties[where]->getStressTensor();
                //F = physicalProperties[where]->getF();
                //temp01 = PK2Stress("ik") * F("jk");
                //temp01.null_indices();

                temp01= fdMaterial->getPK1StressTensor();
                temp02= dhGlobal("PJ") * temp01("iJ") * weight;
                temp02.null_indices();
		
                Rr += temp02;
	      }
	  }

      }
    return Rr;
  }


//======================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::getBodyForce(void) const
  {

    std::vector<int> B_dim({NumNodes,NumDof});

    BJtensor Bb(B_dim,0.0);



    double r  = 0.0;

    double rw = 0.0;

    double s  = 0.0;

    double sw = 0.0;

    double t  = 0.0;

    double tw = 0.0;



    int where = 0;

    int GP_c_r, GP_c_s, GP_c_t;

    double weight = 0.0;



    std::vector<int> h_dim({20});

    BJtensor h(h_dim, 0.0);

    std::vector<int> dh_dim({NumNodes,NumDof});

    BJtensor dh(dh_dim, 0.0);

    std::vector<int> bodyforce_dim({3});

    BJtensor bodyforce(bodyforce_dim, 0.0);



    double det_of_Jacobian = 0.0;



    BJtensor Jacobian;

    BJtensor JacobianINV;



    bodyforce(1) = bf(0);

    bodyforce(2) = bf(1);

    bodyforce(3) = bf(2);



    const double rho= physicalProperties.getRho();
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

          Jacobian = this->Jacobian_3D(r,s,t);

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

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::getSurfaceForce(void) const
  {

    std::vector<int> S_dim({NumNodes, NumDof});

    BJtensor Ss(S_dim,0.0);

    // Need Work Here!



    return Ss;

  }


//============================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::getForces(void) const
  {

    std::vector<int> F_dim({NumNodes,NumDof});

    BJtensor Ff(F_dim,0.0);



    Ff = this->getBodyForce( ) + this->getSurfaceForce( );



    return Ff;

  }


//=============================================================================

const XC::Matrix &XC::TotalLagrangianFD8NodeBrick::getTangentStiff(void) const
  {

     K.Zero();



     BJtensor stifftensor = this->getStiffnessTensor();



     int kki=0;

     int kkj=0;

     

     int i, j, k, l;

     for (i=1 ; i<=NumNodes ; i++ ) {

        for (j=1 ; j<=NumNodes ; j++ ) {

           for (k=1 ; k<=NumDof ; k++ ) {

              for (l=1 ; l<=NumDof ; l++ ) {

                 kki = k + NumDof*(i-1);

                 kkj = l + NumDof*(j-1);

                 K(kki-1 , kkj-1) = stifftensor(i,k,l,j); 

              }

           }

        }

     }



     return K;

}


//=============================================================================

const XC::Matrix &XC::TotalLagrangianFD8NodeBrick::getInitialStiff(void) const
  {

     if (Ki != 0) return *Ki;



     K.Zero();

     K = this->getTangentStiff ();



     Ki = new Matrix(K);



     return K;

}


//=============================================================================

const XC::Matrix &XC::TotalLagrangianFD8NodeBrick::getMass(void) const

  {
    // Need Work Here
    M.Zero();
    return M;

  }


//======================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::getNodesCrds(void) const
  {

    const std::vector<int> dimensions({NumNodes, NumDof});

    BJtensor N_coord(dimensions, 0.0);



    int i, j;

    for (i=0; i<NumNodes; i++) {

	  const Vector &TNodesCrds = theNodes[i]->getCrds();

      for (j=0; j<NumDof; j++) {

        N_coord(i+1, j+1) = TNodesCrds(j);

	  }		    

    }

    

    return N_coord;

}


//=============================================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::getNodesDisp(void) const
  {

    int i, j;

    std::vector<int> dimU({NumNodes, NumDof});

    BJtensor total_disp(dimU, 0.0);



    for (i=0; i<NumNodes; i++) {

      const Vector &TNodesDisp = theNodes[i]->getTrialDisp();

      for (j=0; j<NumDof; j++) {

        total_disp(i+1, j+1) = TNodesDisp(j);

      }

    }



    return total_disp;

  }


//=============================================================================

void XC::TotalLagrangianFD8NodeBrick::zeroLoad(void)

{



	if ( Q != 0 )

	  Q->Zero();

    

	return;

}




//=============================================================================

int

XC::TotalLagrangianFD8NodeBrick::addLoad(ElementalLoad *theLoad, double loadFactor)

{

    std::cerr<<"XC::TotalLagrangianFD8NodeBrick::addLoad - load type unknown for ele with tag: "<<this->getTag();          

    return -1;

}


//=============================================================================

int XC::TotalLagrangianFD8NodeBrick::addInertiaLoadToUnbalance(const Vector &accel)

{

    // Check for a quick return

    const double rho= physicalProperties.getRho();
    if (rho == 0.0) return 0;



    static Vector ra(NumElemDof);

    int i, j;



    for (i=0; i<NumNodes; i++) {

      const Vector &RA = theNodes[i]->getRV(accel);

      if ( RA.Size() != NumDof ) {

        std::cerr << "XC::TotalLagrangianFD8NodeBrick::addInertiaLoadToUnbalance(): matrix and vector sizes are incompatable \n";

        return (-1);

      }

      

      for (j=0; j<NumDof; j++) {

	    ra(i*NumDof +j) = RA(j);

      }



    }



    this->getMass();



    if (Q == 0)  

      Q = new Vector(NumElemDof);



    Q->addMatrixVector(1.0, M, ra, -1.0);



    return 0;  
  }




//=============================================================================

const XC::Vector &XC::TotalLagrangianFD8NodeBrick::getResistingForce(void) const
  {   

	int i, j;

    std::vector<int> f_dim({NumNodes, NumDof});
    BJtensor NodalForces_in(f_dim, 0.0);

    NodalForces_in = this->getRtensor() - this->getForces();

    for (i=0; i<NumNodes; i++) {

      for (j=0; j<NumDof; j++) {

         P(i*NumDof +j) = NodalForces_in(i+1, j+1);

      }

    }

        

    if ( Q != 0 )

      P.addVector(1.0, *Q, -1.0);

    

    return P;

}


//=============================================================================

const XC::Vector &XC::TotalLagrangianFD8NodeBrick::getResistingForceIncInertia(void) const
  {

    int i, j;

    Vector a(NumElemDof);

    

    this->getResistingForce();



    const double rho= physicalProperties.getRho();
    if (rho != 0.0)

    {

      for (i=0; i<NumNodes; i++) {

        const Vector &acc = theNodes[i]->getTrialAccel();

        if ( acc.Size() != NumDof ) {

          std::cerr << "XC::TotalLagrangianFD8NodeBrick::getResistingForceIncInertia matrix and vector sizes are incompatable \n";

          exit(-1);

        }

      for (j=0; j<NumDof; j++) {

        a(i*NumDof +j) = acc(j);

      }

    }



    this->getMass();

    P.addMatrixVector(1.0, M, a, 1.0);



  }



  return P;

}


//=============================================================================

int XC::TotalLagrangianFD8NodeBrick::sendSelf(Communicator &comm)
  {
     // Not implemtented yet
     return 0;
  }


//=============================================================================

int XC::TotalLagrangianFD8NodeBrick::recvSelf(const Communicator &comm)
  {
     // Not implemtented yet
     return 0;
  }

//=============================================================================

void XC::TotalLagrangianFD8NodeBrick::Print(std::ostream &s, int flag) const

  {

    s << "\nTotalLagrangianFD8NodeBrick, element id:  " << this->getTag() << std::endl;

    s << "\nConnected external nodes:  " << theNodes;

    s << "\nBody forces:  " << bf(0) << " " << bf(1) << " " << bf(2) << std::endl;



    physicalProperties[0]->Print(s,flag);



    BJtensor sigma;
    Vector P00(6);

    

    int i;

    for (i=0; i<NumTotalGaussPts; i++)

      {
        const FiniteDeformationMaterial *fdMaterial= dynamic_cast<const FiniteDeformationMaterial *>(physicalProperties[i]);
	sigma = fdMaterial->getCauchyStressTensor();

	P00(0) = sigma(1,1);
	P00(1) = sigma(2,2);
	P00(2) = sigma(3,3);
	P00(3) = sigma(2,3);
	P00(4) = sigma(3,1);
	P00(5) = sigma(1,2);
	s << "\n where = " << i << std::endl;
        s << " Stress (Cauchy): xx yy zz yz zx xy) " << P00 << std::endl;
      }
  }


//=============================================================================

XC::Response *XC::TotalLagrangianFD8NodeBrick::setResponse(const std::vector<std::string> &argv, Information &eleInformation)

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
      return nullptr;
    
  }


//=============================================================================

int XC::TotalLagrangianFD8NodeBrick::getResponse (int responseID, Information &eleInfo)

{    

	 int i;

     static Vector P0(NumTotalGaussPts*6);

     

     switch (responseID) {

     

     case 1:

          return eleInfo.setVector(this->getResistingForce() );



     case 3: { 

        Vector P0(NumTotalGaussPts*6);

        BJtensor sigma; 

        for (i=0; i<NumTotalGaussPts; i++)
	  {
            const FiniteDeformationMaterial *fdMaterial= dynamic_cast<const FiniteDeformationMaterial *>(physicalProperties[i]);
	    sigma = fdMaterial->getCauchyStressTensor();
	    P0(i*6 +0 ) = sigma(1,1);
	    P0(i*6 +1 ) = sigma(2,2);
	    P0(i*6 +2 ) = sigma(3,3);
	    P0(i*6 +3 ) = sigma(2,3);
	    P0(i*6 +4 ) = sigma(3,1);
	    P0(i*6 +5 ) = sigma(1,2);

	  }

        return eleInfo.setVector(P0);

     }



     case 4: { 

        Vector P0(NumTotalGaussPts*6);

        BJtensor sigma; 

        for (i=0; i<NumTotalGaussPts; i++) {

          sigma = physicalProperties[i]->getStressTensor();

          P0(i*6 +0 ) = sigma(1,1);

          P0(i*6 +1 ) = sigma(2,2);

          P0(i*6 +2 ) = sigma(3,3);

          P0(i*6 +3 ) = sigma(2,3);

          P0(i*6 +4 ) = sigma(3,1);

          P0(i*6 +5 ) = sigma(1,2);

        }

        return eleInfo.setVector(P0);

     }



     case 5: { 

        Vector P0(NumTotalGaussPts*6);

        BJtensor e;
	BJtensor E;
	BJtensor F;
	BJtensor tI2("I", def_dim_2); 

        for (i=0; i<NumTotalGaussPts; i++)
	  {
          const FiniteDeformationMaterial *fdMaterial= dynamic_cast<const FiniteDeformationMaterial *>(physicalProperties[i]);
          E = fdMaterial->getStrainTensor();

	  F = fdMaterial->getF();

	  F = F.inverse();

	  e = F("ki")*F("kj"); e.null_indices();

	  e = (tI2-e) *0.5;

          P0(i*6 +0 ) = e(1,1);

          P0(i*6 +1 ) = e(2,2);

          P0(i*6 +2 ) = e(3,3);

          P0(i*6 +3 ) = e(2,3);

          P0(i*6 +4 ) = e(3,1);

          P0(i*6 +5 ) = e(1,2);

        }

        return eleInfo.setVector(P0);

     }



     case 6: { 

        Vector P0(NumTotalGaussPts*6);

        BJtensor E; 

        for (i=0; i<NumTotalGaussPts; i++) {

          E = physicalProperties[i]->getStrainTensor();

          P0(i*6 +0 ) = E(1,1);

          P0(i*6 +1 ) = E(2,2);

          P0(i*6 +2 ) = E(3,3);

          P0(i*6 +3 ) = E(2,3);

          P0(i*6 +4 ) = E(3,1);

          P0(i*6 +5 ) = E(1,2);

        }

        return eleInfo.setVector(P0);

     }

    

     default:

     return -1;



   }    

}




//#############################################################################
//===================================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::shapeFunction(double r1, double r2, double r3)

  {



    std::vector<int> dimension({NumNodes});

    BJtensor h(dimension, 0.0);



      // influence of the node number 8

    h(8)=(1.0+r1)*(1.0-r2)*(1.0-r3)*0.125;

      // influence of the node number 7

    h(7)=(1.0-r1)*(1.0-r2)*(1.0-r3)*0.125;

      // influence of the node number 6

    h(6)=(1.0-r1)*(1.0+r2)*(1.0-r3)*0.125;

      // influence of the node number 5

    h(5)=(1.0+r1)*(1.0+r2)*(1.0-r3)*0.125;



      // influence of the node number 4

    h(4)=(1.0+r1)*(1.0-r2)*(1.0+r3)*0.125;

      // influence of the node number 3

    h(3)=(1.0-r1)*(1.0-r2)*(1.0+r3)*0.125;

      // influence of the node number 2

    h(2)=(1.0-r1)*(1.0+r2)*(1.0+r3)*0.125;

      // influence of the node number 1

    h(1)=(1.0+r1)*(1.0+r2)*(1.0+r3)*0.125;



    return h;

  }




//==============================================================

XC::BJtensor XC::TotalLagrangianFD8NodeBrick::shapeFunctionDerivative(double r1, double r2, double r3)

  {



    std::vector<int> dimensions({NumNodes, NumDof});

    BJtensor dh(dimensions, 0.0);



      // influence of the node number 8

    dh(8,1)= (1.0-r2)*(1.0-r3)*0.125;

    dh(8,2)=-(1.0+r1)*(1.0-r3)*0.125;

    dh(8,3)=-(1.0+r1)*(1.0-r2)*0.125;

      // influence of the node number 7

    dh(7,1)=-(1.0-r2)*(1.0-r3)*0.125;

    dh(7,2)=-(1.0-r1)*(1.0-r3)*0.125;

    dh(7,3)=-(1.0-r1)*(1.0-r2)*0.125;

      // influence of the node number 6

    dh(6,1)=-(1.0+r2)*(1.0-r3)*0.125;

    dh(6,2)= (1.0-r1)*(1.0-r3)*0.125;

    dh(6,3)=-(1.0-r1)*(1.0+r2)*0.125;

      // influence of the node number 5

    dh(5,1)= (1.0+r2)*(1.0-r3)*0.125;

    dh(5,2)= (1.0+r1)*(1.0-r3)*0.125;

    dh(5,3)=-(1.0+r1)*(1.0+r2)*0.125;



      // influence of the node number 4

    dh(4,1)= (1.0-r2)*(1.0+r3)*0.125;

    dh(4,2)=-(1.0+r1)*(1.0+r3)*0.125;

    dh(4,3)= (1.0+r1)*(1.0-r2)*0.125;

      // influence of the node number 3

    dh(3,1)=-(1.0-r2)*(1.0+r3)*0.125;

    dh(3,2)=-(1.0-r1)*(1.0+r3)*0.125;

    dh(3,3)= (1.0-r1)*(1.0-r2)*0.125;

      // influence of the node number 2

    dh(2,1)=-(1.0+r2)*(1.0+r3)*0.125;

    dh(2,2)= (1.0-r1)*(1.0+r3)*0.125;

    dh(2,3)= (1.0-r1)*(1.0+r2)*0.125;

      // influence of the node number 1

    dh(1,1)= (1.0+r2)*(1.0+r3)*0.125;

    dh(1,2)= (1.0+r1)*(1.0+r3)*0.125;

    dh(1,3)= (1.0+r1)*(1.0+r2)*0.125;



    return dh;

  }





#endif



