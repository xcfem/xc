//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
// $Date: 2005/12/14 23:49:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/matrix/Vector.cpp,v $
                                                                        
                                                                        
// File: ~/matrix/Vector.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class implementation for Vector.
//
// What: "@(#) Vector.C, revA"

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"
#include <utility/matrix/nDarray/Tensor.h>

#include <cstdlib>
#include <cmath>


#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"

double XC::Vector::VECTOR_NOT_VALID_ENTRY =0.0;

void XC::Vector::free_mem(void)
  {
    if(fromFree == 0)
      if(theData)
	delete [] theData;
    theData= nullptr;
    sz= 0;
    fromFree= 0;
  }

void XC::Vector::alloc(const size_t &size)
  {
    free_mem();
    if(size >=0)
      {
        sz= size;
        if(size>0)
          {
            theData= new double[sz];
            fromFree= 0;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; size specified: " << size << " <0\n";

    if(!theData && (size>0))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; out of memory creating vector of size "
		  << size << std::endl;
        sz= 0; // set this should fatal error handler not kill process!!
      }
  }

// Vector():
//! @brief Standard constructor, sets size= 0;
XC::Vector::Vector(void)
  : sz(0), theData(nullptr), fromFree(0) {}

// Vector(int size):
//! @brief Constructor used to allocate a vector of size size.
XC::Vector::Vector(const int &szt, const double &valor)
  : sz(0), theData(nullptr), fromFree(0)
  {
    alloc(szt);
    for(register int i=0; i<sz; i++)
      theData[i]= valor;
  }

XC::Vector::Vector(const std::vector<double> &v)
  : sz(0), theData(nullptr), fromFree(0)
  {
    alloc(v.size());
    // copy the components
    for(register int i=0; i<sz; i++)
      theData[i]= v[i];
  }

//! @brief Constructor (interfaz Python).
XC::Vector::Vector(const boost::python::list &l)
  :sz(0), theData(nullptr), fromFree(0)
  {
    alloc(len(l));
    // copy the components
    for(int i=0; i<sz; i++)
      theData[i]= boost::python::extract<double>(l[i]);
  }

XC::Vector::Vector(const Vector2d &v)
  : sz(0), theData(nullptr), fromFree(0)
  {
    alloc(2);
    // copy the components
    theData[0]= v.x();
    theData[1]= v.y();
  }

XC::Vector::Vector(const Vector3d &v)
  : sz(0), theData(nullptr), fromFree(0)
  {
    alloc(3);
    // copy the components
    theData[0]= v.x();
    theData[1]= v.y();
    theData[2]= v.z();
  }  

XC::Vector::Vector(const double &x,const double &y,const double &z)
  : sz(0), theData(nullptr), fromFree(0)
  {
    alloc(3);
    // copy the components
    theData[0]= x;
    theData[1]= y;
    theData[2]= z;
  }

//! @brief Constructor.
XC::Vector::Vector(double *data, int size)
  : sz(0),theData(nullptr),fromFree(0)
  {
    setData(data,size);
  }
 


// Vector(const XC::Vector&):
//! @brief Constructor to init a vector from another.
XC::Vector::Vector(const Vector &other)
  : sz(0),theData(nullptr),fromFree(0)
  {
    alloc(other.sz);
    // copy the component data
    for(register int i=0; i<sz; i++)
      theData[i]= other.theData[i];
  }

//! @brief destructor, deletes the [] data
XC::Vector::~Vector(void)
  { free_mem(); }

int XC::Vector::setData(double *newData, int size)
  {
    free_mem();
    sz= size;
    theData= newData;
    fromFree= 1;

    if(sz <= 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; size specified: " << size << " <= 0\n";
        sz= 0;
      }
    return 0;
  }


//! @brief Changes vector size.
int XC::Vector::resize(int newSize)
  {
    if(fromFree!=0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; with fromFree= " << fromFree << std::endl;

    // first check that newSize is valid
    if(newSize<0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; size specified " << newSize << " <= 0\n";
        return -1;
      } 
    else if(newSize == 0)
      { free_mem(); } 
    else // otherwise if newSize is greater than oldSize free old space and get new_ space
      if(newSize > sz)
        {
          alloc(newSize);
          if(!theData)
            return -2;
          sz= newSize;
        }  
      // just set the size to be newSize .. penalty of holding onto additional
      // memory .. but then save the free() and malloc() calls
      else 
        sz= newSize;
    return 0;
  }


// Assemble(Vector &x, ID &y, double fact ):
//! @brief Method to assemble into object the XC::Vector V using the XC::ID l.
//! If XC::ID(x) does not exist program writes error message if
//! VECTOR_CHECK defined, otherwise ignores it and goes on.
int XC::Vector::Assemble(const XC::Vector &V, const XC::ID &l, double fact )
  {
    int result= 0;
    int pos;
    for(register int i=0; i<l.Size(); i++)
      {
        pos= l(i);
        if(pos<0)
          ;
        else if((pos < sz) && (i < V.Size()))
          // assemble into vector
          theData[pos] += V.theData[i] *fact;
        else
          {
            result= -1;
            if(pos < sz)
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; " << pos << " out of range [1, "
			<< sz-1 << "]\n";
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; " << pos
			<< " out of range [1, "<< V.Size()-1 << "]\n";
          }
      }
    return result;
  }
    
//! @brief Normalizes the vector with the euclidean norm.
int XC::Vector::Normalize(void)
  {
    double length= 0.0;
    for(register int i=0; i<sz; i++)
      length += theData[i] * theData[i];
    length= sqrt(length);
  
    if(length == 0.0) 
      return -1;
    length= 1.0/length;
    for(register int j=0; j<sz; j++)
      theData[j]*= length;
    return 0;
  }

//! @brief Normaliza el vector con la norma_infinito.
int XC::Vector::NormalizeInf(void)
  {
    int retval= 0;
    const double norma= NormInf();
  
    if(norma == 0.0)
      retval= -1;
    else
      {
        for(register int j=0; j<sz; j++)
          theData[j]/= norma;
      }
    return 0;
  }

//! @brief Returns the normalized vector con la norma euclidea.
XC::Vector XC::Vector::Normalized(void) const
  {
    Vector retval(*this);
    retval.Normalize();
    return retval;
  }

//! @brief Returns the normalized vector con la norma_infinito.
XC::Vector XC::Vector::NormalizedInf(void) const
  {
    Vector retval(*this);
    retval.NormalizeInf();
    return retval;
  }

//! @brief Returns the normalized vector (euclidean norm).
XC::Vector XC::normalize(const Vector &v)
  { return v.Normalized(); }

//! @brief Returns the normalized vector (infinite norm).
XC::Vector XC::normalize_inf(const Vector &v)
  { return v.NormalizedInf(); }

int XC::Vector::addVector(double thisFact, const Vector &other, double otherFact )
  {
    // check if quick return
    if(otherFact == 0.0 && thisFact == 1.0)
      return 0; 

    // if sizes are compatable add
#ifdef _G3DEBUG
    if(sz != other.sz)
      {
        // else sizes are incompatable, do nothing but warning
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; incompatable Vector sizes\n";
        return -1;
      }
#endif

    if(thisFact == 1.0)
      {
        // want: this += other * otherFact
        double *dataPtr= theData;
        double *otherDataPtr= other.theData;
        if(otherFact == 1.0)
          { // no point doing a multiplication if otherFact == 1.0
            for(register int i=0; i<sz; i++) 
	      *dataPtr++ += *otherDataPtr++;
          }
        else if(otherFact == -1.0)
          { // no point doing a multiplication if otherFact == 1.0
            for(register int i=0; i<sz; i++) 
  	    *dataPtr++ -= *otherDataPtr++;
          }
        else 
          for(register int i=0; i<sz; i++) 
  	    *dataPtr++ += *otherDataPtr++ * otherFact;
      } 
    else if(thisFact == 0.0)
      {
        // want: this= other * otherFact
        double *dataPtr= theData;
        double *otherDataPtr= other.theData;
        if(otherFact == 1.0)
          { // no point doing a multiplication if otherFact == 1.0
            for(register int i=0; i<sz; i++) 
  	    *dataPtr++= *otherDataPtr++;
          }
        else if(otherFact == -1.0)
          { // no point doing a multiplication if otherFact == 1.0
            for(register int i=0; i<sz; i++) 
  	    *dataPtr++= *otherDataPtr++;
          }
        else 
          for(register int i=0; i<sz; i++) 
  	    *dataPtr++= *otherDataPtr++ * otherFact;
      }
    else
      {
        // want: this= this * thisFact + other * otherFact
        double *dataPtr= theData;
        double *otherDataPtr= other.theData;
        if(otherFact == 1.0)
          { // no point doing a multiplication if otherFact == 1.0
            for(register int i=0; i<sz; i++)
              {
  	        double value= *dataPtr * thisFact + *otherDataPtr++;
  	        *dataPtr++= value;
              }
          }
        else if(otherFact == -1.0)
          { // no point doing a multiplication if otherFact == 1.0
            for(register int i=0; i<sz; i++)
              {
  	        double value= *dataPtr * thisFact - *otherDataPtr++;
  	        *dataPtr++= value;
              }
          }
        else 
          for(register int i=0; i<sz; i++)
            {
              double value= *dataPtr * thisFact + *otherDataPtr++ * otherFact;
  	      *dataPtr++= value;
            }
      } 
    // successfull
    return 0;
  }
	    
	
int XC::Vector::addMatrixVector(double thisFact, const Matrix &m, const XC::Vector &v, double otherFact )
  {
    // see if quick return
    if(thisFact == 1.0 && otherFact == 0.0)
      return 0;

  // check the sizes are compatable
#ifdef _G3DEBUG
  // check the sizes are compatable
    if((sz != m.noRows()) && (m.noCols() != v.sz))
      {
        // otherwise incompatable sizes
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; incompatable sizes\n";
        return -1;    
      }
#endif

    if(thisFact == 1.0)
      {
        // want: this += m * v * otherFact
        if(otherFact == 1.0)
          { // no point doing multiplication if otherFact= 1.0
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++;
    	for(int j=0; j<sz; j++)
    	  theData[j] += *matrixDataPtr++ * otherData;
          }
        } 
        else if(otherFact == -1.0) { // no point doing multiplication if otherFact= -1.0
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++;
    	for(int j=0; j<sz; j++)
    	  theData[j] -= *matrixDataPtr++ * otherData;
          }
        } 
        else { // have to do the multiplication
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++ * otherFact;
    	for(int j=0; j<sz; j++)
    	  theData[j] += *matrixDataPtr++ * otherData;
          }
        }
      }
    
      else if(thisFact == 0.0) {
        
        // want: this= m * v * otherFact
        for(int i=0; i<sz; i++)
          theData[i]= 0.0;
    
        if(otherFact == 1.0) { // no point doing multiplication if otherFact= 1.0
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++;
    	for(int j=0; j<sz; j++)
    	  theData[j] += *matrixDataPtr++ * otherData;
          }
        } 
        else if(otherFact == -1.0) { // no point doing multiplication if otherFact= -1.0
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++;
    	for(int j=0; j<sz; j++)
    	  theData[j] -= *matrixDataPtr++ * otherData;
          }
        } else {
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++ * otherFact;
    	for(int j=0; j<sz; j++)
    	  theData[j] += *matrixDataPtr++ * otherData;
          }
        }
      }
    
      else {
    
        // want: this= this * thisFact + m * v * otherFact
        for(int i=0; i<sz; i++)
          theData[i] *= thisFact;
    
        if(otherFact == 1.0) { // no point doing multiplication if otherFact= 1.0
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++;
    	for(int j=0; j<sz; j++)
    	  theData[j] += *matrixDataPtr++ * otherData;
          }
        } else if(otherFact == -1.0) { // no point doing multiplication if otherFact= 1.0
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++;
    	for(int j=0; j<sz; j++)
    	  theData[j] -= *matrixDataPtr++ * otherData;
          }
        } else {
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtr= v.theData;
          for(int i=0; i<otherSize; i++) {
    	double otherData= *otherDataPtr++ * otherFact;
    	for(int j=0; j<sz; j++)
    	  theData[j] += *matrixDataPtr++ * otherData;
          }
        }
      }
      
    // successfull
    return 0;
  }



int XC::Vector::addMatrixTransposeVector(double thisFact, const XC::Matrix &m, const XC::Vector &v, double otherFact)
  {
  // see if quick return
    if(otherFact == 0.0 && thisFact == 1.0)
      return 0;

#ifdef _G3DEBUG
  // check the sizes are compatable
    if((sz != m.noRows()) && (m.noRows() != v.sz))
      {
        // otherwise incompatable sizes
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; incompatable sizes\n";
        return -1;    
      }
#endif

  if(thisFact == 1.0)
    {
      // want: this += m^t * v * otherFact
      if(otherFact == 1.0) // no point doing multiplication if otherFact= 1.0
        {
          const int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtrA= v.theData;
          for(int i=0; i<sz; i++)
            {
              double *otherDataPtr= otherDataPtrA;
	      double sum= 0.0;
	      for(int j=0; j<otherSize; j++)
	        sum += *matrixDataPtr++ * *otherDataPtr++;
	      theData[i] += sum;
            }
        }
      else if(otherFact == -1.0) // no point doing multiplication if otherFact= 1.0
        {
          int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtrA= v.theData;
          for(int i=0; i<sz; i++)
            {
              double *otherDataPtr= otherDataPtrA;
	      double sum= 0.0;
	      for(int j=0; j<otherSize; j++)
	        sum += *matrixDataPtr++ * *otherDataPtr++;
	      theData[i] -= sum;
            }
        }
      else
        {
          const int otherSize= v.sz;
          const double *matrixDataPtr= m.getDataPtr();
          double *otherDataPtrA= v.theData;
          for(int i=0; i<sz; i++)
            {
	      double *otherDataPtr= otherDataPtrA;
	      double sum= 0.0;
              for(int j=0; j<otherSize; j++)
	        sum += *matrixDataPtr++ * *otherDataPtr++;
	      theData[i] += sum * otherFact;
            }
        }
      }
    else if(thisFact == 0.0)
       {
        // want: this= m^t * v * otherFact
        if(otherFact == 1.0) // no point doing multiplication if otherFact= 1.0
          {
            int otherSize= v.sz;
            const double *matrixDataPtr= m.getDataPtr();
            double *otherDataPtrA= v.theData;
            for(int i=0; i<sz; i++)
              {
	        double *otherDataPtr= otherDataPtrA;
	        double sum= 0.0;
	        for(int j=0; j<otherSize; j++)
	          sum += *matrixDataPtr++ * *otherDataPtr++;
	        theData[i]= sum;
              }
          }
        else if(otherFact == -1.0) // no point doing multiplication if otherFact= -1.0
          {
            const int otherSize= v.sz;
            const double *matrixDataPtr= m.getDataPtr();
            double *otherDataPtrA= v.theData;
            for(int i=0; i<sz; i++)
              {
	        double *otherDataPtr= otherDataPtrA;
	        double sum= 0.0;
	        for(int j=0; j<otherSize; j++)
	          sum += *matrixDataPtr++ * *otherDataPtr++;
	        theData[i]= -sum;
              }
          }
        else
          {
            const int otherSize= v.sz;
            const double *matrixDataPtr= m.getDataPtr();
            double *otherDataPtrA= v.theData;
            for(int i=0; i<sz; i++)
              {
	        double *otherDataPtr= otherDataPtrA;
	        double sum= 0.0;
	        for(int j=0; j<otherSize; j++)
	          sum += *matrixDataPtr++ * *otherDataPtr++;
	        theData[i]= sum * otherFact;
              }
	  }
       }
     else
       {
        // want: this= this * thisFact + m^t * v * otherFact
        if(otherFact == 1.0) { // no point doing multiplication if otherFact= 1.0
      int otherSize= v.sz;
      const double *matrixDataPtr= m.getDataPtr();
      double *otherDataPtrA= v.theData;
      for(int i=0; i<sz; i++) {
	double *otherDataPtr= otherDataPtrA;
	double sum= 0.0;
	for(int j=0; j<otherSize; j++)
	  sum += *matrixDataPtr++ * *otherDataPtr++;
	double value= theData[i] * thisFact + sum;
	theData[i]= value;
      }
    } else if(otherFact == -1.0) { // no point doing multiplication if otherFact= 1.0
      int otherSize= v.sz;
      const double *matrixDataPtr= m.getDataPtr();
      double *otherDataPtrA= v.theData;
      for(int i=0; i<sz; i++) {
	double *otherDataPtr= otherDataPtrA;
	double sum= 0.0;
	for(int j=0; j<otherSize; j++)
	  sum += *matrixDataPtr++ * *otherDataPtr++;
	double value= theData[i] * thisFact - sum;
	theData[i]= value;
      }
    } else {
      int otherSize= v.sz;
      const double *matrixDataPtr= m.getDataPtr();
      double *otherDataPtrA= v.theData;
      for(int i=0; i<sz; i++) {
	double *otherDataPtr= otherDataPtrA;
	double sum= 0.0;
	for(int j=0; j<otherSize; j++)
	  sum += *matrixDataPtr++ * *otherDataPtr++;
	double value= theData[i] * thisFact + sum * otherFact;
	theData[i]= value;
      }
    }
}

  return 0;
}
	
	

//! @brief Returns the cuadrado del módulo del vector.
double XC::Vector::Norm2(void) const
  {
    double value= 0.0;
    for(int i=0; i<sz; i++)
      {
        const double &data= theData[i];
        value+= data*data;
      }
    return value;
  }

//! @brief Method to return the norm of  vector.
double XC::Vector::Norm(void) const
  { return sqrt(Norm2()); }

//! @brief Returns the máximo de los valores absolutos de las componentes del vector (norma_infinito).
double XC::Vector::NormInf(void) const
  {
    double retval= 0.0;
    for(int i=0; i<sz; i++)
      retval= std::max(retval,fabs(theData[i]));
    return retval;
  }

double XC::Vector::pNorm(int p) const
  {
    double value= 0.0;
  
    if(p>0)
      {
        for(register int i=0; i<sz; i++)
          {
            const double data= fabs(theData[i]);
            value+= pow(data,p);
          }
        return pow(value,1.0/p);
      }
    else
      {
        for(register int i=0; i<sz; i++)
          {
            const double data= fabs(theData[i]);
            value= (data>value) ? data : value;
          }
        return value;
      }
}


//! @brief Returns the number at the row being passed as parameter.
double &XC::Vector::at(const size_t &f)
  {
    if(CheckIndice0(f))
      return theData[f];
    else
      return VECTOR_NOT_VALID_ENTRY;
  }
//! @brief Returns the number at the row being passed as parameter.
const double &XC::Vector::at(const size_t &f) const
  {
    if(CheckIndice0(f))
      return theData[f];
    else
      return VECTOR_NOT_VALID_ENTRY;
  }
//! @brief Checks the renge for the index being passed as parameter.
bool XC::Vector::CheckIndice0(const size_t &i) const
  {
    if(i<0 || i>=size_t(sz))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; index: " << i
		  << " out of range: 0 - << " << sz-1 << std::endl;
        return false;
      }
    else
      return true;
  }

double &XC::Vector::operator[](int x) 
  {
#ifdef _G3DEBUG
  // check if it is inside range [0,sz-1]
    if(!CheckIndice(x))
     return VECTOR_NOT_VALID_ENTRY;
#endif
    return theData[x];
  }

const double &XC::Vector::operator[](int x) const
  {
#ifdef _G3DEBUG
  // check if it is inside range [0,sz-1]
    if(!CheckIndice(x))
     return VECTOR_NOT_VALID_ENTRY;
#endif
    return theData[x];
  }


// operator()(const XC::ID &rows) const
//! @brief Method to return a vector whose components are the components of the
//! current vector located in positions given by the XC::ID rows.
XC::Vector XC::Vector::operator()(const XC::ID &rows) const
  {
    // create a new_ Vector to be returned
    Vector result(rows.Size());

    // check if obtained VEctor of correct size
    if(result.Size() != rows.Size())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; new_ XC::Vector could not be constructed\n";
        return result;
      }

    // copy the appropraite contents from current to result     
    int pos;
    for(int i=0; i<rows.Size(); i++)
      {
        pos= rows(i);
        if(pos <0 || pos >= sz)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; invalid location " 
                      << pos << " outside range [0, " << sz-1 << "]\n";
          }
        else
          result(i)= (*this)(pos);
      }
    return result;
  }


//! @brief the assignment operator, This is assigned to be a copy of V. if sizes
//! are not compatable this.theData [] is deleted. The data pointers will not
//! point to the same area in mem after the assignment.
XC::Vector &XC::Vector::operator=(const Vector &V) 
  {
    // first check we are not trying v= v
    if(this != &V)
      {
        if(sz != V.sz)
          alloc(V.sz);
        // copy the data
        for(int i=0; i<sz; i++)
	  theData[i]= V.theData[i];
      }
    return *this;
  }





// Vector &operator+=(double fact):
//! @brief The += operator adds fact to each element of the vector, data[i]= data[i]+fact.
XC::Vector &XC::Vector::operator+=(double fact)
  {
  if(fact != 0.0)
    for(int i=0; i<sz; i++)
      theData[i] += fact;
  return *this;    
}



// Vector &operator-=(double fact)
//! @brief The -= operator subtracts fact from each element of the vector, data[i]= data[i]-fact.
XC::Vector &XC::Vector::operator-=(double fact)
{
  if(fact != 0.0)
    for(int i=0; i<sz; i++)
      theData[i] -= fact;
  return *this;    
}



//! @brief The *= operator multiplies each element by the factor.
XC::Vector &XC::Vector::operator*=(double fact)
  {
    for(int i=0; i<sz; i++)
      theData[i] *= fact;
    return *this;
  }


const double VECTOR_VERY_LARGE_VALUE= 1.0e200;

// Vector &operator/=(double fact):
//! @brief The /= operator divides each element of the vector by fact, theData[i]= theData[i]/fact.
//! Program exits if divide-by-zero would occur with warning message.
XC::Vector &XC::Vector::operator/=(double fact)
  {
    if(fact == 0.0)
      { // instead of divide-by-zero error set to VECTOR_VERY_LARGE_VALUE
        for(int i=0; i<sz; i++)
          theData[i]= VECTOR_VERY_LARGE_VALUE;
      }
    else
      {
        for(int i=0; i<sz; i++)
          theData[i] /= fact;
      }
    return *this;
  }




// Vector operator+(double fact):
//! @brief The + operator returns a XC::Vector of the same size as current, whose components
//! are return(i)= theData[i]+fact;
XC::Vector  XC::Vector::operator+(double fact) const
  {
    Vector result(*this);
    if(result.Size() != sz) 
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ran out of memory for new_ Vector\n";
    result+= fact;
    return result;
  }


// Vector operator-(double fact):
//! @brief The + operator returns a XC::Vector of the same size as current, whose components
//! are return(i)= theData[i]-fact;
XC::Vector  XC::Vector::operator-(double fact) const
  {
    Vector result(*this);
    if(result.Size() != sz) 
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ran out of memory for new_ Vector\n";
    result-= fact;
    return result;
  }



//! @brief The + operator returns a vector of the same size as current,
//! whose components are return(i)= theData[i]*fact;
XC::Vector  XC::Vector::operator*(double fact) const
  {
    Vector result(*this);
    if(result.Size() != sz) 
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ran out of memory for new_ Vector\n";
    result*= fact;
    return result;
  }


// Vector operator/(double fact):
//! @brief The + operator returns a vector of the same size as current,
//! whose components are return(i)= theData[i]/fact;
//! Exits if divide-by-zero error.
XC::Vector XC::Vector::operator/(double fact) const
  {
    if(fact == 0.0) 
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; divide-by-zero error coming\n";

    Vector result(*this);
    if(result.Size() != sz) 
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ran out of memory for new_ Vector\n";
    result/= fact;
    return result;
  }

//! @brief Unary minus operator.
XC::Vector XC::Vector::operator-(void) const
  {
    Vector retval(*this);
    for(int i=0; i<sz; i++)
      retval.theData[i]= -retval.theData[i];
    return retval;
  }
  
// Vector &operator+=(const XC::Vector &V):
//! @brief The += operator adds V's data to data, data[i]+=V(i). A check to see if
//! vectors are of same size is performed if VECTOR_CHECK is defined.
XC::Vector &XC::Vector::operator+=(const Vector &other)
  {
#ifdef _G3DEBUG
    if(sz != other.sz)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING vectors not of same sizes: "
		  << sz << " != " << other.sz << std::endl;
        return *this;
      }    
#endif
    for(int i=0; i<sz; i++)
      theData[i]+= other.theData[i];
    return *this;	    
  }



// Vector &operator-=(const XC::Vector &V):
//! @brief The -= operator subtracts V's data from  data, data[i]+=V(i). A check 
//! to see if vectors are of same size is performed if VECTOR_CHECK is defined.
XC::Vector &XC::Vector::operator-=(const XC::Vector &other)
  {
#ifdef _G3DEBUG
  if(sz != other.sz) {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; WARNING vectors not of same sizes: "
	      << sz << " != " << other.sz << std::endl;
    return *this;
  }
#endif
  
  for(int i=0; i<sz; i++)
    theData[i] -= other.theData[i];
  return *this;    
}



// Vector operator+(const XC::Vector &V):
//! The + operator checks the two vectors are of the same size if VECTOR_CHECK is defined.
//! Then returns a XC::Vector whose components are the vector sum of current and V's data.
XC::Vector  XC::Vector::operator+(const XC::Vector &b) const
  {
#ifdef _G3DEBUG
    if(sz != b.sz)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING vectors not of same sizes: "
		  << sz << " != " << b.sz << std::endl;
        return *this;
      }
#endif

    Vector result(*this);

    // check new_ Vector of correct size
    if(result.Size() != sz)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vector not of correct size \n";
        return result;
      }
    result+= b;
    return result;
  }


// Vector operator-(const XC::Vector &V):
//! @brief The - operator checks the two vectors are of the same size and then returns a XC::Vector
//! whose components are the vector difference of current and V's data.
XC::Vector XC::Vector::operator-(const XC::Vector &b) const
  {
#ifdef _G3DEBUG
    if(sz!=b.sz)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING vectors not of same sizes: "
		  << sz << " != " << b.sz << std::endl;
        return *this;
      }
#endif

    Vector result(*this);

    // check new_ Vector of correct size
    if(result.Size() != sz)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vector not of correct size \n";
        return result;
      }
    result-= b;
    return result;
  }



//! @brief Method to perform (Vector)transposed * vector.
double XC::Vector::operator^(const XC::Vector &V) const
  {
#ifdef _G3DEBUG
    if(sz != V.sz)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING vectors not of same sizes: " 
                  << sz << " != "  << V.sz << std::endl;
        return 0.0;
      }
#endif

    double result= 0.0;
    double *dataThis= theData;
    double *dataV= V.theData;
    for(register int i=0; i<sz; i++)
      result+= *dataThis++ * *dataV++;
    return result;
  }

double XC::Vector::dot(const Vector &v) const
  { return operator^(v); }    

double XC::dot(const Vector &a,const Vector &b)
  { return a^b;}

// Vector operator/(const XC::Matrix &M) const;    
//! @brief Method to return inv(M)*this
XC::Vector XC::Vector::operator/(const XC::Matrix &M) const
  {
    Vector res(M.noRows());
    if(M.noRows() != M.noCols())
      { // if not square do least squares solution
        Matrix A(M^M);
        A.Solve(*this, res);    
      }
    else
      {
        M.Solve(*this, res);
      }
    return res;
  }

//! @brief Escribe el vector en un archivo binario.
void XC::Vector::write(std::ofstream &os)
  {
    const int sz= Size();
    const size_t nb= getNumBytes();
    os.write((char *) &sz,sizeof sz);
    os.write((char *) &nb,sizeof nb);
    if(theData)
      os.write(reinterpret_cast<char *>(theData),nb);
  }

//! @brief Lee el vector de un archivo binario.
void XC::Vector::read(std::ifstream &is)
  {
    int sz= 0;
    size_t nb= 0;
    is.read((char *) &sz,sizeof sz);
    is.read((char *) &nb,sizeof nb);
    if(sz>0)
      {
        resize(sz);
        is.read(reinterpret_cast<char *>(theData),nb);
      }
  }

//! @brief Producto tensorial de dos tensores de primer orden.
//! @param u: vector fila.
//! @param v: vector columna.
XC::Matrix XC::prod_tensor(const Vector &u,const Vector &v)
  {
    const size_t sz_u= u.Size();
    const size_t sz_v= v.Size();
    Matrix retval(sz_u,sz_v);
    for(size_t i=0;i<sz_u;i++)
      for(size_t j=0;j<sz_v;j++)
        retval(i,j)= u[i]*v[j];
    return retval;
  }

//! @brief tensor product.
XC::Matrix XC::operator&(const Vector &u,const Vector &v)
  { return prod_tensor(u,v); }

//! @brief Returns a string that represents the vector.
std::string XC::to_string(const XC::Vector &V)
  {
    //Doing this way clients will be able to manage the formatting
    //with things like 'std::scientific << std::setprecision(10)' 
    std::ostringstream ss; 
    ss << V;
    return ss.str();
  }


//! @brief A function is defined to allow user to print the vectors using std::ostream.
std::ostream &XC::operator<<(std::ostream &s, const Vector &V)
  {
    for(int i=0; i<V.Size(); i++) 
        s << V(i) << " ";
    return s;
  }

//! @brief * operator.
XC::Vector XC::operator*(double a, const XC::Vector &V)
  { return V * a; }

int XC::Vector::Assemble(const XC::Vector &V, int init_pos, double fact) 
  {
    int res= 0;
    int cur_pos  = init_pos;  
    int final_pos= init_pos + V.sz - 1;
  
    if((init_pos >= 0) && (final_pos < sz))
      {
        for(int j=0; j<V.sz; j++) 
          (*this)(cur_pos++) += V(j)*fact;
      }
    else 
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; (const Vector &V, int init_pos, double fact): "
                  << "position outside bounds \n";
        res= -1;
      }
    return res;
  }

int XC::Vector::Extract(const Vector &V, int init_pos, double fact) 
  {
    int res= 0;
    int cur_pos= init_pos;  
    const int final_pos= init_pos + sz - 1;
  
    if((init_pos >= 0) && (final_pos < V.sz))
      {
        for(register int j=0; j<sz; j++) 
          (*this)(j)= V(cur_pos++)*fact;
      }
    else 
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; (const Vector &V, int init_pos, double fact): "
                  << "position outside bounds \n";
        res= -1;
      }
    return res;
  }

//! @brief Returns a vector with the specified subset of components.
XC::Vector XC::Vector::getComponents(const ID &idx) const 
  {
    const int sz= idx.Size();
    Vector retval(sz);
    for(int i= 0;i<sz;i++)
      retval[i]= (*this)(idx(i));
    return retval;
  }

//! @brief Assigns the specified values to the specified set of vecto's components
void XC::Vector::putComponents(const Vector &v,const ID &idx)
  {
    const int sz= idx.Size();
    for(int i= 0;i<sz;i++)
      (*this)(idx(i))= v(i);
  }

//! @brief Sums the specified values to the specified set of vecto's components
void XC::Vector::addComponents(const Vector &v,const ID &idx)
  {
    const int sz= idx.Size();
    for(int i= 0;i<sz;i++)
      (*this)(idx(i))+= v(i);
  }


//! @brief Convierte el vector en un std::vector<double>.
std::vector<double> XC::vector_to_std_vector(const XC::Vector &v)
  {
    const size_t sz= v.Size();
    std::vector<double> retval(sz,0.0);
    for(register size_t i=0;i<sz;i++)
      retval[i]= v[i];
    return retval;
  }

//! @brief Convierte el vector en un m_double.
m_double XC::vector_to_m_double(const XC::Vector &v)
  {
    const size_t fls= v.Size();
    m_double retval(fls,1,0.0);
    for(register size_t i=0;i<fls;i++)
      retval(i+1,1)= v(i);
    return retval;
  }

// //! @brief Convierte en vector la cadena de caracteres being passed as parameter.
// void XC::Vector::from_string(const std::string &str)
//   {
//     std::vector<double> tmp= create_vector_double(str);
//     const int nc=tmp.size(); //Number of values.
//     resize(nc);
//     for(register int i= 0;i<nc;i++)
//       theData[i]= tmp[i];
//   }

// XC::Vector XC::interpreta_xc_vector(const std::string &str)
//   {
//     Vector retval(1);
//     retval.from_string(str);
//     return retval;
//   }

// //! @brief Intenta, por todos los medios, convertir el argumento en un vector.
// XC::Vector XC::convert_to_vector(const boost::any &operand)
//   {
//     const std::vector<double> tmp= convert_to_vector_double(operand);
//     const size_t sz= tmp.size();
//     XC::Vector retval(sz);
//     for(size_t i= 0;i<sz;i++)
//       retval[i]= tmp[i];
//     return retval;
//   }
