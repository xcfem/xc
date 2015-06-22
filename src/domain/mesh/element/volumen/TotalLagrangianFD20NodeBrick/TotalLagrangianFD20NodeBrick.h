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
//# DATE:              Sept2003
//# UPDATE HISTORY:    28May2004, Zhao Optimized the Stiffness Tensor
//#
//#
//===============================================================================
#ifndef TOTALLAGRANGIANFD20BRICK_H
#define TOTALLAGRANGIANFD20BRICK_H

#include <domain/mesh/element/ElemWithMaterial.h>
#include "domain/mesh/element/physical_properties/NDMaterialPhysicalProperties.h"
#include "domain/mesh/element/body_forces/BodyForces3D.h"

namespace XC {
  class BJtensor;
  class NDMaterial;
//! \ingroup ElemVol
//
//! @brief Hexaedro de veinte nodos con formulación lagrangiana.
class TotalLagrangianFD20NodeBrick: public ElemWithMaterial<20,NDMaterialPhysicalProperties>
  {
  private:
    static Matrix K;    // Element stiffness Matrix
//    static Matrix C;    // Element damping matrix
    static Matrix M;    // Element mass matrix
    static Vector P;    // Element resisting force vector
    static const double pts[3];   // Stores quadrature points
    static const double wts[3];   // Stores quadrature weights
    BodyForces3D bf; //!< Body forces

    double rho;    // Mass per unit volume

    double det_of_Jacobian;
    
    mutable Matrix *Ki;

    static const int  NumIntegrationPts;
    static const int  NumTotalGaussPts;
    static const int  NumNodes;
    static const int  NumDof;
    static const int  NumElemDof;
    
    static BJtensor shapeFunction(double , double , double );
    static BJtensor shapeFunctionDerivative(double , double , double );

    BJtensor Jacobian_3D(BJtensor dh) const;
    BJtensor Jacobian_3Dinv(BJtensor dh) const;
    BJtensor dh_Global(BJtensor dh) const;
    BJtensor getNodesCrds(void) const;
    BJtensor getNodesDisp(void) const;

    BJtensor getStiffnessTensor(void) const;
    BJtensor getRtensor(void) const;
    BJtensor getBodyForce(void) const;
    BJtensor getSurfaceForce(void) const;
    BJtensor getForces(void) const;
  public:
    TotalLagrangianFD20NodeBrick(int tag,
    int node_numb_1,  int node_numb_2,  int node_numb_3,  int node_numb_4,
    int node_numb_5,  int node_numb_6,  int node_numb_7,  int node_numb_8,
    int node_numb_9,  int node_numb_10, int node_numb_11, int node_numb_12,
    int node_numb_13, int node_numb_14, int node_numb_15, int node_numb_16,
    int node_numb_17, int node_numb_18, int node_numb_19, int node_numb_20,
    NDMaterial &m, const BodyForces3D &bForces= BodyForces3D());
    TotalLagrangianFD20NodeBrick (void);
    Element *getCopy(void) const;
    ~TotalLagrangianFD20NodeBrick(void);

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    int update();

    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse (int responseID, Information &eleInformation);

//    int setParameter(const std::vector<std::string> &argv, Parameter &param);
//    int updateParameter(int parameterID, Information &info);
  };
} // fin namespace XC


#endif

