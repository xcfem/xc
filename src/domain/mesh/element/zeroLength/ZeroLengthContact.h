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
//ZeroLengthContact.h

#ifndef ZeroLengthContact_h
#define ZeroLengthContact_h

#include "domain/mesh/element/Element0D.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class Node;
class Channel;
class Response;

//! \ingroup ElemZL
//
//! @brief Elemento de longitud cero para problemas de contacto.
class ZeroLengthContact: public Element0D
  {
  protected:
    // contact forces
    mutable double pressure; // contact pressure at n+1
    double t1; //!< friction in local dir1
    double t2; //!< friction in local dir2

  

    // parameters
    mutable double gap; //!< gap of time n+1 step
    double gap_n; //!< gap of time n step
    double Kn; //!< normal penalty
    double Kt; //!< tangential penalty
    double fs; //!< friction ratio

    // Normal and Tangental Vectors for Elemental Nodes, (4*1)
    mutable Vector N;

    mutable int ContactFlag; //!< 0: not contact; 1: stick; 2: slide
  
    
  public:
    ZeroLengthContact(int tag= 0,int classTag= 0, int dim= 2);
    ZeroLengthContact(int tag, int classTag, int dim, int Nd1, int Nd2,double Kn, double Kt, double fRatio);

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &);

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &os, int flag =0);
  };

} // fin namespace XC


#endif











