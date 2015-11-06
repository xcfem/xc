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
//ImposedMotionBase.cc

#include <domain/constraints/ImposedMotionBase.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>
#include <domain/load/pattern/LoadPattern.h>
#include "domain/mesh/node/Node.h"

//! @brief Constructor.
XC::ImposedMotionBase::ImposedMotionBase(int classTag)
:SP_Constraint(classTag), groundMotionTag(0), patternTag(0),
 theGroundMotion(nullptr), theNode(nullptr), theGroundMotionResponse(3) {}

// constructor for a subclass to use
XC::ImposedMotionBase::ImposedMotionBase(int classTag,int tag, int node, int ndof, int pattern, int motion)
:SP_Constraint(tag, node, ndof, classTag),
 groundMotionTag(motion), patternTag(pattern), theGroundMotion(nullptr), theNode(nullptr), theGroundMotionResponse(3) {}

XC::ImposedMotionBase::~ImposedMotionBase(void)
  {
    if(theNode)
      theNode->desconecta(this);
  }

void XC::ImposedMotionBase::setDomain(Domain *theDomain)
  {
    if(theDomain == nullptr)
      {
        std::cerr << "WARNING ImposedMotionBase::SetDomain: Specified domain does not exist";
        std::cerr << "Domain = 0\n";
        return;
      }
    SP_Constraint::setDomain(theDomain);

    if(theDomain)
      {
        // get a pointer to the retained and constrained nodes - make sure they exist
        theNode = theDomain->getNode(getNodeTag());
        if(!theNode)
          {
            std::cerr << "ImposedMotionBase::setup - retained node " <<  getNodeTag() <<  " not in domain\n";
          }
        else
          theNode->conecta(this);
      }
  }

bool XC::ImposedMotionBase::isHomogeneous(void) const
  { return true; }

//! @brief Devuelve el valor impuesto.
double XC::ImposedMotionBase::getValue(void) const
  {
    // always return 0.0 - applyConstraint() sets the values at XC::Node 
    return theGroundMotionResponse(0);
  }

int XC::ImposedMotionBase::getMotion(void)
  {
    if(!theNode)
      {
        std::cerr << "ImposedMotionBase::getMotion - puntero a nodo nulo."
                  <<  getNodeTag() <<  "\n";
      }
    if(!theGroundMotion)
      {

        LoadPattern *theLoadPattern = getDomain()->getConstraints().getLoadPattern(patternTag);
        if(theLoadPattern == 0)
          return -3;

        theGroundMotion = theLoadPattern->getMotion(groundMotionTag);
        if (theGroundMotion == 0)
          return -4;
      }
    return 0;
  }

//! @brief Envía los miembreos del objeto a través del canal que se pasa como parámetro.
int XC::ImposedMotionBase::sendData(CommParameters &cp)
  {
    int result= SP_Constraint::sendData(cp);
    result+= cp.sendInts(groundMotionTag,patternTag,getDbTagData(),CommMetaData(7));
    return result;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::ImposedMotionBase::recvData(const CommParameters &cp)
  {
    int res= SP_Constraint::recvData(cp);
    res+= cp.receiveInts(groundMotionTag,patternTag,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::ImposedMotionBase::sendSelf(CommParameters &cp)
  {
    static ID data(8);
    int result= sendData(cp);
    const int dbTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << "ImposedMotionBase::sendSelf() - failed to send extra data\n";
    return result;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::ImposedMotionBase::recvSelf(const CommParameters &cp)
  {
    static ID data(8);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ImposedMotionBase::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }
