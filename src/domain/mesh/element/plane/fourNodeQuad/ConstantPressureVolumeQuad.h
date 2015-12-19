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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/02/14 23:01:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/ConstantPressureVolumeQuad.h,v $

// Ed "C++" Love
//
// Constant Presssure/Volume Four Node Quadrilateral
// Plane Strain (NOT PLANE STRESS)

#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/physical_properties/NDMaterialPhysicalProperties.h"

namespace XC{
//! \ingroup ElemPlanos
//
//! @brief ??.
class ConstantPressureVolumeQuad: public QuadBase4N<NDMaterialPhysicalProperties>
  {
  private: 
    double xl[2][4]; //!< nodal coordinates, two coordinates for each of four nodes

    //static data
    static double matrixData[64];  // array data for matrix
    static Matrix stiff ;
    static Vector resid ;
    static Matrix mass ;
    static Matrix damping ;
    
    //volume-pressure constants
    static double one3 ;
    static double two3 ;
    static double four3 ;
    static double one9 ;
    
    //quadrature data
    static double sg[4] ;
    static double tg[4] ;
    static double wg[4] ;
    
    
    //form residual and tangent					  
    void formResidAndTangent(int tang_flag) const;
    //inertia terms
    void formInertiaTerms(int tangFlag) const;

    //shape function routine for four node quads
    static void shape2d( double ss, double tt, 
		  const double x[2][4], 
		  double shp[3][4], 
		  double &xsj, 
		  Matrix &sx ) ;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public :
    ConstantPressureVolumeQuad(void) ;
    ConstantPressureVolumeQuad(int tag, int node1,int node2,int node3,int node4, NDMaterial &theMaterial);
    Element *getCopy(void) const;

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain ) ;

    // public methods to set the state of the element    
    int update(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    // public methods for updating ele load information
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print( std::ostream &s, int flag ) ;
  }; 
} // end of XC namespace
