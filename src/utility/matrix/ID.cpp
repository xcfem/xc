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
                                                                        
// $Revision: 1.10 $
// $Date: 2005/11/23 22:37:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/matrix/ID.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class implementation for XC::ID.
//
// What: "@(#) ID.C, revA"

#include "ID.h"
#include <cstdlib>


#include <boost/python/extract.hpp>

int XC::ID::ID_NOT_VALID_ENTRY= 0;

//! @brief Standard constructor, sets size = 0;
XC::ID::ID(void)
  :EntCmd(), std::vector<int>(0) {}


//! @brief Constructor used to allocate a ID of size size.
XC::ID::ID(const int &size)
  :EntCmd(), std::vector<int>(size,0) {}


//! @brief Constructor.
XC::ID::ID(const std::vector<int> &v)
  : EntCmd(), std::vector<int>(v)
  {}

//! @brief Constructor (interfaz Python).
XC::ID::ID(const boost::python::list &l)
  : EntCmd(), std::vector<int>(len(l))
  {
    // copy the data
    const size_t sz= size();
    for(size_t i=0; i<sz; i++)
      (*this)[i]= boost::python::extract<int>(l[i]);
  }

XC::ID::ID(const std::set<int> &setInt)
  : EntCmd(), std::vector<int>(setInt.size())
  {
    // copy the data
    int count= 0;
    for(std::set<int>::const_iterator i= setInt.begin();i!=setInt.end();i++,count++)
      (*this)[count]= *i;
  }


void XC::ID::Zero(void)
  { std::fill(begin(),end(),0); }

//! @brief Returns the position of 'value' in the vector.
int XC::ID::getLocation(const int &value) const
  {
    const size_t sz= size();
    // search through ID for the value
    for(size_t i=0; i<sz; i++)
      if((*this)[i] == value)
        return i;
    // if we get here the value is not in the array
    return -1;
  }


int XC::ID::getLocationOrdered(const int &value) const
  {
    const size_t sz= size();    
    int middle = 0;
    int left = 0;
    int right = sz-1;
    if(sz!=0)
      {
        while(left <= right)
          {
            middle = (left + right)/2;
            double dataMiddle = (*this)[middle];
            if(value == dataMiddle)
              return middle;   // already there
            else if(value>dataMiddle)
              left = middle + 1;
            else 
              right = middle-1;
          }
      }
    // if we get here the value is not in the array
    return -1;
  }


int XC::ID::removeValue(const int &value)
  {
    int place = -1;
    auto it = std::find(begin(), end(), value);
    if(it != end())
      place= std::distance(begin(), it);
      erase(it);
    return place;
  }    


int XC::ID::resize(const int &newSize)
  {
    int retval= 0;
    if(newSize<0) // first check that newSize is valid
      {
        std::cerr << "ID::resize() - size specified " << newSize << " < 0\n";
        retval= -1;
      }
    else if(newSize==0)
      clear();
    else
      std::vector<int>::resize(newSize,0);
    return retval;
  }

//! @brief Devuelve el máximo de las componentes del vector.
const int &XC::ID::max(void) const
  { return *std::max_element(begin(),end()); }

//! @brief Devuelve el mínimo de las componentes del vector.
const int &XC::ID::min(void) const
  { return *std::min_element(begin(),end()); }

//! @brief A function is defined to allow user to print the IDs using streams.
std::ostream &XC::operator<<(std::ostream &s, const XC::ID &V)
  {
    const size_t sz= V.size();
    for(size_t i=0; i<sz;i++) 
      s << V(i) << " ";
    return s;
  }

XC::ID XC::getIDFromIntPtr(const int *d,const int &size)
  {
    ID retval(std::vector<int>(d,d+size));
    if(!d)
      {
	std::cerr << "getIDFromIntPtr constructor can't copy from null pointer."
	          << std::endl;
      }
    return retval;
  }

// friend istream &operator>>(istream &s, ID &V)
//        A function is defined to allow user to input the data into a XC::ID which has already
//        been constructed with data, i.e. ID(int) or XC::ID(const XC::ID &) constructors.

/*
istream &operator>>(istream &s, ID &V)
{
    for(size_t i=0; i<V.Size(); i++) 
        s >> V(i);

    return s;
}
*/



