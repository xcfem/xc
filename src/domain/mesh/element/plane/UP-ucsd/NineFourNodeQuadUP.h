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
///////////////////////////////////////////////////////////////////////////////
// Description: This file contains the class declaration for                 //
// NineFourNodeQuadUP, a 9-4-node (9 node for solid and 4 node for fluid) //
// plane strain element for solid-fluid fully coupled analysis. This         //
// implementation is a simplified u-p formulation of Biot theory             //
// (u - solid displacement, p - fluid pressure). Each element node has two   //
// DOFs for u and 1 DOF for p.                                               //
//                                                                           //
// Written by Zhaohui Yang	(March 2004)                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// $Revision: 1.1 $
// $Date: 2005/09/22 21:28:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/UP-ucsd/Nine_Four_Node_QuadUP.h,v $

#ifndef NineFourNodeQuadUP_h
#define NineFourNodeQuadUP_h

#include <domain/mesh/element/ElemWithMaterial.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "domain/mesh/element/physical_properties/SolidMech2D.h"
#include "domain/mesh/element/body_forces/BodyForces2D.h"

namespace XC {
class Node;
class NDMaterial;
class Response;

//! \ingroup ElemPlanos
//
//! @brief 9-4-node (9 node for solid and 4 node for fluid)
//! plane strain element for solid-fluid fully coupled analysis. This
//! implementation is a simplified u-p formulation of Biot theory
//! (u - solid displacement, p - fluid pressure). Each element node has two
//! DOFs for u and 1 DOF for p.
class NineFourNodeQuadUP: public ElemWithMaterial<9,SolidMech2D>
  {
  private:
    BodyForces2D bf; //!< Body forces
    mutable Matrix *Ki;

    double kc;   // combined bulk modulus
    double perm[2];  // lateral/vertical permeability

    static Matrix K;		// Element stiffness, damping, and mass Matrix
    static Vector P;		// Element resisting force vector
    static const int nintu;
    static const int nintp;
    static const int nenu;
    static const int nenp;

    static double shgu[3][9][9];	// Stores shape functions and derivatives (overwritten)
    static double shgp[3][4][4];	// Stores shape functions and derivatives (overwritten)
    static double shgq[3][9][4];	// Stores shape functions and derivatives (overwritten)
    static double shlu[3][9][9];	// Stores shape functions and derivatives
    static double shlp[3][4][4];	// Stores shape functions and derivatives
    static double shlq[3][9][4];	// Stores shape functions and derivatives
    static double wu[9];		// Stores quadrature weights
    static double wp[4];		// Stores quadrature weights
    static double dvolu[9];  // Stores detJacobian (overwritten)
    static double dvolp[4];  // Stores detJacobian (overwritten)
    static double dvolq[4];  // Stores detJacobian (overwritten)

    // private member functions - only objects of this class can call these
    double mixtureRho(int ipt) const;  // Mixture mass density at integration point i
    void shapeFunction(double *w, int nint, int nen, int mode) const;
    void globalShapeFunction(double *dvol, double *w, int nint, int nen, int mode) const;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    NineFourNodeQuadUP(int tag, int nd1, int nd2, int nd3, int nd4,
		  int nd5, int nd6, int nd7, int nd8, int nd9,
		  NDMaterial &m, const char *type,
		  double t, double bulk, double rhof, double perm1, double perm2,
		  const BodyForces2D &bForces= BodyForces2D());
    NineFourNodeQuadUP(void);
    Element *getCopy(void) const;    
    virtual ~NineFourNodeQuadUP();

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element
    int update(void);

    // public methods to obtain stiffness, mass, damping and residual information
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);

    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
  };
} // end of XC namespace

#endif

