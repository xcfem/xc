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
** (C) Copyright 2001, The Regents of the University of California    **
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
                                                                        
// $Revision: 1.7 $
// $Date: 2008/08/26 15:38:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/Parameter.h,v $

#ifndef Parameter_h
#define Parameter_h

#include "utility/tagged/TaggedObject.h"
#include "domain/mesh/element/Information.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {

class DomainComponent;
class Channel;
class FEM_ObjectBroker;
class Domain;

class Parameter : public TaggedObject, public MovableObject
  {
  private:
    Information theInfo;
    double currentValue;

    enum {initialSize = 64};
    enum {expandSize = 128};

    std::vector<DomainComponent *> theComponents;
    int numComponents;
    int maxNumComponents;

    std::vector<MovableObject *> theObjects;
    int numObjects;
    int maxNumObjects;

    std::vector<int> parameterID;

    int gradIndex; // 0,...,nparam-1
  public:
    Parameter(int tag, DomainComponent *theObject, const std::vector<std::string> &);
    Parameter(const Parameter &param);
    Parameter(int tag, int classTag = PARAMETER_TAG_Parameter);
    Parameter();
  
    virtual void Print(std::ostream &s, int flag =0);
  
    virtual int update(int newValue); 
    virtual int update(double newValue); 
    virtual int activate(bool active);
    virtual double getValue(void) {return theInfo.theDouble;}
    virtual void setValue(double newValue) {theInfo.theDouble = newValue;}

    virtual int addComponent(DomainComponent *theObject, const std::vector<std::string> &);  
    virtual int addObject(int parameterID, MovableObject *object);

    void setGradIndex(int gradInd) {gradIndex = gradInd;}
    int getGradIndex(void) {return gradIndex;}

    virtual void setDomain(Domain *theDomain);
    virtual int sendSelf(CommParameters &);  
    virtual int recvSelf(const CommParameters &);
  };
} // fin namespace XC

#endif
