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
                                                                        
// $Revision: 1.11 $
// $Date: 2006/01/10 18:41:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/brick/Brick.h,v $

// Ed "C++" Love
//
// Eight node Brick element 
//

#include <domain/mesh/element/volumen/BrickBase.h>
#include "domain/mesh/element/body_forces/BodyForces3D.h"

namespace XC {
//! \ingroup ElemVol
//
//! @brief Hexaedro de ocho nodos.
class Brick : public BrickBase
  {
  private : 
    BodyForces3D bf; //!< Body forces
    mutable Matrix *Ki;

    //
    // static attributes
    //

    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;

    //quadrature data
    static const double sg[2];
    static const double wg[8];
  
    //local nodal coordinates, three coordinates for each of eight nodes
    static double xl[3][8];

    //
    // private methods
    //

    //inertia terms
    void formInertiaTerms(int tangFlag) const;
    //form residual and tangent					  
    void formResidAndTangent(int tang_flag) const;

    //compute coordinate system
    void computeBasis(void) const;

    //compute B matrix
    const Matrix& computeB( int node, const double shp[4][8]) const;
  
    //Matrix transpose
    Matrix transpose( int dim1, int dim2, const Matrix &M );
    static size_t getVectorIndex(const size_t &,const size_t &);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public :
    
    //null constructor
    Brick(void);
    Brick(int tag,const NDMaterial *ptr_mat);
  
    //full constructor
    Brick( int tag, int node1,int node2,int node3,int node4,int node5,int node6,int node7,int node8, NDMaterial &theMaterial, const BodyForces3D &bf= BodyForces3D());
    Element *getCopy(void) const;
    //destructor 
    virtual ~Brick(void);

    //set domain
    void setDomain( Domain *theDomain );

    //return number of dofs
    int getNumDOF(void) const;

    // update
    int update(void);

    //return stiffness matrix 
    const Matrix &getTangentStiff() const;
    const Matrix &getInitialStiff() const;    
    const Matrix &getMass() const;    

    Vector getAvgStress(void) const;
    double getAvgStress(const size_t &,const size_t &) const;
    Vector getAvgStrain(void) const;
    double getAvgStrain(const size_t &,const size_t &) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual
    const Vector &getResistingForce(void) const;
    
    //get residual with inertia terms
    const Vector &getResistingForceIncInertia(void) const;

    // public methods for element output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
      
    //print out element data
    void Print( std::ostream &s, int flag );
    any_const_ptr GetProp(const std::string &cod) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  }; 

} // end of XC namespace
