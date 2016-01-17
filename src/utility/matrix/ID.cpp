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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include <boost/python/extract.hpp>

int XC::ID::ID_NOT_VALID_ENTRY = 0;

void XC::ID::libera(void)
  {
    if(data && (fromFree==0))
      {
        delete [] data;
        data= nullptr;
      }
  }

void XC::ID::check_sizes(void)
  {
#ifdef _G3DEBUG
    if(sz<0)
      {
        std::cerr << "alloc - size " << size << " specified < 0\n";
        sz = 0;
      }
    if(arraySize <= 0)
      {
        std::cerr << "alloc - arraySize " << arraySize << " specified < 0\n";
        if(sz!=0) 
          arraySize= sz;
        else
          arraySize= 1;
      }
    if(arraySize<sz)
      {
        std::cerr << "alloc - arraySize " << arraySize << " specified < " << size << std::endl;
        arraySize = sz;
      }
#endif
  }

void XC::ID::alloc(const size_t &arrSize)
  {
    libera();

    if(arrSize>0)
      {
        // create the space
        data=new int[arrSize];
        fromFree= 0;
        if(!data)
          std::cerr << "alloc: ran out of memory with arraySize: " << arraySize << std::endl;
      }
  }

// ID():
//	Standard constructor, sets size = 0;

XC::ID::ID(void)
  :sz(0), data(nullptr), arraySize(0), fromFree(0) {}


// ID(int size):
//	Constructor used to allocate a ID of size size.

XC::ID::ID(int size)
  :sz(size), data(nullptr), arraySize(size), fromFree(0)
  {
    check_sizes();
    alloc(arraySize);
    // zero the data
    for(int i=0; i<size; i++)
      data[i] = 0;
  }


// ID(int size):
//	Constructor used to allocate a XC::ID of size size.

XC::ID::ID(int size, int arraySz)
  :sz(size), data(nullptr), arraySize(arraySz), fromFree(0)
  {
    check_sizes();
    alloc(arraySize);
    // zero the data
    for(int i=0; i<arraySize; i++)
      data[i] = 0;
  }

XC::ID::ID(int *d, int size, bool cleanIt)
  :sz(size), data(d), arraySize(size), fromFree(1)
  {
    if(!d)
      {
        std::cerr << "ID; se pasó un puntero nulo." << std::endl;
        sz = 0;
        data = 0;
        arraySize = size;
        fromFree = 0;

        // create the space
        if(arraySize!=0)
          {
	    std::cerr << "IDWrapper; llama a malloc" << std::endl;
            data =  (int *)malloc(arraySize*sizeof(int));
            if(!data)
              {
	        std::cerr << "ID(int*,int): ran out of memory with arraySize "
                          << arraySize << std::endl;
	        exit(-1);
              }
          }
        // zero the data
        for(int i=0; i<arraySize; i++)
          data[i] = 0;
       }
    if(cleanIt==true)
      fromFree=0;
  }

XC::ID::ID(const int *d, int size)
  :sz(size), data(nullptr), arraySize(size), fromFree(0)
  {
    if(d)
      {
        check_sizes();
        alloc(arraySize);
        if(arraySize!=0)
          {
            for(int i=0; i<arraySize; i++)
              data[i]= d[i];
          }
      }
    else
      {
        sz= 0;
        data= nullptr;
        arraySize= 0;
        fromFree= 0;
      }
  }

//! @brief Constructor.
XC::ID::ID(const std::vector<int> &v)
  :sz(v.size()), data(nullptr), arraySize(v.size()), fromFree(0)
  {
    check_sizes();
    alloc(arraySize);
    // copy the data
    for(int i=0; i<sz; i++)
      data[i]= v[i];
  }

//! @brief Constructor (interfaz Python).
XC::ID::ID(const boost::python::list &l)
  :sz(len(l)), data(nullptr), arraySize(len(l)), fromFree(0)
  {
    check_sizes();
    alloc(arraySize);
    // copy the data
    for(int i=0; i<sz; i++)
      data[i]= boost::python::extract<int>(l[i]);
  }

XC::ID::ID(const std::set<int> &setInt)
  :sz(setInt.size()), data(nullptr), arraySize(setInt.size()), fromFree(0)
  {
    check_sizes();
    alloc(arraySize);
    // copy the data
    int count= 0;
    for(std::set<int>::const_iterator i= setInt.begin();i!=setInt.end();i++,count++)
      data[count]= *i;
  }


//! @brief Constructor de copia.
XC::ID::ID(const ID &other)
  :sz(other.sz), data(nullptr), arraySize(other.arraySize), fromFree(0)
  {
    alloc(arraySize);
    // copy the data 
    for(int i=0; i<sz; i++)
      data[i] = other.data[i];
  }	

// ~ID():
// 	destructor, deletes the [] data

XC::ID::~ID(void)
 { libera(); }

int XC::ID::setData(int *newData, int size, bool cleanIt)
  {
    libera();
    sz = size;
    data = newData;
  
    if(cleanIt == false)
      fromFree= 1;
    else
      fromFree= 0;

    if(sz <= 0)
      {
        std::cerr << "XC::ID::ID(int *, size) - size " << size << " specified <= 0\n";
        sz= 0;
      }
    return 0;
  }


void XC::ID::Zero(void)
  {
    for(int i=0; i<sz; i++)
      data[i]=0;
  }

int XC::ID::getLocation(int value) const
  {
    // search through ID for the value
    for(int i=0; i<sz; i++)
      if(data[i] == value)
        return i;
    // if we get here the value is not in the array
    return -1;
  }


int XC::ID::getLocationOrdered(int value) const
  {
    int middle = 0;
    int left = 0;
    int right = sz-1;
    if(sz!=0)
      {
        while(left <= right)
          {
            middle = (left + right)/2;
            double dataMiddle = data[middle];
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


int XC::ID::removeValue(int value)
  {
    int place = -1;
    for(int i=0; i<sz; i++)
    if(data[i] == value)
      {
        place = i;
        // copy the rest of the components down one in XC::ID
        for(int j=i; j<sz-1; j++)
	  data[j] = data[j+1];		
        sz--;
      }
    return place;
  }    


int &XC::ID::operator[](int x) 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(x < 0)
      {
        std::cerr << "XC::ID::[] - location " << x << " < 0\n";
        return ID_NOT_VALID_ENTRY;
      }
#endif

    // see if XC::quick return
    if(x<sz)
      return data[x];

    //otherwise we have to enlarge the order of the XC::ID
    
    // see if we can just enlarge the array
    // without having to go get more space

    if(x<arraySize)
      {
        for(int i=sz; i<x; i++)
          data[i] = 0;
        sz= x+1;
        return data[x];
      }

    // otherwise we go get more space
    if(x>=arraySize)
      {
	std::cerr << "ID::operator[] aumenta el tamaño del vector." << std::endl;
        int newArraySize= arraySize * 2;
        if(newArraySize < x) 
          newArraySize= x;
        resize(newArraySize);
        return data[x];
      }
    else
      {
        // we could not allocate more mem .. leave the current size
        std::cerr << "ID::[]): ran out of memory with arraySize " << arraySize << std::endl;
        return ID_NOT_VALID_ENTRY;
      }
    // we should never get here, but some compilers need this line
    return ID_NOT_VALID_ENTRY;	
  }
    

int XC::ID::resize(int newSize)
  {
    int retval= 0;
    if(newSize<0) // first check that newSize is valid
      {
        std::cerr << "ID::resize() - size specified " << newSize << " < 0\n";
        retval= -1;
      }
    else if(newSize==0)
      {
        libera();
        sz= 0;
        arraySize= newSize;
      }
    else
      { 
        if(sz > newSize)
          {
            // is size smaller than current, simply reset sz
            sz = newSize;
          }
        else if(newSize <= arraySize) //Corregido LCPT.
          {
            // see if we can just enlarge the array
            // without having to go get more space
            for(int i=sz; i<newSize; i++)
              data[i] = 0;
            sz = newSize;
          }
        else if(newSize > arraySize)
          {
           // otherwise we go get more space
    
           int *newData= new int[newSize];
           if(newData)
             {
               // copy the old
               for(int i=0; i<sz; i++)
	         newData[i] = data[i];
               // zero the new
               for(int j=sz; j<newSize; j++)
 	         newData[j]= 0;
      
               libera();
               sz= newSize;
               arraySize = newSize;
               data = newData;
             }
           else
             {
               std::cerr << "ID::resize() - out of memory creating ID of size " << newSize << "\n";
               retval= -1;      
             }
          }
      }
    return retval;
  }

//! @brief Devuelve el máximo de las componentes del vector.
const int &XC::ID::max(void) const
  { return *std::max_element(data,data+Size()); }

//! @brief Devuelve el mínimo de las componentes del vector.
const int &XC::ID::min(void) const
  { return *std::min_element(data,data+Size()); }

// ID &operator=(const ID  &V):
//	the assignment operator, This is assigned to be a copy of V. if sizes
//	are not compatable this.data [] is deleted. The data pointers will not
//	point to the same area in mem after the assignment.
//

XC::ID &XC::ID::operator=(const ID &V) 
  {
    // first check we are not trying v = v
    if(this != &V)
      {
	// check size compatability, if different delete
	// old and make room for new.
	if(sz != V.sz)
          {
	    if(arraySize < V.sz)
              {
		arraySize= V.sz;
                check_sizes();
                alloc(arraySize);
              }
	    sz = V.sz;
	  }
	// copy the data
	for(int i=0; i<sz; i++)
	    data[i] = V(i);
      }
    return *this;
  }





//! @brief A function is defined to allow user to print the IDs using streams.
std::ostream &XC::operator<<(std::ostream &s, const XC::ID &V)
  {
    for(int i=0; i<V.Size();i++) 
      s << V(i) << " ";
    return s;
  }

// friend istream &operator>>(istream &s, ID &V)
//	A function is defined to allow user to input the data into a XC::ID which has already
//	been constructed with data, i.e. ID(int) or XC::ID(const XC::ID &) constructors.

/*
istream &operator>>(istream &s, ID &V)
{
    for(int i=0; i<V.Size(); i++) 
	s >> V(i);

    return s;
}
*/


//! @brief Convierte el vector en un std::vector<double>.
std::vector<int> XC::id_to_std_vector(const ID &v)
  {
    const size_t sz= v.Size();
    std::vector<int> retval(sz,0);
    for(register size_t i=0;i<sz;i++)
      retval[i]= v(i);
    return retval;
  }

