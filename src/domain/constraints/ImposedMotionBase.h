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
//ImposedMotionBase.h

#ifndef ImposedMotionBase_h
#define ImposedMotionBase_h

#include <domain/constraints/SP_Constraint.h>
#include <utility/matrix/Vector.h>

namespace XC {
class GroundMotion;
class Node;
class LoadPattern;

//! @ingroup CContSP
//
//! @brief Clase base para los movimientos impuestos sobre nodo.
class ImposedMotionBase : public SP_Constraint
  {
  private:
    int groundMotionTag;
    int patternTag;

  protected:
    GroundMotion *theGroundMotion;  // pointer to ground motion
    Node *theNode; //!< pointer to node being constrained
    Vector theGroundMotionResponse; // the GMotions response
  public:
    // constructors    
    ImposedMotionBase(int classTag);        
    ImposedMotionBase(int classTag,int spTag, int nodeTag, int ndof, int patternTag, int theGroundMotionTag);

    // destructor
    ~ImposedMotionBase(void);

    void setDomain(Domain *theDomain);

    double getValue(void) const;
    bool isHomogeneous(void) const;

    int getMotion(void);

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // fin namespace XC

#endif
