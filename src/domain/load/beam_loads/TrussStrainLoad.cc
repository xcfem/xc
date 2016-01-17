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


#include "TrussStrainLoad.h"
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableVector.h"

XC::TrussStrainLoad::TrussStrainLoad(int tag,const double &t1, const double &t2,const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussStrainLoad, theElementTags), e1(t1),  e2(t2) {}
XC::TrussStrainLoad::TrussStrainLoad(int tag,const double &temp,const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussStrainLoad, theElementTags), e1(temp),  e2(temp) {}

XC::TrussStrainLoad::TrussStrainLoad(int tag, const ID &theElementTags)
  :ElementBodyLoad(tag, LOAD_TAG_TrussStrainLoad, theElementTags), e1(0.0), e2(0.0) {}

XC::TrussStrainLoad::TrussStrainLoad(int tag)
  :ElementBodyLoad(tag, LOAD_TAG_TrussStrainLoad), e1(0.0), e2(0.0) {}

XC::TrussStrainLoad::TrussStrainLoad(void)
  :ElementBodyLoad(LOAD_TAG_TrussStrainLoad), e1(0.0), e2(0.0) {}

int XC::TrussStrainLoad::sendSelf(CommParameters &cp)
  {
    static ID data(2);
    int res= sendData(cp);
    res+= cp.sendDoubles(e1,e2,getDbTagData(),CommMetaData(1));
    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "TrussStrainLoad::sendSelf() - failed to send extra data\n";    
    return res;
  }

int XC::TrussStrainLoad::recvSelf(const CommParameters &cp)
  {
    static ID data(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "TrussStrainLoad::recvSelf() - data could not be received\n" ;
    else
      {
        res+= recvData(cp);
        res+= cp.receiveDoubles(e1,e2,getDbTagData(),CommMetaData(1));
      }
    return res;
  }

void XC::TrussStrainLoad::Print(std::ostream &s, int flag) const
  {
    s << "TrussStrainLoad - reference load : " << e1 << " strain at node 1 : " << e1 << " strain at node 1\n";
    s <<  e2 << " strain at node 2\n";
    ElementBodyLoad::Print(s,flag);
  }
