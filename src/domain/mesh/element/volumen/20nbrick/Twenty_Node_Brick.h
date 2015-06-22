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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// by Jinchi Lu and Zhaohui Yang (May 2004)
//
// 20NodeBrick element
//

#include <domain/mesh/element/ElemWithMaterial.h>
#include "domain/mesh/element/physical_properties/NDMaterialPhysicalProperties.h"
#include "domain/mesh/element/body_forces/BodyForces3D.h"


namespace XC {
  class NDMaterial;
//! \ingroup ElemVol
//
//! @brief Hexaedro de veinte nodos.
class Twenty_Node_Brick : public ElemWithMaterial<20,NDMaterialPhysicalProperties>
  {
  private:
    BodyForces3D bf; //!< Body forces
    mutable Matrix *Ki;

    typedef ElemWithMaterial<20,NDMaterialPhysicalProperties> Twenty_Node_Brick_Base;
    //static data
    static Matrix stiff ;
    static Vector resid ;
    static Matrix mass ;
    static Matrix damp ;

    //quadrature data
    static const int nintu;
    static const int nenu;

    //local nodal coordinates, three coordinates for each of twenty nodes
    //    static double xl[3][20] ;
    static double xl[3][20] ;

    static double shgu[4][20][27];	// Stores shape functions and derivatives (overwritten)
    static double shlu[4][20][27];	// Stores shape functions and derivatives
    static double wu[27];		// Stores quadrature weights
    static double dvolu[27];  // Stores detJacobian (overwritten)

    void formInertiaTerms( int tangFlag ) const;
    void formDampingTerms( int tangFlag ) const;
    double mixtureRho(int ipt) const;
    void computeBasis(void) const;


    // compute local shape functions
    void compuLocalShapeFunction(void);
    static void Jacobian3d(int gaussPoint, double& xsj, int mode);
    const Matrix &getStiff( int flag ) const;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public :
    //null constructor
    Twenty_Node_Brick( ) ;

    //full constructor
    Twenty_Node_Brick( int tag,
		int node1,
		int node2,
		int node3,
		int node4,
		int node5,
		int node6,
		int node7,
		int node8,
		int node9,
		int node10,
		int node11,
		int node12,
		int node13,
		int node14,
		int node15,
		int node16,
		int node17,
		int node18,
		int node19,
		int node20,
		NDMaterial &theMaterial,
		 const BodyForces3D &bForces= BodyForces3D()) ;
    Element *getCopy(void) const;
    //destructor
    virtual ~Twenty_Node_Brick(void) ;

    //set domain
    void setDomain( Domain *theDomain ) ;

    //return number of dofs
    int getNumDOF(void) const;

    //print out element data
    void Print( std::ostream &s, int flag ) ;

	int update(void);

    //return stiffness matrix
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);

    //plotting
    int displaySelf(Renderer &theViewer, int displayMode, float fact);
  };

} // fin namespace XC
