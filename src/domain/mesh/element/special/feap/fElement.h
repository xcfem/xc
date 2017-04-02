//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/feap/fElement.h,v $
                                                                        
                                                                        
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
#include "domain/mesh/element/utils/NodePtrs.h"

namespace XC {
class Node;
class Channel;
class UniaxialMaterial;

//! \ingroup SpecialElem
//!
//!
//! @defgroup fElem Interface with FEAP elements.
//
//! \ingroup fElem
//! @brief Wrapper used to call fortran element subroutines from FEAP.
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
  protected:
    // protected methods 
    virtual int invokefRoutine(int ior, int iow, double *ctan, int isw);
    virtual int readyfRoutine(bool incInertia);
    virtual int invokefInit(int isw, int iow); 

    // protected data
    Vector feapData;
    ID connectedNodes;    
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
 };
} // end of XC namespace

#endif




