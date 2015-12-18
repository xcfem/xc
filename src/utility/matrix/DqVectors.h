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
//DqVectors.h

#ifndef DqVectors_h
#define DqVectors_h

#include <deque>
#include <iostream>

namespace XC {

class Vector;

class DqVectors: public std::deque<Vector *>
  {
  private:
    DqVectors(const DqVectors &otro);
    DqVectors &operator=(const DqVectors &otro);
  protected:
    typedef std::deque<Vector *> dq_ptr_Vectors;
    typedef dq_ptr_Vectors::const_reference const_reference;
    typedef dq_ptr_Vectors::reference reference;
    typedef dq_ptr_Vectors::iterator iterator;
    typedef dq_ptr_Vectors::const_iterator const_iterator;


  public:
    DqVectors(const size_t &num= 0);
    ~DqVectors(void);

    void clear(void);

    void Print(std::ostream &s,const int &flag);
  };
} // end of XC namespace

#endif
