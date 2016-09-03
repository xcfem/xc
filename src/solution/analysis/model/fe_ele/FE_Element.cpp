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

// $Revision: 1.17 $
// $Date: 2005/11/29 22:46:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/FE_Element.cpp,v $


// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::FE_Element class interface.

#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <cstdlib>

#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/NodePtrsWithIDs.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

const int MAX_NUM_DOF= 64;

// static variables initialisation
XC::Matrix XC::FE_Element::errMatrix(1,1);
XC::Vector XC::FE_Element::errVector(1);
XC::UnbalAndTangentStorage XC::FE_Element::unbalAndTangentArray(MAX_NUM_DOF+1);
int XC::FE_Element::numFEs(0);           // number of objects


//! @brief set the pointers for the tangent and residual
void XC::FE_Element::set_pointers(void)
  {
    if(myEle->isSubdomain() == false)
      {
        unbalAndTangent= UnbalAndTangent(numDOF,unbalAndTangentArray);
      }
    else
      {

        // as subdomains have own matrix for tangent and residual don't need
        // to set matrix and vector pointers to these objects
        //theResidual= Vector(numDOF);
         // invoke setFE_ElementPtr() method on Subdomain
        Subdomain *theSub= dynamic_cast<Subdomain *>(myEle);
        theSub->setFE_ElementPtr(this);
      }
  }

// FE_Element(Element *, Integrator *theIntegrator);
// construictor that take the corresponding model element.
XC::FE_Element::FE_Element(int tag, Element *ele)
  :TaggedObject(tag),numDOF(ele->getNumDOF()),unbalAndTangent(0,unbalAndTangentArray),
   theModel(nullptr), myEle(ele), theIntegrator(nullptr),
   myDOF_Groups((ele->getNodePtrs().getExternalNodes()).Size()), myID(ele->getNumDOF())
  {
    if(numDOF <= 0)
      {
        std::cerr << "XC::FE_Element::FE_Element(Element *) ";
        std::cerr << " element must have 1 dof " << *ele;
        exit(-1);
      }

    // get elements domain & check it is valid
    Domain *theDomain= ele->getDomain();
    if(!theDomain)
      {
        std::cerr << "FATAL XC::FE_Element::FE_Element() - element has no domain "<< *ele;
        exit(-1);
      }

    // keep a pointer to all DOF_Groups
    int numGroups= ele->getNumExternalNodes();
    const ID &nodes= ele->getNodePtrs().getExternalNodes();

    for(int i=0; i<numGroups; i++)
      {
        Node *nodePtr =theDomain->getNode(nodes(i));
        if(nodePtr == 0)
          {
            std::cerr << "FATAL XC::FE_Element::FE_Element() - Node: ";
            std::cerr <<  nodes(i) <<  "does not exist in the XC::Domain\n";
            std::cerr << *ele;
            exit(-1);
          }

        DOF_Group *dofGrpPtr= nodePtr->getDOF_GroupPtr();
        if(dofGrpPtr != 0)
          myDOF_Groups(i)= dofGrpPtr->getTag();
        else
          {
            std::cerr << "FATAL XC::FE_Element::FE_Element() - Node: ";
            std::cerr <<  *nodePtr <<  " has no XC::DOF_Group associated with it\n";
            exit(-1);
          }
      }

    set_pointers();
    // increment number of FE_Elements by 1
    numFEs++;
  }


XC::FE_Element::FE_Element(int tag, int numDOF_Group, int ndof)
  :TaggedObject(tag),numDOF(ndof),unbalAndTangent(ndof,unbalAndTangentArray),
    theModel(nullptr), myEle(nullptr), theIntegrator(nullptr),
    myDOF_Groups(numDOF_Group), myID(ndof)
  {
    // this is for a subtype, the subtype must set the myDOF_Groups ID array
    numFEs++;

    // as subtypes have no access to the tangent or residual we don't set them
    // this way we can detect if subclass does not provide all methods it should
  }


//! @brief destructor.
XC::FE_Element::~FE_Element(void)
  {
    // decrement number of FE_Elements
    numFEs--;
    theModel= nullptr;
    myEle= nullptr;
    theIntegrator= nullptr; 
  }


const XC::ID &XC::FE_Element::getDOFtags(void) const
  { return myDOF_Groups; }


// const XC::ID &getID(void) const;
//        Method to return the current XC::ID.

const XC::ID &XC::FE_Element::getID(void) const
  { return myID; }

void XC::FE_Element::setAnalysisModel(AnalysisModel &theAnalysisModel)
  { theModel= &theAnalysisModel; }

// void setID(int index, int value);
//        Method to set the corresponding index of the XC::ID to value.

int XC::FE_Element::setID(void)
  {
    if(!theModel)
      {
        std::cerr << "WARNING XC::FE_Element::setID() - no XC::AnalysisModel set\n";
        return -1;
      }

    int numGrps= myDOF_Groups.Size();
    int current= 0;
    for(int i=0;i<numGrps;i++)
      {
        int tag= myDOF_Groups(i);
        DOF_Group *dofPtr= theModel->getDOF_GroupPtr(tag);
        if(!dofPtr)
          {
            std::cerr << "WARNING XC::FE_Element::setID: 0 XC::DOF_Group Pointer\n";
            return -2;
          }

        const XC::ID &theDOFid= dofPtr->getID();

        for(int j=0; j<theDOFid.Size(); j++)
          if(current < numDOF)
            myID(current++)= theDOFid(j);
          else
            {
              std::cerr << "WARNING XC::FE_Element::setID() - numDOF and";
              std::cerr << " number of dof at the DOF_Groups\n";
              return -3;
            }
      }
    return 0;
  }

//! @brief Devuelve la matriz tangente que le indica el objeto Integrator.
const XC::Matrix &XC::FE_Element::getTangent(Integrator *theNewIntegrator)
  {
    theIntegrator= theNewIntegrator;

    if(myEle == 0)
      {
        std::cerr << "FATAL XC::FE_Element::getTangent() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation - ";
        std::cerr << " - a 1x1 error matrix will be returned.\n";
        exit(-1);
      }

    if(myEle->isSubdomain() == false)
      {
        if(theNewIntegrator)
          theNewIntegrator->formEleTangent(this);
        return unbalAndTangent.getTangent();
      }
    else
      {
        Subdomain *theSub= dynamic_cast<Subdomain *>(myEle);
        theSub->computeTang();
        return theSub->getTang();
      }
  }

//! @brief Devuelve el vector residuo que le indica el objeto Integrator.
const XC::Vector &XC::FE_Element::getResidual(Integrator *theNewIntegrator)
  {
    theIntegrator= theNewIntegrator;

    if(!theIntegrator)
      return unbalAndTangent.getResidual();

    if(!myEle)
      {
        std::cerr << "FATAL FE_Element::getTangent() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation - ";
        std::cerr << " - an error XC::Vector of order 1 will be returned.\n";
        exit(-1);
      }
    else
      {
        if(myEle->isSubdomain() == false)
          {
            theNewIntegrator->formEleResidual(this);
            return unbalAndTangent.getResidual();
          }
        else
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(myEle);
            theSub->computeResidual();
            return theSub->getResistingForce();
          }
      }
  }



void XC::FE_Element::zeroTangent(void)
  {
    if(myEle)
      {
        if(myEle->isSubdomain() == false)
          unbalAndTangent.getTangent().Zero();
        else
          {
            std::cerr << "WARNING XC::FE_Element::zeroTangent() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::zeroTangent() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addKtToTang(double fact)
  {
    if(myEle != 0)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        else if(myEle->isSubdomain() == false)
          unbalAndTangent.getTangent().addMatrix(1.0, myEle->getTangentStiff(),fact);
        else
          {
            std::cerr << "WARNING XC::FE_Element::addKToTang() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else 
      {
        std::cerr << "WARNING XC::FE_Element::addKToTang() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addCtoTang(double fact)
  {
    if(myEle != 0)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        else if(myEle->isSubdomain() == false)
          unbalAndTangent.getTangent().addMatrix(1.0, myEle->getDamp(),fact);
        else
          {
            std::cerr << "WARNING XC::FE_Element::addCToTang() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addCToTang() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addMtoTang(double fact)
  {
    if(myEle != 0)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        else if(myEle->isSubdomain() == false)
          {
            const Matrix &masas= myEle->getMass();
            unbalAndTangent.getTangent().addMatrix(1.0, masas,fact);
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addMtoTang() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addMtoTang() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }


void XC::FE_Element::addKiToTang(double fact)
  {
  if(myEle != 0)
    {
    // check for a XC::quick return
    if(fact == 0.0)
      return;
    else if(myEle->isSubdomain() == false)
      unbalAndTangent.getTangent().addMatrix(1.0, myEle->getInitialStiff(), fact);
    else
      {
        std::cerr << "WARNING XC::FE_Element::addKiToTang() - ";
        std::cerr << "- this should not be called on a XC::Subdomain!\n";
      }
    }
  else
    {
      std::cerr << "WARNING XC::FE_Element::addKiToTang() - no XC::Element *given ";
      std::cerr << "- subclasses must provide implementation\n";
    }
  }


void XC::FE_Element::zeroResidual(void)
  {
    if(myEle != 0)
      {
        if(myEle->isSubdomain() == false)
          unbalAndTangent.getResidual().Zero();
        else
          {
            std::cerr << "WARNING XC::FE_Element::zeroResidual() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::zeroResidual() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addRtoResidual(double fact)
  {
    if(myEle != 0)
      {
        // check for a quick return
        if(fact == 0.0)
            return;
        else if(myEle->isSubdomain() == false)
          {
            const Vector &eleResisting= myEle->getResistingForce();
            unbalAndTangent.getResidual().addVector(1.0, eleResisting, -fact);
          }
        else 
          {
            std::cerr << "WARNING XC::FE_Element::addRtoResidual() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addRtoResidual() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }


void XC::FE_Element::addRIncInertiaToResidual(double fact)
  {
    if(myEle)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        else if(myEle->isSubdomain() == false)
          {
            const Vector &eleResisting= myEle->getResistingForceIncInertia();
            unbalAndTangent.getResidual().addVector(1.0, eleResisting, -fact);
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addRtoResidual() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addRtoResidual() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }


const XC::Vector &XC::FE_Element::getTangForce(const Vector &disp, double fact)
  {
    if(myEle != 0)
      {

        // zero out the force vector
        unbalAndTangent.getResidual().Zero();

        // check for a XC::quick return
        if(fact == 0.0)
          return unbalAndTangent.getResidual();

        // get the components we need out of the vector
        // and place in a temporary vector
        Vector tmp(numDOF);
        for(int i=0; i<numDOF; i++)
          {
            int dof= myID(i);
            if(dof >= 0)
              tmp(i)= disp(myID(i));
            else
              tmp(i)= 0.0;
          }

        if(myEle->isSubdomain() == false)
          {
            // form the tangent again and then add the force
            theIntegrator->formEleTangent(this);
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, unbalAndTangent.getTangent(),tmp,fact) < 0) 
              {
                std::cerr << "WARNING XC::FE_Element::getTangForce() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        else
          {
	    Subdomain *theSub= dynamic_cast<Subdomain *>(myEle);
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, theSub->getTang(),tmp,fact) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::getTangForce() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        return unbalAndTangent.getResidual();
      }
    else
      {
        std::cerr << "WARNING FE_Element::addTangForce() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
        return errVector;
      }
  }



const XC::Vector &XC::FE_Element::getK_Force(const Vector &disp, double fact)
  {
    if(myEle != 0)
      {
        // zero out the force vector
        unbalAndTangent.getResidual().Zero();

        // check for a XC::quick return
        if(fact == 0.0)
            return unbalAndTangent.getResidual();

        // get the components we need out of the vector
        // and place in a temporary vector
        Vector tmp(numDOF);
        for(int i=0; i<numDOF; i++)
          {
            int dof= myID(i);
            if(dof >= 0)
              tmp(i)= disp(myID(i));
            else
              tmp(i)= 0.0;
          }

        if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getTangentStiff(), tmp, fact) < 0)
          {
            std::cerr << "WARNING XC::FE_Element::getKForce() - ";
            std::cerr << "- addMatrixVector returned error\n";
          }
        return unbalAndTangent.getResidual();
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::getKForce() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
        return errVector;
      }
  }

const XC::Vector &XC::FE_Element::getM_Force(const Vector &disp, double fact)
  {

    if(myEle)
      {
        // zero out the force vector
        unbalAndTangent.getResidual().Zero();

        // check for a quick return
        if(fact == 0.0)
            return unbalAndTangent.getResidual();

        // get the components we need out of the vector
        // and place in a temporary vector
        Vector tmp(numDOF);
        for(int i=0; i<numDOF; i++)
          {
            int dof= myID(i);
            if(dof >= 0)
              tmp(i)= disp(myID(i));
            else
              tmp(i)= 0.0;
          }
        if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMass(), tmp, fact) < 0)
          {
            std::cerr << "WARNING XC::FE_Element::getMForce() - ";
            std::cerr << "- addMatrixVector returned error\n";
          }
        return unbalAndTangent.getResidual();
      }
    else
      {
        std::cerr << "WARNING FE_Element::getMForce() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
        return errVector;
      }
  }

const XC::Vector &XC::FE_Element::getC_Force(const XC::Vector &disp, double fact)
  {
    if(myEle)
      {
        // zero out the force vector
        unbalAndTangent.getResidual().Zero();

        // check for a XC::quick return
        if(fact == 0.0)
          return unbalAndTangent.getResidual();

        // get the components we need out of the vector
        // and place in a temporary vector
        Vector tmp(numDOF);
        for(int i=0; i<numDOF; i++)
          {
            const int dof= myID(i);
            if(dof >= 0)
              tmp(i)= disp(myID(i));
            else
              tmp(i)= 0.0;
          }
        if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDamp(), tmp, fact) < 0)
          {
            std::cerr << "WARNING XC::FE_Element::getDForce() - ";
            std::cerr << "- addMatrixVector returned error\n";
          }
        return unbalAndTangent.getResidual();
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::getDForce() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
        return errVector;
      }
  }



XC::Integrator *XC::FE_Element::getLastIntegrator(void)
  { return theIntegrator; }

const XC::Vector &XC::FE_Element::getLastResponse(void)
  {
    if(myEle)
      {
        if(theIntegrator)
          {
            if(theIntegrator->getLastResponse(unbalAndTangent.getResidual(),myID) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::getLastResponse(void)";
                std::cerr << " - the XC::Integrator had problems with getLastResponse()\n";
              }
          }
        else
          {
            unbalAndTangent.getResidual().Zero();
            std::cerr << "WARNING  XC::FE_Element::getLastResponse()";
            std::cerr << " No XC::Integrator yet passed\n";
          }
        Vector &result= unbalAndTangent.getResidual();
        return result;
      }
    else
      {
        std::cerr << "WARNING  XC::FE_Element::getLastResponse()";
        std::cerr << " No XC::Element passed in constructor\n";
        return errVector;
      }
  }

void XC::FE_Element::addM_Force(const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            // get the components we need out of the vector
            // and place in a temporary vector
            Vector tmp(numDOF);
            for(int i=0; i<numDOF; i++)
              {
                const int loc= myID(i);
                if(loc >= 0)
                  tmp(i)= accel(loc);
                else
                  tmp(i)= 0.0;
              }
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMass(), tmp, fact) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::addM_Force() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addM_Force() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addM_Force() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addD_Force(const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            // get the components we need out of the vector
            // and place in a temporary vector
            Vector tmp(numDOF);
            for(int i=0; i<numDOF; i++)
              {
                const int loc= myID(i);
                if(loc >= 0)
                  tmp(i)= accel(loc);
                else
                  tmp(i)= 0.0;
              }
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDamp(), tmp, fact) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::addD_Force() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addD_Force() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addD_Force() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addLocalM_Force(const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMass(),accel, fact) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::addLocalM_Force() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addLocalM_Force() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addLocalM_Force() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addLocalD_Force(const XC::Vector &accel, double fact)
  {
    if(myEle != 0)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDamp(),accel, fact) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::addLocalD_Force() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addLocalD_Force() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addLocalD_Force() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }


XC::Element *XC::FE_Element::getElement(void)
  { return myEle; }


// AddingSensitivity:BEGIN /////////////////////////////////
void XC::FE_Element::addResistingForceSensitivity(int gradNumber, double fact)
  { unbalAndTangent.getResidual().addVector(1.0, myEle->getResistingForceSensitivity(gradNumber), -fact); }

void XC::FE_Element::addM_ForceSensitivity(int gradNumber, const XC::Vector &vect, double fact)
  {
    // Get the components we need out of the vector
    // and place in a temporary vector
    Vector tmp(numDOF);
    for(int i=0; i<numDOF; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          { tmp(i)= vect(loc); }
        else
          { tmp(i)= 0.0; }
      }
    if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMassSensitivity(gradNumber),tmp,fact) < 0)
      {
        std::cerr << "WARNING XC::FE_Element::addM_ForceSensitivity() - ";
        std::cerr << "- addMatrixVector returned error\n";
      }
  }

void XC::FE_Element::addD_ForceSensitivity(int gradNumber, const XC::Vector &vect, double fact)
  {
    if(myEle)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            // get the components we need out of the vector
            // and place in a temporary vector
            Vector tmp(numDOF);
            for(int i=0; i<numDOF; i++)
              {
                int loc= myID(i);
                if(loc >= 0)
                  tmp(i)= vect(loc);
                else
                  tmp(i)= 0.0;
              }
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDampSensitivity(gradNumber), tmp, fact) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::addD_ForceSensitivity() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addD_ForceSensitivity() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addD_ForceSensitivity() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addLocalD_ForceSensitivity(int gradNumber, const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDampSensitivity(gradNumber),accel, fact) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::addLocalD_ForceSensitivity() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addLocalD_ForceSensitivity() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addLocalD_ForceSensitivity() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

void XC::FE_Element::addLocalM_ForceSensitivity(int gradNumber, const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a XC::quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMassSensitivity(gradNumber),accel, fact) < 0)
              {
                std::cerr << "WARNING XC::FE_Element::addLocalD_ForceSensitivity() - ";
                std::cerr << "- addMatrixVector returned error\n";
              }
          }
        else
          {
            std::cerr << "WARNING XC::FE_Element::addLocalD_ForceSensitivity() - ";
            std::cerr << "- this should not be called on a XC::Subdomain!\n";
          }
      }
    else
      {
        std::cerr << "WARNING XC::FE_Element::addLocalD_ForceSensitivity() - no XC::Element *given ";
        std::cerr << "- subclasses must provide implementation\n";
      }
  }

int XC::FE_Element::commitSensitivity(int gradNum, int numGrads)
  {
    myEle->commitSensitivity(gradNum, numGrads);
    return 0;
  }

// AddingSensitivity:END ////////////////////////////////////


int XC::FE_Element::updateElement(void)
  {
    if(myEle)
      return myEle->update();
    else
      return 0;
  }
