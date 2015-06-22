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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/feap/fElement.h,v $
                                                                        
                                                                        
#ifndef fElement_h
#define fElement_h

// File: ~/element/fortran/fElement.h
// 
// Written: fmk 
// Created: 03/99
// Revision: A
//
// Description: This file contains the class definition for fElement. fElement
// is a wrapper used to call fortran element subroutine. It is an abstract class.
//
// What: "@(#) fElement.h, revA"

#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/NodePtrs.h"

namespace XC {
class Node;
class Channel;
class UniaxialMaterial;

//! \ingroup Elem
//!
//!
//! @defgroup fElem Interfaz con los elementos de FEAP.
//!
//! \ingroup fElem
//
//! @brief Interfaz con los elementos de FEAP.
class fElement : public Element
  {
  private:
    // private attributes - a copy for each object of the class      
    double *h;            //!< the elements local h array
    NodePtrs theNodes;       //!< pointer to the elements nodes
    double *u;            //!< to hold u^(k-1)_(n+1) as nodes don't save this
    double *d; 	          //!< data stored for each element
    int eleType;          //!< defines which elmtnn subroutine to invoke
    int ndf;		  //!< number of dof at nodes  nst = nen x ndf
    int nen;              //!< number of element nodes
    int ndm;	          //!< dimension of mesh
    int nh1, nh3;         //!< the size of nh1(nh2) and nh3 data in h
    int nrCount;          //!< needed for Prof. Fillipou's Elmt05

    Matrix *Ki;
	
    // static data - single copy for all objects of the class	
    static std::vector<Matrix *> fElementM;   // class wide matrices - use s array
    static std::vector<Vector *> fElementV;   // class wide vectors - use r array
    static std::vector<double> s;  // element tangent (nst x nst)
    static std::vector<double> r;  // element residual (ndf x nen) == (nst)
    static std::vector<double> ul; // nodal responses (ndf x nen x 5) == (nst X 5)
    static std::vector<double> xl; // nodal coordinates (ndf x nen) == (nst)
    static std::vector<double> tl; // nodal temp (nen)
    static std::vector<int> ix; // nodal tags (nen)
    static int numfElements;
  public:
    // constructors
    fElement(int tag, 
	     int classTag,
	     int eleType,
	     int sizeD, int nen,
	     int ndm, int ndf,
	     int nh1, int nh3);

    fElement(int tag, 
	     int classTag,
	     int eleType,
	     int sizeD, int nen,
	     int ndm, int ndf, int iow);
    
    fElement(int classTag);    
    
    // destructor
    virtual ~fElement();

    // public methods for element operations
    virtual int getNumExternalNodes(void) const;
    virtual const ID &getExternalNodes(void) const;

    virtual int getNumDOF(void) const;	
    virtual void setDomain(Domain *theDomain);
    
    virtual int commitState(void);
    virtual int revertToLastCommit(void);        
    virtual int revertToStart(void);        
    virtual int update(void);
    
    virtual const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    virtual const Matrix &getDamp(void) const;    
    virtual const Matrix &getMass(void) const;    

    virtual void zeroLoad(void);	
    virtual int addLoad(ElementalLoad *theLoad, double loadFactor);
    virtual int addInertiaLoadToUnbalance(const Vector &accel);

    virtual const Vector &getResistingForce(void) const;
    virtual const Vector &getResistingForceIncInertia(void) const;            

    // public methods for output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    virtual void Print(std::ostream &s, int flag =0);    

  protected:
    // protected methods 
    virtual int invokefRoutine(int ior, int iow, double *ctan, int isw);
    virtual int readyfRoutine(bool incInertia);
    virtual int invokefInit(int isw, int iow); 

    // protected data
    Vector *data;
    ID *connectedNodes;
	
};
} // fin namespace XC

#endif




