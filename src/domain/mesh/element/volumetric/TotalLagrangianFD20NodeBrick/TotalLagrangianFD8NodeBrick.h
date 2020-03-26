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
//===============================================================================

#ifndef TOTALLAGRANGIANFD8BRICK_H
#define TOTALLAGRANGIANFD8BRICK_H

#include <domain/mesh/element/ElemWithMaterial.h>
#include "domain/mesh/element/utils/physical_properties/NDMaterialPhysicalProperties.h"
#include "domain/mesh/element/utils/body_forces/BodyForces3D.h"


namespace XC {

//! @ingroup ElemVol
//!
//! @brief Total lagrangian formulation eight node hexahedral element
//! for three-dimensional problems.
//!
//! <a href="https://en.wikiversity.org/wiki/Nonlinear_finite_elements/Total_Lagrangian_approach">Nonlinear finite elements/Total Lagrangian approach</a> 
class TotalLagrangianFD8NodeBrick: public ElemWithMaterial<8,NDMaterialPhysicalProperties>
  {
  private:


    static Matrix K;    // Element stiffness Matrix
//    static Matrix C;    // Element damping matrix

    static Matrix M;    // Element mass matrix
    static Vector P;    // Element resisting force vector
    static const double pts[2];   // Stores quadrature points
    static const double wts[2];   // Stores quadrature weights

    Vector *Q;     // Applied nodal loads

    BodyForces3D bf;     // Body forces

    double det_of_Jacobian;
    mutable Matrix *Ki;

    static const int  NumIntegrationPts;
    static const int  NumTotalGaussPts;
    static const int  NumNodes;
    static const int  NumDof;
    static const int  NumElemDof;



    static BJtensor shapeFunction(double , double , double );
    static BJtensor shapeFunctionDerivative(double , double , double );


    BJtensor Jacobian_3D(double , double , double) const;
    BJtensor Jacobian_3Dinv(double , double , double) const;
    BJtensor dh_Global(double , double , double) const;
    BJtensor getNodesCrds(void) const;
    BJtensor getNodesDisp(void) const;

    BJtensor getStiffnessTensor(void) const;
    BJtensor getRtensor(void) const;
    BJtensor getBodyForce(void) const;
    BJtensor getSurfaceForce(void) const;
    BJtensor getForces(void) const;
  public:
    TotalLagrangianFD8NodeBrick(int tag,

    int node_numb_1,  int node_numb_2,  int node_numb_3,  int node_numb_4,

    int node_numb_5,  int node_numb_6,  int node_numb_7,  int node_numb_8,

    NDMaterial &m, const BodyForces3D &bForces= BodyForces3D());



    TotalLagrangianFD8NodeBrick(void);
    Element *getCopy(void) const;
    ~TotalLagrangianFD8NodeBrick(void);


    int getNumDOF(void) const;

    void setDomain(Domain *theDomain);

    int commitState();
    int revertToLastCommit ();
    int revertToStart ();
    int update();

    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;



    void zeroLoad ();
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);



    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;



    // public methods for element output

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;



    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);


//    int setParameter(const char **argv, int argc, Information &info);
//    int updateParameter(int parameterID, Information &info);
  };

} // end of XC namespace

#endif



