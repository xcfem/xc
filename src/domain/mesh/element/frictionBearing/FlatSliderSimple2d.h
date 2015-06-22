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

// $Revision: 1.2 $
// $Date: 2009/11/03 23:12:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/frictionBearing/FlatSliderSimple2d.h,v $

#ifndef FlatSliderSimple2d_h
#define FlatSliderSimple2d_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/06
// Revision: A
//
// Description: This file contains the class definition for FlatSliderSimple2d.
// FlatSliderSimple2d is a friction slider element defined by two nodes. This
// simplified version uses small angle approximations and accounts for the
// rotation of the sliding surface by shifting the shear force.

#include "ElemFriccionBase.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class Channel;
class FrictionModel;
class UniaxialMaterial;
class Response;

class FlatSliderSimple2d: public ElemFriccionBase
  {
  private:    
    // state variables
    double ubPlastic;   // plastic displacement in basic system
    
    // committed history variables
    double ubPlasticC;  // plastic displacement in basic system
    
    static Matrix theMatrix;
    static Vector theVector;
    // private methods
    void setUp();
    double sgn(double x);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // constructors
    FlatSliderSimple2d(int tag, int Nd1, int Nd2,FrictionModel &theFrnMdl, double uy,
		       const std::vector<UniaxialMaterial *> &theMaterials,
		       const Vector y= Vector(), const Vector x= Vector(),
        double mass = 0.0, int maxIter = 20, double tol = 1E-8);
    FlatSliderSimple2d();
    
    // public methods to obtain information about dof & connectivity    
    const ID &getExternalNodes() const;
    int getNumDOF();
    void setDomain(Domain *theDomain);
    
    // public methods to set the state of the element    
    int commitState();
    int revertToLastCommit();        
    int revertToStart();        
    int update();
    
    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff();
    const Matrix &getInitialStiff();
    const Matrix &getMass();
    
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    
    const Vector &getResistingForce();
    const Vector &getResistingForceIncInertia();
    
    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    int displaySelf(Renderer &theViewer, int displayMode, float fact);    
    void Print(std::ostream &s, int flag = 0);    
    
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  };
} // fin namespace XC

#endif
