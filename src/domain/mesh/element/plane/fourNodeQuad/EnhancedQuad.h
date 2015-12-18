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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/02/14 23:01:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/EnhancedQuad.h,v $
                                                                        
#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/physical_properties/NDMaterialPhysicalProperties.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

namespace XC {
//! \ingroup ElemPlanos
//
//! @brief Cuadrilátero.
class EnhancedQuad : public QuadBase4N<NDMaterialPhysicalProperties>
  {
  private:

    mutable Vector alpha; //!< enhanced strain parameters
    mutable Matrix *Ki;

    //static data
    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;

    //quadrature data
    static const double sg[4];
    static const double tg[4];
    static const double wg[4];

    
    static double stressData[][4]; //!< stress data
    static double tangentData[][3][4]; //!< tangent data 


    //local nodal coordinates, two coordinates for each of four nodes
    //    static double xl[2][4]; 
    static double xl[][4]; 

    //save stress and tangent data
    static void saveData(int gp, const Vector &stress,const Matrix &tangent);

    //recover stress and tangent data
    void getData(int gp,Vector &stress,Matrix &tangent) const;

    //compute enhanced strain B-matrices
    const Matrix& computeBenhanced( int node, double L1, double L2, double j, const Matrix &Jinv) const;

			   
    //compute local coordinates and basis
    void computeBasis(void) const;
        
    //form residual and tangent					  
    void formResidAndTangent(int tang_flag) const;

    //inertia terms
    void formInertiaTerms(int tangFlag) const;


    //compute Jacobian matrix and inverse at point {L1,L2}
    static void computeJacobian( double L1, double L2, const double x[2][4], Matrix &JJ, Matrix &JJinv );

    //compute Bbend matrix
    const Matrix& computeB(int node, const double shp[3][4]) const;

    //Matrix transpose of a 3x2 matrix
    static const Matrix& transpose(const Matrix &M);

    //shape function routine for four node quads
    static void shape2d( double ss, double tt, const double x[2][4], double shp[3][4], double &xsj );


    bool check_material_type(const std::string &type) const;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:

  //full constructor
    EnhancedQuad(int tag, int nd1, int nd2, int nd3, int nd4, NDMaterial &, const std::string &);

    //null constructor
    EnhancedQuad(void);
    Element *getCopy(void) const;
    //destructor
    virtual ~EnhancedQuad(void);

    //set domain
    void setDomain(Domain *);

    //return number of dofs
    int getNumDOF(void) const;

    // methods dealing with state updates
    int update(void);

    //print out element data
    void Print(std::ostream &s, int flag);
	
    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual and residual with inertia terms
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  }; 
} // end of XC namespace
