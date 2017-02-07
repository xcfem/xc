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
                                                                        
// Written: Leopoldo Tesser, Diego Talledo
// 9-node lagrangian shell element with membrane and drill
//
#ifndef ShellNL_h
#define ShellNL_h

#include "domain/mesh/element/plane/QuadBase9N.h"
#include "domain/mesh/element/utils/physical_properties/SectionFDPhysicalProperties.h"
#include "ShellLinearCrdTransf3d.h"
#include "domain/mesh/element/utils/fvectors/FVectorShell.h"

namespace XC {

//! \ingroup ElemPlanos
//
//! @brief Elemento shell lagrangiano.
class ShellNL : public QuadBase9N<SectionFDPhysicalProperties>
  {
  private : 
    double Ktt;//!< drilling stiffness
    ShellLinearCrdTransf3d theCoordTransf; //!< Coordinate transformation.
    mutable Matrix *Ki;
    //local nodal coordinates, two coordinates for each of nine nodes
    double xl[2][9];
    //shell basis vectors

    FVectorShell p0; // Reactions in the basic system due to element loads

    //static data
    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;

    //quadrature data
    static const double root3;
    static const double root3_over_root5;
				  


    void computeBasis(void);
    void formInertiaTerms(int tangFlag) const;
    void formResidAndTangent(int tang_flag) const;

    //compute Jacobian matrix and inverse at point {L1,L2}
    //void  computeJacobian( double L1, double L2,const double x[2][9], 
    //                       Matrix &JJ,Matrix &JJinv );

    double* computeBdrill(int node,const double shp[3][9]) const;
    const Matrix& assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const;
    const Matrix& computeBmembrane(int node, const double shp[3][9]) const;
    const Matrix& computeBbend(int node, const double shp[3][9]) const;
    const Matrix& computeBshear(int node, const double shp[3][9]) const;
    
    //Matrix transpose
    Matrix transpose( int dim1, int dim2, const Matrix &M);

    //shape function routine for four node quads
    static void shape2d( double ss, double tt, const double x[2][9], double shp[3][9], double &xsj );
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    //null constructor
    ShellNL(void);
    //full constructor
    ShellNL(int tag,const SectionForceDeformation *theMaterial);
    Element *getCopy(void) const;
    virtual ~ShellNL(void);

    int getNumDOF(void) const;

    void setDomain(Domain *theDomain);

    int revertToLastCommit(void);
    int revertToStart(void);

    //print out element data
    void Print(std::ostream &, int flag);
	
    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff() const;
    const Matrix &getMass() const;

    const GaussModel &getGaussModel(void) const;

    // methods for applying loads
    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    virtual ShellCrdTransf3dBase *getCoordTransf(void);
    virtual const ShellCrdTransf3dBase *getCoordTransf(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

  };
} // end of XC namespace

#endif



