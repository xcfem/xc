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
                                                                        
// $Revision: 1.16 $
// $Date: 2005/11/28 21:38:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/transformation/TransformationFE.cpp,v $
                                                                        
// Written: fmk 
// Created: 05/99
//
// Purpose: This file contains the code for implementing the methods
// of the XC::TransformationFE class interface.

#include "solution/analysis/model/fe_ele/transformation/TransformationFE.h"
#include <cstdlib>

#include <domain/mesh/element/Element.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/IntPtrWrapper.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/handler/TransformationConstraintHandler.h>
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"

const int MAX_NUM_DOF= 64;

// static variables initialisation
XC::UnbalAndTangentStorage XC::TransformationFE::unbalAndTangentArrayMod(MAX_NUM_DOF+1);
std::vector<XC::Matrix *> XC::TransformationFE::theTransformations; 
int XC::TransformationFE::numTransFE(0);           
int XC::TransformationFE::transCounter(0);           
int XC::TransformationFE::sizeTransformations(0);          
XC::Vector XC::TransformationFE::dataBuffer(MAX_NUM_DOF*MAX_NUM_DOF);
XC::Vector XC::TransformationFE::localKbuffer(MAX_NUM_DOF*MAX_NUM_DOF);          
XC::ID XC::TransformationFE::dofData(MAX_NUM_DOF);          
int XC::TransformationFE::sizeBuffer(MAX_NUM_DOF*MAX_NUM_DOF);

//  TransformationFE(Element *, Integrator *theIntegrator);
//        construictor that take the corresponding model element.
XC::TransformationFE::TransformationFE(int tag, Element *ele)
  :FE_Element(tag, ele), theDOFs(), /* numSPs(0), theSPs(),*/  
  numGroups(0), numTransformedDOF(0),unbalAndTangentMod(numTransformedDOF,unbalAndTangentArrayMod)
  {
  // set number of original dof at ele
    numOriginalDOF = ele->getNumDOF();

    // create the array of pointers to DOF_Groups
    const ID &nodes = ele->getNodePtrs().getExternalNodes();
    Domain *theDomain = ele->getDomain();
    int numNodes = nodes.Size();
    theDOFs= std::vector<DOF_Group *>(numNodes,static_cast<DOF_Group *>(nullptr));

    numGroups = numNodes;

    // now fill the array of DOF_Group pointers
    for(int i=0; i<numNodes; i++) {
        Node *theNode = theDomain->getNode(nodes(i));
        if(theNode == 0) {
            std::cerr << "FATAL XC::TransformationFE::TransformationFE() - no XC::Node with tag: ";
            std::cerr << nodes(i) << " in the domain\n";;
            exit(-1);
        }
        DOF_Group *theDofGroup = theNode->getDOF_GroupPtr();
        if(theDofGroup == 0) {
            std::cerr << "FATAL XC::TransformationFE::TransformationFE() - no XC::DOF_Group : ";
            std::cerr << " associated with node: " << nodes(i) << " in the domain\n";;
            exit(-1);
        }        
        theDOFs[i] = theDofGroup;
    }

    // see if theTransformation array is big enough
    // if not delete the old and create a new_ one
    if(numNodes > sizeTransformations)
      {
        theTransformations.resize(numNodes,static_cast<Matrix *>(nullptr));
        sizeTransformations = numNodes;
      }        

    // increment the number of transformations
    numTransFE++;
  }

//! @brief destructor.
XC::TransformationFE::~TransformationFE()
  {
    numTransFE--;

    // if this is the last XC::FE_Element, clean up the
    // storage for the matrix and vector objects
    if(numTransFE == 0)
      {
        sizeTransformations = 0;
        sizeBuffer = 0;
        transCounter = 0;
      }
  }    


const XC::ID &XC::TransformationFE::getDOFtags(void) const 
  {
    return this->XC::FE_Element::getDOFtags();
  }


const XC::ID &XC::TransformationFE::getID(void) const
  { return modID; }


int XC::TransformationFE::setID(void)
  {
    // determine number of DOF
    numTransformedDOF = 0;
    for(int ii=0; ii<numGroups; ii++)
      {
        DOF_Group *dofPtr = theDOFs[ii];
        numTransformedDOF += dofPtr->getNumDOF();
      }

    // create an ID to hold the array, cannot use existing as 
    // may be different size
    modID= ID(numTransformedDOF);

    // fill in the ID
    int current = 0;
    for(int i=0; i<numGroups; i++)
      {
        DOF_Group *dofPtr = theDOFs[i];
        const XC::ID &theDOFid = dofPtr->getID();

        for(int j=0; j<theDOFid.Size(); j++)  
            if(current < numTransformedDOF)
                modID(current++) = theDOFid(j);
            else
              {
                std::cerr << "WARNING XC::TransformationFE::setID() - numDOF and";
                std::cerr << " number of dof at the DOF_Groups\n";
                return -3;
              }                
      }
    unbalAndTangentMod= UnbalAndTangent(numTransformedDOF,unbalAndTangentArrayMod);
    return 0;
  }

const XC::Matrix &XC::TransformationFE::getTangent(Integrator *theNewIntegrator)
  {
    const Matrix &theTangent = this->FE_Element::getTangent(theNewIntegrator);

    static IntPtrWrapper numDOFs(dofData.getDataPtr(), 1);
    numDOFs.setData(dofData.getDataPtr(), numGroups);
    
    // DO THE SP STUFF TO THE TANGENT 
    
    // get the transformation matrix from each dof group & number of local dof
    // for original node.
    int numNode = numGroups;
    for(int a= 0;a<numNode;a++)
      {
        Matrix *theT = theDOFs[a]->getT();
        theTransformations[a] = theT;
        if(theT != 0)
          numDOFs[a] = theT->noRows(); // T^ 
        else
          numDOFs[a] = theDOFs[a]->getNumDOF();
      }

    // perform Tt K T -- as T is block diagonal do T(i)^T K(i,j) T(j)
    // where blocks are of size equal to num ele dof at a node

    int startRow = 0;
    int noRowsTransformed = 0;
    int noRowsOriginal = 0;

    static XC::Matrix localK;

    // foreach block row, for each block col do
    for(int i=0; i<numNode; i++) {

        int startCol = 0;
        int numDOFi = numDOFs[i];        
        int noColsOriginal = 0;

        for(int j=0; j<numNode; j++) {

            const Matrix *Ti= theTransformations[i];
            const Matrix *Tj= theTransformations[j];
            int numDOFj = numDOFs[j];        
            localK.setData(localKbuffer.getDataPtr(), numDOFi, numDOFj);

            // copy K(i,j) into localK matrix
            // CHECK SIZE OF BUFFFER            
            for(int a=0; a<numDOFi; a++)
                for(int b=0; b<numDOFj; b++)
                    localK(a,b) = theTangent(noRowsOriginal+a, noColsOriginal+b);

            // now perform the matrix computation T(i)^T localK T(j)
            // note: if T == 0 then the Identity is assumed
            int noColsTransformed = 0;
            static Matrix localTtKT;
            
            if(Ti != 0 && Tj != 0) {
                noRowsTransformed = Ti->noCols();
                noColsTransformed = Tj->noCols();
                // CHECK SIZE OF BUFFFER
                localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
                localTtKT = (*Ti) ^ localK * (*Tj);
            } else if(Ti == 0 && Tj != 0) {
                noRowsTransformed = numDOFi;
                noColsTransformed = Tj->noCols();
                // CHECK SIZE OF BUFFFER
                localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
                // localTtKT = localK * (*Tj);               
                localTtKT.addMatrixProduct(0.0, localK, *Tj, 1.0);
            } else if(Ti != 0 && Tj == 0) {
                noRowsTransformed = Ti->noCols();
                noColsTransformed = numDOFj;
                // CHECK SIZE OF BUFFFER
                localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
                localTtKT = (*Ti) ^ localK;
            } else {
                noRowsTransformed = numDOFi;
                noColsTransformed = numDOFj;
                localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
                localTtKT = localK;
            }
            // now copy into modTangent the T(i)^t K(i,j) T(j) product
            for(int c=0; c<noRowsTransformed; c++) 
                for(int d=0; d<noColsTransformed; d++) 
                    (unbalAndTangentMod.getTangent())(startRow+c, startCol+d) = localTtKT(c,d);
            
            startCol += noColsTransformed;
            noColsOriginal += numDOFj;
        }

        noRowsOriginal += numDOFi;
        startRow += noRowsTransformed;
    }

    return unbalAndTangentMod.getTangent();
  }


const XC::Vector &XC::TransformationFE::getResidual(Integrator *theNewIntegrator)
  {
    const Vector &theResidual = this->XC::FE_Element::getResidual(theNewIntegrator);
    // DO THE SP STUFF TO THE TANGENT
    
    // perform Tt R  -- as T is block diagonal do T(i)^T R(i)
    // where blocks are of size equal to num ele dof at a node

    int startRowTransformed = 0;
    int startRowOriginal = 0;
    int numNode = numGroups;

    // foreach block row, for each block col do
    for(int i=0; i<numNode; i++) {
        int noRows = 0;
        int noCols = 0;
        const Matrix *Ti = theDOFs[i]->getT();
        if(Ti != 0) {
          noRows = Ti->noCols(); // T^
          noCols = Ti->noRows();

          /*
          Vector orig(noCols);
          Vector mod(noRows);
          for(int k=startRowOriginal; k<startRowOriginal+noCols; k++)
            orig(k-startRowOriginal)= theResidual(k);
          mod = (*Ti)^orig;
          for(int k=startRowTransformed; k<startRowTransformed+noRows; k++)
            (unbalAndTangentMod.getResidual())(k) = mod (k-startRowTransformed);

          */

          for(int j=0; j<noRows; j++) {
            double sum = 0.0;
            for(int k=0; k<noCols; k++)
              sum += (*Ti)(k,j) * theResidual(startRowOriginal + k);
            (unbalAndTangentMod.getResidual())(startRowTransformed +j) = sum;
          }

        } else {
          noCols = theDOFs[i]->getNumDOF();
          noRows = noCols;
          for(int j=0; j<noRows; j++)
            (unbalAndTangentMod.getResidual())(startRowTransformed +j) = theResidual(startRowOriginal + j);
        }
        startRowTransformed += noRows;
        startRowOriginal += noCols;
    }

    return unbalAndTangentMod.getResidual();
}




const XC::Vector &XC::TransformationFE::getTangForce(const XC::Vector &disp, double fact)
  {
    std::cerr << "XC::TransformationFE::getTangForce() - not yet implemented\n";
    unbalAndTangentMod.getResidual().Zero();
    return unbalAndTangentMod.getResidual();
  }

const XC::Vector &XC::TransformationFE::getK_Force(const XC::Vector &accel, double fact)
{
  this->FE_Element::zeroTangent();    
  this->FE_Element::addKtToTang();    
  const Matrix &theTangent = this->XC::FE_Element::getTangent(0);

  static IntPtrWrapper numDOFs(dofData.getDataPtr(), 1);
  numDOFs.setData(dofData.getDataPtr(), numGroups);
    
  // DO THE SP STUFF TO THE TANGENT 
  
  // get the transformation matrix from each dof group & number of local dof
  // for original node.
  int numNode = numGroups;
  for(int a = 0; a<numNode; a++)
    {
      Matrix *theT = theDOFs[a]->getT();
      theTransformations[a] = theT;
      if(theT != 0)
        numDOFs[a] = theT->noRows(); // T^ 
      else
        numDOFs[a] = theDOFs[a]->getNumDOF();
    }
  
  // perform Tt K T -- as T is block diagonal do T(i)^T K(i,j) T(j)
  // where blocks are of size equal to num ele dof at a node
  
  int startRow = 0;
  int noRowsTransformed = 0;
  int noRowsOriginal = 0;
  
  static XC::Matrix localK;
  
  // foreach block row, for each block col do
  for(int i=0; i<numNode; i++) {
    
    int startCol = 0;
    int numDOFi = numDOFs[i];        
    int noColsOriginal = 0;
    
    for(int j=0; j<numNode; j++)
      {
        const Matrix *Ti= theTransformations[i];
        const Matrix *Tj= theTransformations[j];
      int numDOFj = numDOFs[j];        
      localK.setData(localKbuffer.getDataPtr(), numDOFi, numDOFj);
      
      // copy K(i,j) into localK matrix
      // CHECK SIZE OF BUFFFER            
      for(int a=0; a<numDOFi; a++)
        for(int b=0; b<numDOFj; b++)
          localK(a,b) = theTangent(noRowsOriginal+a, noColsOriginal+b);
      
      // now perform the matrix computation T(i)^T localK T(j)
      // note: if T == 0 then the Identity is assumed
      int noColsTransformed = 0;
      static XC::Matrix localTtKT;
      
      if(Ti != 0 && Tj != 0) {
        noRowsTransformed = Ti->noCols();
        noColsTransformed = Tj->noCols();
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = (*Ti) ^ localK * (*Tj);
      } else if(Ti == 0 && Tj != 0) {
        noRowsTransformed = numDOFi;
        noColsTransformed = Tj->noCols();
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        // localTtKT = localK * (*Tj);               
        localTtKT.addMatrixProduct(0.0, localK, *Tj, 1.0);
      } else if(Ti != 0 && Tj == 0) {
        noRowsTransformed = Ti->noCols();
        noColsTransformed = numDOFj;
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = (*Ti) ^ localK;
      } else {
        noRowsTransformed = numDOFi;
        noColsTransformed = numDOFj;
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = localK;
      }
      // now copy into modTangent the T(i)^t K(i,j) T(j) product
      for(int c=0; c<noRowsTransformed; c++) 
        for(int d=0; d<noColsTransformed; d++) 
          (unbalAndTangentMod.getTangent())(startRow+c, startCol+d) = localTtKT(c,d);
      
      startCol += noColsTransformed;
      noColsOriginal += numDOFj;
    }
    
    noRowsOriginal += numDOFi;
    startRow += noRowsTransformed;
  }
  
  // get the components we need out of the vector
  // and place in a temporary vector
  Vector tmp(numTransformedDOF);
  for(int j=0; j<numTransformedDOF; j++) {
    int dof = modID(j);
    if(dof >= 0)
      tmp(j) = accel(dof);
    else
      tmp(j) = 0.0;
  }

  unbalAndTangentMod.getResidual().addMatrixVector(0.0, unbalAndTangentMod.getTangent(), tmp, 1.0);

  return unbalAndTangentMod.getResidual();
}

const XC::Vector &XC::TransformationFE::getM_Force(const Vector &accel, double fact)
  {
    this->FE_Element::zeroTangent();    
    this->FE_Element::addMtoTang();    
    const Matrix &theTangent = this->FE_Element::getTangent(0);

    static IntPtrWrapper numDOFs(dofData.getDataPtr(), 1);
    numDOFs.setData(dofData.getDataPtr(), numGroups);
    
    // DO THE SP STUFF TO THE TANGENT 
  
    // get the transformation matrix from each dof group & number of local dof
    // for original node.
    int numNode = numGroups;
    for(int a = 0; a<numNode; a++)
      {
        Matrix *theT = theDOFs[a]->getT();
        theTransformations[a] = theT;
        if(theT != 0)
          numDOFs[a] = theT->noRows(); // T^ 
        else
          numDOFs[a] = theDOFs[a]->getNumDOF();
      }
  
    // perform Tt K T -- as T is block diagonal do T(i)^T K(i,j) T(j)
    // where blocks are of size equal to num ele dof at a node
  
    int startRow = 0;
    int noRowsTransformed = 0;
    int noRowsOriginal = 0;
  
    static Matrix localK;
  
    // foreach block row, for each block col do
    for(int i=0; i<numNode; i++)
      {
    
        int startCol = 0;
        int numDOFi = numDOFs[i];        
        int noColsOriginal = 0;
    
        for(int j=0; j<numNode; j++)
          {
            const Matrix *Ti= theTransformations[i];
            const Matrix *Tj= theTransformations[j];
            int numDOFj = numDOFs[j];        
            localK.setData(localKbuffer.getDataPtr(), numDOFi, numDOFj);
      
            // copy K(i,j) into localK matrix
            // CHECK SIZE OF BUFFFER            
            for(int a=0; a<numDOFi; a++)
              for(int b=0; b<numDOFj; b++)
                localK(a,b) = theTangent(noRowsOriginal+a, noColsOriginal+b);
      
            // now perform the matrix computation T(i)^T localK T(j)
            // note: if T == 0 then the Identity is assumed
            int noColsTransformed = 0;
            static Matrix localTtKT;
      
      if(Ti != 0 && Tj != 0) {
        noRowsTransformed = Ti->noCols();
        noColsTransformed = Tj->noCols();
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = (*Ti) ^ localK * (*Tj);
      } else if(Ti == 0 && Tj != 0) {
        noRowsTransformed = numDOFi;
        noColsTransformed = Tj->noCols();
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        // localTtKT = localK * (*Tj);               
        localTtKT.addMatrixProduct(0.0, localK, *Tj, 1.0);
      } else if(Ti != 0 && Tj == 0) {
        noRowsTransformed = Ti->noCols();
        noColsTransformed = numDOFj;
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = (*Ti) ^ localK;
      } else {
        noRowsTransformed = numDOFi;
        noColsTransformed = numDOFj;
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = localK;
      }
      // now copy into modTangent the T(i)^t K(i,j) T(j) product
      for(int c=0; c<noRowsTransformed; c++) 
        for(int d=0; d<noColsTransformed; d++) 
          (unbalAndTangentMod.getTangent())(startRow+c, startCol+d) = localTtKT(c,d);
      
      startCol += noColsTransformed;
      noColsOriginal += numDOFj;
    }
    
    noRowsOriginal += numDOFi;
    startRow += noRowsTransformed;
  }
  
  // get the components we need out of the vector
  // and place in a temporary vector
  Vector tmp(numTransformedDOF);
  for(int j=0; j<numTransformedDOF; j++) {
    int dof = modID(j);
    if(dof >= 0)
      tmp(j) = accel(dof);
    else
      tmp(j) = 0.0;
  }

  unbalAndTangentMod.getResidual().addMatrixVector(0.0, unbalAndTangentMod.getTangent(), tmp, 1.0);

  return unbalAndTangentMod.getResidual();
}

const XC::Vector &XC::TransformationFE::getC_Force(const XC::Vector &accel, double fact)
{
  this->FE_Element::zeroTangent();    
  this->FE_Element::addCtoTang();    
  const Matrix &theTangent = this->XC::FE_Element::getTangent(0);

  static IntPtrWrapper numDOFs(dofData.getDataPtr(), 1);
  numDOFs.setData(dofData.getDataPtr(), numGroups);
    
  // DO THE SP STUFF TO THE TANGENT 
  
  // get the transformation matrix from each dof group & number of local dof
  // for original node.
  int numNode = numGroups;
  for(int a = 0; a<numNode; a++)
    {
      Matrix *theT = theDOFs[a]->getT();
      theTransformations[a] = theT;
      if(theT != 0)
        numDOFs[a] = theT->noRows(); // T^ 
      else
        numDOFs[a] = theDOFs[a]->getNumDOF();
    }
  
  // perform Tt K T -- as T is block diagonal do T(i)^T K(i,j) T(j)
  // where blocks are of size equal to num ele dof at a node
  
  int startRow = 0;
  int noRowsTransformed = 0;
  int noRowsOriginal = 0;
  
  static XC::Matrix localK;
  
  // foreach block row, for each block col do
  for(int i=0; i<numNode; i++) {
    
    int startCol = 0;
    int numDOFi = numDOFs[i];        
    int noColsOriginal = 0;
    
    for(int j=0; j<numNode; j++)
      {
        const Matrix *Ti= theTransformations[i];
        const Matrix *Tj= theTransformations[j];
      int numDOFj = numDOFs[j];        
      localK.setData(localKbuffer.getDataPtr(), numDOFi, numDOFj);
      
      // copy K(i,j) into localK matrix
      // CHECK SIZE OF BUFFFER            
      for(int a=0; a<numDOFi; a++)
        for(int b=0; b<numDOFj; b++)
          localK(a,b) = theTangent(noRowsOriginal+a, noColsOriginal+b);
      
      // now perform the matrix computation T(i)^T localK T(j)
      // note: if T == 0 then the Identity is assumed
      int noColsTransformed = 0;
      static XC::Matrix localTtKT;
      
      if(Ti != 0 && Tj != 0) {
        noRowsTransformed = Ti->noCols();
        noColsTransformed = Tj->noCols();
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = (*Ti) ^ localK * (*Tj);
      } else if(Ti == 0 && Tj != 0) {
        noRowsTransformed = numDOFi;
        noColsTransformed = Tj->noCols();
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        // localTtKT = localK * (*Tj);               
        localTtKT.addMatrixProduct(0.0, localK, *Tj, 1.0);
      } else if(Ti != 0 && Tj == 0) {
        noRowsTransformed = Ti->noCols();
        noColsTransformed = numDOFj;
        // CHECK SIZE OF BUFFFER
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = (*Ti) ^ localK;
      } else {
        noRowsTransformed = numDOFi;
        noColsTransformed = numDOFj;
        localTtKT.setData(dataBuffer.getDataPtr(), noRowsTransformed, noColsTransformed);
        localTtKT = localK;
      }
      // now copy into modTangent the T(i)^t K(i,j) T(j) product
      for(int c=0; c<noRowsTransformed; c++) 
        for(int d=0; d<noColsTransformed; d++) 
          (unbalAndTangentMod.getTangent())(startRow+c, startCol+d) = localTtKT(c,d);
      
      startCol += noColsTransformed;
      noColsOriginal += numDOFj;
    }
    
    noRowsOriginal += numDOFi;
    startRow += noRowsTransformed;
  }
  
  // get the components we need out of the vector
  // and place in a temporary vector
  Vector tmp(numTransformedDOF);
  for(int j=0; j<numTransformedDOF; j++) {
    int dof = modID(j);
    if(dof >= 0)
      tmp(j) = accel(dof);
    else
      tmp(j) = 0.0;
  }

  unbalAndTangentMod.getResidual().addMatrixVector(0.0, unbalAndTangentMod.getTangent(), tmp, 1.0);

  return unbalAndTangentMod.getResidual();
}


void XC::TransformationFE::addD_Force(const XC::Vector &disp,  double fact)
  {
    if(fact == 0.0)
      return;

    static Vector response;
    response.setData(dataBuffer.getDataPtr(), numOriginalDOF);
                    
    for(int i=0; i<numTransformedDOF; i++) {
        int loc = modID(i);
        if(loc >= 0)
            (unbalAndTangentMod.getResidual())(i) = disp(loc);
        else
            (unbalAndTangentMod.getResidual())(i) = 0.0;
    }
    transformResponse(unbalAndTangentMod.getResidual(), response);
    this->addLocalD_Force(response, fact);
  }            

void XC::TransformationFE::addM_Force(const XC::Vector &disp,  double fact)
  {
    if(fact == 0.0)
        return;

    static Vector response;
    response.setData(dataBuffer.getDataPtr(), numOriginalDOF);
                    
    for(int i=0; i<numTransformedDOF; i++) {
        int loc = modID(i);
        if(loc >= 0)
            (unbalAndTangentMod.getResidual())(i) = disp(loc);
        else
            (unbalAndTangentMod.getResidual())(i) = 0.0;
    }
    transformResponse(unbalAndTangentMod.getResidual(), response);
    this->addLocalM_Force(response, fact);
  }            



// CHANGE THE XC::ID SENT
const XC::Vector &XC::TransformationFE::getLastResponse(void)
  {
    Integrator *theLastIntegrator = this->getLastIntegrator();
    if(theLastIntegrator != 0) {
        if(theLastIntegrator->getLastResponse(unbalAndTangentMod.getResidual(),modID) < 0) {
            std::cerr << "WARNING XC::TransformationFE::getLastResponse(void)";
            std::cerr << " - the XC::Integrator had problems with getLastResponse()\n";
        }
    }
    else {
        unbalAndTangentMod.getResidual().Zero();
        std::cerr << "WARNING  XC::TransformationFE::getLastResponse()";
        std::cerr << " No XC::Integrator yet passed\n";
    }
    
    Vector &result = unbalAndTangentMod.getResidual();
    return result;
  }


int XC::TransformationFE::transformResponse(const XC::Vector &modResp, 
                                    Vector &unmodResp)
  {
    // perform T R  -- as T is block diagonal do T(i) R(i)
    // where blocks are of size equal to num ele dof at a node

    int startRowOriginal = 0;
    int startRowTransformed = 0;
    int numNode = numGroups;
    int noRows = 0;
    int noCols = 0;

    for(int i=0; i<numNode; i++) {
        const Matrix *Ti = theDOFs[i]->getT();
        if(Ti != 0) {
            noRows = Ti->noRows();
            noCols = Ti->noCols();
            for(int j=0; j<noRows; j++) {
                double sum = 0.0;
                for(int k=0; k<noCols; k++)
                    sum += (*Ti)(j,k) * modResp(startRowTransformed +k) ;
                unmodResp(startRowOriginal + j) = sum;
            }
        } else {
            noCols = theDOFs[i]->getNumDOF();
            noRows = noCols;
            for(int j=0; j<noCols; j++)
                unmodResp(startRowOriginal + j) = modResp(startRowTransformed +j);
        }
        startRowOriginal += noRows;
        startRowTransformed += noCols;
    }

    return 0;
}


// AddingSensitivity:BEGIN /////////////////////////////////
void XC::TransformationFE::addD_ForceSensitivity(int gradNumber, const XC::Vector &disp,  double fact)
{
    if(fact == 0.0)
        return;

    static Vector response;
    response.setData(dataBuffer.getDataPtr(), numOriginalDOF);
                    
    for(int i=0; i<numTransformedDOF; i++) {
        int loc = modID(i);
        if(loc >= 0)
            (unbalAndTangentMod.getResidual())(i) = disp(loc);
        else
            (unbalAndTangentMod.getResidual())(i) = 0.0;
    }
    transformResponse(unbalAndTangentMod.getResidual(), response);
    this->addLocalD_ForceSensitivity(gradNumber, response, fact);
}            

void XC::TransformationFE::addM_ForceSensitivity(int gradNumber, const XC::Vector &disp,  double fact)
  {
    if(fact == 0.0)
        return;

    static Vector response;
    response.setData(dataBuffer.getDataPtr(), numOriginalDOF);
                    
    for(int i=0; i<numTransformedDOF; i++) {
        int loc = modID(i);
        if(loc >= 0)
            (unbalAndTangentMod.getResidual())(i) = disp(loc);
        else
            (unbalAndTangentMod.getResidual())(i) = 0.0;
    }
    transformResponse(unbalAndTangentMod.getResidual(), response);
    this->addLocalM_ForceSensitivity(gradNumber, response, fact);
  }            

// AddingSensitivity:END ////////////////////////////////////
