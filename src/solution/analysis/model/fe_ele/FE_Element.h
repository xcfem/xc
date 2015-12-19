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
                                                                        
// $Revision: 1.13 $
// $Date: 2005/11/29 22:46:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/FE_Element.h,v $
                                                                        
                                                                        
#ifndef FE_Element_h
#define FE_Element_h

// File: ~/analysis/model/fe_ele/FE_Element.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for FE_Element.
//
// What: "@(#) FE_Element.h, revA"

#include <utility/tagged/TaggedObject.h>
#include "utility/matrix/ID.h"
#include "solution/analysis/UnbalAndTangent.h"

namespace XC {
class TransientIntegrator;
class Element;
class Vector;
class Matrix;
class Integrator;
class AnalysisModel;

//! @ingroup Analisis
//
//! @defgroup AnalisisFE Coacciones para los grados de libertad del análisis.
//
//! @ingroup AnalisisFE
//
//! @brief ??.
class FE_Element: public TaggedObject
  {
  private:
    int numDOF;
    UnbalAndTangent unbalAndTangent;
    AnalysisModel *theModel;
    Element *myEle;
    Integrator *theIntegrator; // need for Subdomain

    // static variables - single copy for all objects of the class	
    static Matrix errMatrix;
    static Vector errVector;
    static UnbalAndTangentStorage unbalAndTangentArray; //!< array of class wide vectors and matrices
    static int numFEs; //!< number of objects
    void set_pointers(void);

  protected:
    void  addLocalM_Force(const Vector &accel, double fact = 1.0);    
    void  addLocalD_Force(const Vector &vel, double fact = 1.0);    
    void  addLocalM_ForceSensitivity(int gradNumber, const Vector &accel, double fact = 1.0);    
    void  addLocalD_ForceSensitivity(int gradNumber, const Vector &vel, double fact = 1.0);    


    // protected variables - a copy for each object of the class        
    ID myDOF_Groups;
    ID myID;

    friend class AnalysisModel;
    FE_Element(int tag, Element *theElement);
    FE_Element(int tag, int numDOF_Group, int ndof);
  public:
    virtual ~FE_Element(void);    

    // public methods for setting/obtaining mapping information
    virtual const ID &getDOFtags(void) const;
    virtual const ID &getID(void) const;
    void setAnalysisModel(AnalysisModel &theModel);
    virtual int  setID(void);
    
    // methods to form and obtain the tangent and residual
    virtual const Matrix &getTangent(Integrator *theIntegrator);
    virtual const Vector &getResidual(Integrator *theIntegrator);

    // methods to allow integrator to build tangent
    virtual void  zeroTangent(void);
    virtual void  addKtToTang(double fact = 1.0);
    virtual void  addKiToTang(double fact = 1.0);
    virtual void  addCtoTang(double fact = 1.0);    
    virtual void  addMtoTang(double fact = 1.0);    
    
    // methods to allow integrator to build residual    
    virtual void  zeroResidual(void);    
    virtual void  addRtoResidual(double fact = 1.0);
    virtual void  addRIncInertiaToResidual(double fact = 1.0);    

    // methods for ele-by-ele strategies
    virtual const Vector &getTangForce(const Vector &x, double fact = 1.0);
    virtual const Vector &getK_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getC_Force(const Vector &x, double fact = 1.0);
    virtual const Vector &getM_Force(const Vector &x, double fact = 1.0);
    virtual void  addM_Force(const Vector &accel, double fact = 1.0);    
    virtual void  addD_Force(const Vector &vel, double fact = 1.0);    

    virtual int updateElement(void);

    virtual Integrator *getLastIntegrator(void);
    virtual const Vector &getLastResponse(void);
    Element *getElement(void);

    virtual void Print(std::ostream &, int = 0) {return;};

    // AddingSensitivity:BEGIN ////////////////////////////////////
    virtual void addResistingForceSensitivity(int gradNumber, double fact = 1.0);
    virtual void addM_ForceSensitivity(int gradNumber, const Vector &vect, double fact = 1.0);
    virtual void addD_ForceSensitivity(int gradNumber, const Vector &vect, double fact = 1.0);
    virtual int  commitSensitivity(int gradNum, int numGrads);
    // AddingSensitivity:END //////////////////////////////////////
    
  };
} // end of XC namespace

#endif


