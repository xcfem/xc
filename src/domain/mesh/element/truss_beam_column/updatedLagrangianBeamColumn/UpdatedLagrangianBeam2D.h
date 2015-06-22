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
** See file 'COPYRIGHT'  in main directory for information on usage   **
** and redistribution of OpenSees, and for a DISCLAIMER OF ALL        **
** WARRANTIES.                                                        **
**                                                                    **
** UpdatedLagrangianBeam2D.h: interface for UpdatedLagrangianBeam2D   **
** Developed by:                                                      **
**    Rohit Kaul       (rkaul@stanford.edu)                           **
**    Greg Deierlein   (ggd@stanford.edu)                             **
**                                                                    **
**           John A. Blume Earthquake Engineering Center              **
**                    Stanford University                             **
** ****************************************************************** **/


// UpdatedLagrangianBeam2D.h: interface for the UpdatedLagrangianBeam2D class
// Written: rkaul
//
// Description: This file contains the class definition for UpdatedLagrangianBeam2D.

// UpdatedLagrangianBeam2D is an abstract class providing most methods required by
// the base class "Element", for 2D beam-column elements. Geometric
// nonlinearity is incorporated at this level using updated lagrangian
// formulation

#ifndef UpdatedLagrangianElement2D
#define UpdatedLagrangianElement2D



#include <domain/mesh/node/Node.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/Element1D.h>

namespace XC {
class Response;

//! \ingroup ElemBarra
//
//! @brief Elemento barra con formulación lagrangiana para problemas bidimensionales.
class UpdatedLagrangianBeam2D : public Element1D
  {
  protected:
    bool    isLinear;
    int     numDof;
    mutable double  L; //Longitud del elemento.
    mutable double sn, cs; //Dirección del elemento.
    double massDof;
  
    Node *end1Ptr(void);
    const Node *end1Ptr(void) const;
    Node *end2Ptr(void);
    const Node *end2Ptr(void) const;
    double  L_hist, cs_hist, sn_hist;
  
    mutable Vector eleForce;
    Vector eleForce_hist;

    int nodeRecord, dofRecord;
    mutable int m_Iter;

    mutable Matrix *Ki;

    static Matrix K, Kg, Kt; // stiffness matrices
    static Matrix M; // mass matrix
    static Matrix D; // damping matrix
    static Matrix T; // transformation matrix
    
    static Vector disp;
    static Vector force;
    
    // ZeroVector and ZeroMatrix should always have zero value,
    // used for quick return
    static Vector ZeroVector;
    static Matrix ZeroMatrix;
    
    // used for temporarily storing nodal displacements
    static Vector end1IncrDisp;
    static Vector end2IncrDisp;
    
    virtual void  getLocalStiff(Matrix &K) const=0;
    virtual void  getLocalMass(Matrix &M) const=0;
    
    
    void  getIncrLocalDisp(Vector &localDisp) const;
    void  getTrialNaturalDisp(Vector &localDisp);
    void  getIncrNaturalDisp(Vector &nDisp) const;
    void  getConvLocalDisp(Vector &lDisp);
    void  getTrialLocalDisp(Vector &lDisp) const;
    void  getTrialLocalForce(Vector &force) const;
    
    virtual void updateState(void) const;

    void  addInternalGeomStiff(Matrix &K) const;
    void  addExternalGeomStiff(Matrix &K) const;
    
    void  transformToGlobal(Matrix &K) const;
  public:
    UpdatedLagrangianBeam2D(int classTag);
    UpdatedLagrangianBeam2D(int tag, int classTag, int nd1, int nd2, bool islinear = false);
    virtual ~UpdatedLagrangianBeam2D(void);

    //////////////////////////////////////////////////////////////////////
    // Overridden public methods, defined in Element class
    // (see ~/OpenSees/SRC/element/Element.h)
    //////////////////////////////////////////////////////////////////////
 public:
    virtual int update(void);
    // void	setEndRelease(ID &g);
    
    int getNumDOF(void) const;
    virtual void setDomain(Domain *theDomain);
    
    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    
    // methods to return the current linearized stiffness,
    // damping and mass matrices
    virtual const	Matrix &getTangentStiff(void) const;
    virtual const Matrix &getInitialStiff(void) const;
    virtual const Matrix &getMass(void) const;
    
    // methods for returning and applying loads
    virtual Vector &getUVLoadVector(double q1, double q2);
    int addLoad(const Vector &load);
    int addLoad(ElementalLoad *theLoad, double loadFactor)
      { return -1;}
    int     addInertiaLoadToUnbalance(const Vector &accel)
      { return -1;}
    
    virtual const Vector &getResistingForce(void) const;
    const	Vector &getResistingForceIncInertia(void) const;
    
    virtual Response *setResponse(const std::vector<std::string> &argv,
				Information &eleInformation);
    virtual int getResponse(int responseID, Information &eleInformation);
  };
} // fin namespace XC


#endif // !defined(UpdatedLagrangianElement2D)
