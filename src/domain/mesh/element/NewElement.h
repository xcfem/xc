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
// $Date: 2003/03/11 20:40:07 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/NewElement.h,v $
                                                                        
#ifndef NewElement_h
#define NewElement_h

// Written: fmk 
// Created: 08/01
//
// Description: This file contains the class definition for NewElement. 
//
// What: "@(#) NewElement.h, revA"

#include <domain/mesh/element/Element.h>
#include "domain/mesh/element/NodePtrs.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Channel;
class UniaxialMaterial;

#define ELE_TAG_NewElement 100001

//! \ingroup Elem
//
//! @brief Prototipo de elemento.
class NewElement : public Element
  {
  private:
    mutable Matrix theMatrix; //!< matrix to return stiff, damp & mass
    Vector theVector; //!< vector to return the residual
  public:
    NewElement(int tag);
    NewElement();    

    // public methods to obtain inforrmation about dof & connectivity    
    NodePtrsWithIDs &getNodePtrs(void);
    const NodePtrsWithIDs &getNodePtrs(void) const;
    int getNumDOF(void) const;	
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);
    
    // public methods to obtain stiffness, mass, damping and 
    // residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;    

    void zeroLoad(void);	
    int addLoad(const Vector &addP);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInfo);
    int getResponse(int responseID, Information &eleInformation);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter (int parameterID, Information &info);
  };
} // fin namespace XC

#endif




