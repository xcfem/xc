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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/03/17 19:33:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/TrussSection.h,v $
                                                                        
                                                                        
#ifndef TrussSection_h
#define TrussSection_h

// File: ~/element/truss/TrussSection.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for Truss. A Truss 
// object provides the abstraction of the small deformation bar element. 
// Each truss object is assocaited with a section object. This Truss element 
// will work in 1d, 2d or 3d problems.
//
// What: "@(#) Truss.h, revA"

#include "TrussBase.h"
#include <utility/matrix/Matrix.h>

namespace XC {
class Channel;
class SectionForceDeformation;

//! \ingroup ElemBarraArt
//
//! @brief Elemento de tipo barra articulada con material de tipo SectionForceDeformation.
class TrussSection : public TrussBase
  {
  private:
    SectionForceDeformation  *theSection;
    double computeCurrentStrain(void) const;    
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    TrussSection(int tag, int dimension, int Nd1, int Nd2, SectionForceDeformation &theSection);
    TrussSection(int tag,int dimension,const Material *ptr_mat);    
    TrussSection(void);    
    Element *getCopy(void) const;
    ~TrussSection(void);

    // public methods to obtain inforrmation about dof & connectivity    
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);

    const Material *getMaterial(void) const;
    Material *getMaterial(void);
    virtual double getRho(void) const;

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    
    
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter (int parameterID, Information &info);
  };
} // fin namespace XC

#endif
