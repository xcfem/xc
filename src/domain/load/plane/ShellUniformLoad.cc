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
//ShellUniformLoad.cpp

#include "ShellUniformLoad.h"
#include "domain/mesh/element/fvectors/FVectorShell.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"


XC::ShellUniformLoad::ShellUniformLoad(int tag,const double &wt,const double &wa1,const double &wa2,const ID &theElementTags)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad,wt,wa1,wa2,theElementTags) {}
XC::ShellUniformLoad::ShellUniformLoad(int tag, const Vector &Fxyz, const ID &theElementTags)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad,Fxyz[2],Fxyz[0],Fxyz[1],theElementTags) {}

XC::ShellUniformLoad::ShellUniformLoad(int tag)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad) {}

std::string XC::ShellUniformLoad::Categoria(void) const
  { return "uniforme"; }


int XC::ShellUniformLoad::getType(void)
  { return LOAD_TAG_ShellUniformLoad; }

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::ShellUniformLoad::getDimVectorFuerza(void) const
  { return 3; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::ShellUniformLoad::getDimVectorMomento(void) const
  { return 3; }

//! @brief Devuelve las componentes de los vectores fuerza.
const XC::Matrix &XC::ShellUniformLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Wx();
        retval(i,1)= Wy();
        retval(i,2)= Wz();
      }
    return retval;
  }

//! @brief Devuelve las componentes de los vectores momento.
const XC::Matrix &XC::ShellUniformLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= 0.0;
        retval(i,1)= 0.0;
        retval(i,2)= 0.0;
      }
    return retval;
  }

//! @brief Añade la carga al vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param area Area del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector de cargas del elemento.
void XC::ShellUniformLoad::addReactionsInBasicSystem(const double &area,const double &loadFactor,FVectorShell &p0)
  {
    //Cargas sobre nodos.
    const double Px= Wx()*loadFactor*area/4;
    const double Py= Wy()*loadFactor*area/4;
    const double Pz= Wz()*loadFactor*area/4; //Axil.

    // Reactions in basic system
    p0.addForce(0,Px,Py,Pz);

    //p0[3]-= 0.0;
    //p0[4]-= 0.0;
    //p0[5]-= 0.0;

    p0.addForce(1,Px,Py,Pz);

    //p0[9]-= 0.0;
    //p0[10]-= 0.0;
    //p0[11]-= 0.0;

    p0.addForce(2,Px,Py,Pz);

    //p0[15]-= 0.0;
    //p0[16]-= 0.0;
    //p0[17]-= 0.0;

    p0.addForce(3,Px,Py,Pz);

    //p0[21]-= 0.0;
    //p0[22]-= 0.0;
    //p0[23]-= 0.0;
  }

//! @brief ??
//! @param area Area del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 
void XC::ShellUniformLoad::addFixedEndForcesInBasicSystem(const double &area,const double &loadFactor,FVectorShell &q0)
  {
    std::cerr << "ShellUniformLoad::addFixedEndForcesInBasicSystem no implementada." << std::endl;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::ShellUniformLoad::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

int XC::ShellUniformLoad::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(6);
    int result= sendData(cp);

    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "ShellUniformLoad::sendSelf() - failed to send extra data\n";
    return result;
  }

int XC::ShellUniformLoad::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ShellUniformLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

void  XC::ShellUniformLoad::Print(std::ostream &s, int flag)
  {
    s << "ShellUniformLoad - Reference load" << std::endl;
    s << "  Transverse:  " << Trans << std::endl;
    s << "  Axial1:      " << Axial1 << std::endl;
    s << "  Axial2:      " << Axial2 << std::endl;
    s << "  Elements acted on: " << getElementTags();
  }
