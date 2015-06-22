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
//ZeroLengthContact.cpp

#include "ZeroLengthContact.h"
#include "domain/mesh/element/Information.h"
#include "utility/recorder/response/ElementResponse.h"
#include "domain/mesh/node/Node.h"


//! @brief Constructor.
XC::ZeroLengthContact::ZeroLengthContact(int tag, int classTag, int dim, int Nd1, int Nd2,double Knormal, double Ktangent, double frictionRatio)
  :Element0D(tag,classTag,Nd1,Nd2,dim), N(dim*numNodos())
  {
    // assign Kn, Kt, fs
    Kn = Knormal;
    Kt = Ktangent;
    fs = frictionRatio;

    // initialized contact flag be zero
    ContactFlag=0;

    gap_n  = 0 ;
    pressure=0; // add for augmented lagrange
  }

//! @brief Constructor por defecto.
XC::ZeroLengthContact::ZeroLengthContact(int tag,int classTag, int dim)
  :Element0D(tag,classTag,0,0,dim),
   N(dim*numNodos())
  {}

int XC::ZeroLengthContact::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    // meaningless to addLoad to a contact !
    if(isDead())
      std::cerr << nombre_clase() 
                << "; se intentó cargar el elemento "
                << getTag() << " que está desactivado." 
                << std::endl;
    return 0;
  }

int XC::ZeroLengthContact::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // does nothing as element has no mass yet!
    return 0;
  }

int XC::ZeroLengthContact::sendSelf(CommParameters &cp)
  {
    // doing nothing here
    return 0;
  }

int XC::ZeroLengthContact::recvSelf(const CommParameters &cp)
  {
    // doing nothing here
    return 0;
  }


void XC::ZeroLengthContact::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      { // print everything
        s << "Element: " << this->getTag()
          << " type: ZeroLengthContact  Nodes: "
          << theNodes << std::endl;
      }
    else if(flag == 1)
      {
        s << this->getTag() << "  ";
      }
  }

