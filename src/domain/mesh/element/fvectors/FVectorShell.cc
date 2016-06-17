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
//FVectorShell.cc

#include "FVectorShell.h"
#include <utility/matrix/Vector.h>

//! @brief Constructor por defecto.
XC::FVectorShell::FVectorShell(void)
  : FVectorData<24>() { zero(); }

//! @brief Constructor de copia.
XC::FVectorShell::FVectorShell(const XC::FVectorShell &otro)
  : FVectorData<24>(otro)
  {
    p[0]= otro.p[0];
    p[1]= otro.p[1];
    p[2]= otro.p[2];
    p[3]= otro.p[3];
    p[4]= otro.p[4];
    p[5]= otro.p[5];
    p[6] = otro.p[6];
    p[7] = otro.p[7];
    p[8] = otro.p[8];
    p[9] = otro.p[9];
    p[10] = otro.p[10];
    p[11] = otro.p[11];
    p[12] = otro.p[12];
    p[13] = otro.p[13];
    p[14] = otro.p[14];
    p[15] = otro.p[15];
    p[16] = otro.p[16];
    p[17] = otro.p[17];
    p[18] = otro.p[18];
    p[19] = otro.p[19];
    p[20] = otro.p[20];
    p[21] = otro.p[21];
    p[22] = otro.p[22];
    p[23] = otro.p[23];
  }

//! @brief Constructor a partir de un vector.
XC::FVectorShell::FVectorShell(const XC::Vector &v)
  {
    if(v.Size()>=24)
      {
        p[0] = v[0];
        p[1] = v[1];
        p[2] = v[2];
        p[3] = v[3];
        p[4] = v[4];
        p[5] = v[5];
        p[6] = v[6];
        p[7] = v[7];
        p[8] = v[8];
        p[9] = v[9];
        p[10] = v[10];
        p[11] = v[11];
        p[12] = v[12];
        p[13] = v[13];
        p[14] = v[14];
        p[15] = v[15];
        p[16] = v[16];
        p[17] = v[17];
        p[18] = v[18];
        p[19] = v[19];
        p[20] = v[20];
        p[21] = v[21];
        p[22] = v[22];
        p[23] = v[23];
      }
    else
      std::cerr << "FVectorShell::FVectorShell; se esperaba un vector de dimensión 6" << std::endl;
  }

//! @brief Operador asignación.
XC::FVectorShell &XC::FVectorShell::operator=(const XC::FVectorShell &otro)
  {
    p[0] = otro.p[0];
    p[1] = otro.p[1];
    p[2] = otro.p[2];
    p[3] = otro.p[3];
    p[4] = otro.p[4];
    p[5] = otro.p[5];
    p[6] = otro.p[6];
    p[7] = otro.p[7];
    p[8] = otro.p[8];
    p[9] = otro.p[9];
    p[10] = otro.p[10];
    p[11] = otro.p[11];
    p[12] = otro.p[12];
    p[13] = otro.p[13];
    p[14] = otro.p[14];
    p[15] = otro.p[15];
    p[16] = otro.p[16];
    p[17] = otro.p[17];
    p[18] = otro.p[18];
    p[19] = otro.p[19];
    p[20] = otro.p[20];
    p[21] = otro.p[21];
    p[22] = otro.p[22];
    p[23] = otro.p[23];
    return *this;
  }

void XC::FVectorShell::zero(void)
  {
    p[0]= 0.0;
    p[1]= 0.0;
    p[2]= 0.0;
    p[3]= 0.0;
    p[4]= 0.0;
    p[5]= 0.0;
    p[6]= 0.0;
    p[7]= 0.0;
    p[8]= 0.0;
    p[9]= 0.0;
    p[10]= 0.0;
    p[11]= 0.0;
    p[12]= 0.0;
    p[13]= 0.0;
    p[14]= 0.0;
    p[15]= 0.0;
    p[16]= 0.0;
    p[17]= 0.0;
    p[18]= 0.0;
    p[19]= 0.0;
    p[20]= 0.0;
    p[21]= 0.0;
    p[22]= 0.0;
    p[23]= 0.0;
  }

XC::FVectorShell &XC::FVectorShell::operator*=(const double &d)
  {
    p[0]*= d;
    p[1]*= d;
    p[2]*= d;
    p[3]*= d;
    p[4]*= d;
    p[5]*= d;
    p[6]*= d;
    p[7]*= d;
    p[8]*= d;
    p[9]*= d;
    p[10]*= d;
    p[11]*= d;
    p[12]*= d;
    p[13]*= d;
    p[14]*= d;
    p[15]*= d;
    p[16]*= d;
    p[17]*= d;
    p[18]*= d;
    p[19]*= d;
    p[20]*= d;
    p[21]*= d;
    p[22]*= d;
    p[23]*= d;
    return *this;    
  }

XC::FVectorShell &XC::FVectorShell::operator+=(const FVectorShell &otro)
  {
    p[0]+= otro.p[0];
    p[1]+= otro.p[1];
    p[2]+= otro.p[2];
    p[3]+= otro.p[3];
    p[4]+= otro.p[4];
    p[5]+= otro.p[5];
    p[6]+= otro.p[6];
    p[7]+= otro.p[7];
    p[8]+= otro.p[8];
    p[9]+= otro.p[9];
    p[10]+= otro.p[10];
    p[11]+= otro.p[11];
    p[12]+= otro.p[12];
    p[13]+= otro.p[13];
    p[14]+= otro.p[14];
    p[15]+= otro.p[15];
    p[16]+= otro.p[16];
    p[17]+= otro.p[17];
    p[18]+= otro.p[18];
    p[19]+= otro.p[19];
    p[20]+= otro.p[20];
    p[21]+= otro.p[21];
    p[22]+= otro.p[22];
    p[23]+= otro.p[23];
    return *this;    
  }

XC::FVectorShell &XC::FVectorShell::operator-=(const FVectorShell &otro)
  {
    p[0]-= otro.p[0];
    p[1]-= otro.p[1];
    p[2]-= otro.p[2];
    p[3]-= otro.p[3];
    p[4]-= otro.p[4];
    p[5]-= otro.p[5];
    p[6]-= otro.p[6];
    p[7]-= otro.p[7];
    p[8]-= otro.p[8];
    p[9]-= otro.p[9];
    p[10]-= otro.p[10];
    p[11]-= otro.p[11];
    p[12]-= otro.p[12];
    p[13]-= otro.p[13];
    p[14]-= otro.p[14];
    p[15]-= otro.p[15];
    p[16]-= otro.p[16];
    p[17]-= otro.p[17];
    p[18]-= otro.p[18];
    p[19]-= otro.p[19];
    p[20]-= otro.p[20];
    p[21]-= otro.p[21];
    p[22]-= otro.p[22];
    p[23]-= otro.p[23];
    return *this;    
  }

//! @brief Agrega la fuerza being passed as parameter.
void XC::FVectorShell::addForce(const size_t &inod,const double &P1,const double &P2,const double &P3)
  {
    if(inod>3)
      std::cerr << "FVectorShell::addForce; nodo: "
                << inod << "fuera de rango (0..3)." << std::endl;
    const size_t i= inod*6;
    p[i]-= P1;  //Nodo 1: i=0. Nodo 2: i=6. Nodo 3: i= 12. Nodo 4: i= 18
    p[i+1]-= P2;
    p[i+2]-= P3;
  }

//! @brief Agrega el momento being passed as parameter.
void XC::FVectorShell::addMoment(const size_t &inod,const double &M1,const double &M2,const double &M3)
  {
    if(inod>3)
      std::cerr << "FVectorShell::addMoment; nodo: "
                << inod << "fuera de rango (0..3)." << std::endl;
    const size_t i= inod*6+3;
    p[i]-= M1;  //Nodo 1: i=3. Nodo 2: i=9. Nodo 3: i= 15. Nodo 4: i= 21
    p[i+1]-= M2;
    p[i+2]-= M3;
  }

void XC::FVectorShell::Print(std::ostream &os) const
  {
    os << '[' << p[0] << ',' << p[1] << ',' << p[2] << ',' << p[3] << ',' << p[4] << ','
              << p[5] << ',' << p[6] << ',' << p[7] << ',' << p[8] << ',' << p[9] << ','
              << p[10] << ',' << p[11] << ',' << p[12] << ',' << p[13] << ',' << p[14] << ','
              << p[15] << ',' << p[16] << ',' << p[17] << ',' << p[18] << ',' << p[19] << ','
              << p[20] << ',' << p[21] << ',' << p[22] << ',' << p[23] << ']';
  }

