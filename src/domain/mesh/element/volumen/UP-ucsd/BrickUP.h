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

///////////////////////////////////////////////////////////////////////////////
// Description: This file contains the class declaration for                 //
// BrickUP, an 8-node cubic element for solid-fluid fully coupled analysis.  //
// This implementation is a simplified u-p formulation of Biot theory        //
// (u - solid displacement, p - fluid pressure). Each element node has three //
// DOFs for u and 1 DOF for p.                                               //
//                                                                           //
// Written by Zhaohui Yang	(March 2004)                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// $Revision: 1.1 $
// $Date: 2005/09/22 21:28:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/UP-ucsd/BrickUP.h,v $

// by Zhaohui Yang (Modified based on Ed "C++" Love's Brick element)
//
// Eight node BrickUP element
//

#include <domain/mesh/element/volumen/BrickBase.h>
#include "domain/mesh/element/body_forces/BodyForces3D.h"

namespace XC{
class NDMaterial;
//! \ingroup ElemVol
//
//! @brief Hexaedro de ocho nodos.
class BrickUP : public BrickBase
  {
  private :
    BodyForces3D bf; //!< Body forces
    double rho;			// Fluid mass per unit volume
    double kc;   // combined bulk modulus
    double perm[3];  // permeability
    mutable Matrix *Ki;

    //static data
    static Matrix stiff ;
    static Vector resid ;
    static Matrix mass ;
    static Matrix damp ;

    //quadrature data
    static const double sg[2] ;
    static const double wg[8] ;


    //local nodal coordinates, three coordinates for each of four nodes
    //    static double xl[3][8] ;
    static double xl[][8] ;

    void formInertiaTerms(int tangFlag) const;
    void formDampingTerms(int tangFlag) const;
    void formResidAndTangent(int tang_flag) const;
    double mixtureRho(int ipt) const;
    void computeBasis(void) const;
    const Matrix& computeBbar(int node,const double shp[4][8],const double shpBar[4][8] ) ;
    const Matrix& computeB( int node, const double shp[4][8] ) const;
    Matrix transpose( int dim1, int dim2, const Matrix &M ) ;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public :
    //null constructor
    BrickUP(void);

    //full constructor
    BrickUP( int tag, int node1,int node2,int node3,int node4,int node5,int node6,int node7,int node8,NDMaterial &theMaterial,double bulk, double rhof, double perm1, double perm2, double perm3,  const BodyForces3D &bForces= BodyForces3D()) ;
    Element *getCopy(void) const;
    //destructor
    virtual ~BrickUP(void) ;

    //set domain
    void setDomain( Domain *theDomain ) ;

    //return number of dofs
    int getNumDOF(void) const;

    //print out element data
    void Print( std::ostream &s, int flag ) ;

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

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);


    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
} ;

} // fin namespace XC
