//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
// $Revision: 1.5 $
// $Date: 2004/07/20 22:44:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/nDarray.cpp,v $
                                                                        
                                                                        
//############################################################################
//#                                                                          #
//#             /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\              #
//#            |                                          |____|             #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                 B A S E                  |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |              C L A S S E S               |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |          C + +     S O U R C E           |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#         /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/   |                  #
//#        |                                         |    |                  #
//#         \_________________________________________\__/                   #
//#                                                                          #
//#                                                                          #
//############################################################################
//
//   "C makes it easy to shoot yourself in the foot, C++ makes it harder,
//   but when you do, it blows away your whole leg" -- Bjarne Stroustrup
//
//#############################################################################
//                         PAZI 'VAMO:
//
//
//0. make more types for (int), (int, int),
//   (int,int,int) . . .
//
//1. for efficiency code val(int), val(int, int) . . .  up to
//   fourth order inline . . . also operator() . . .
//   Check in Ellis & Stroustrup about inline functions
//
//2. Code symmetric operators ( *, +, -, : . . . ) as friends
//   see Coplien's recomendation . . .
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
///*
//################################################################################
//# COPY-YES  (C):     :-))                                                   #
//# PROJECT:           Object Oriented Finite XC::Element Program                 #
//# PURPOSE:                                                                  #
//# CLASS:             nDarray                                                #
//#                                                                           #
//# VERSION:                                                                  #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                   #
//# DESIGNER(S):       Boris Jeremic                                          #
//# PROGRAMMER(S):     Boris Jeremic                                          #
//#                                                                           #
//#                                                                           #
//# DATE:              May 28. - July  21  '93                                #
//# UPDATE HISTORY:    august 05. - august __ '93 base_03 ( BJmatrix, BJvector,   #
//#                                               skyBJmatrix ...) from this one#
//#                    August 22-29 '94 choped to separate files and worked on#
//#                                   const and & issues                      #
//#                    August 30-31 '94 added use_def_dim to full the CC      #
//#                                   resolved problem with temoraries for    #
//#                                   operators + and - ( +=, -= )            #
//#                    January 16 '95 fixed the memory leakage introduced     #
//#                                   by previous work on +=, -+. I was       #
//#                                   by mistake decreasing                   #
//#                                   this->pc_nDarray_rep->total_numb--;     #
//#                                   inststead of                            #
//#                                   this->pc_nDarray_rep->n--;              #
//#                    28June2004     added val4 for efficiency still         #
//#                                   to be worked on                         #
//#                                                                           #
//#                                                                           #
//#############################################################################
//*/
// nDarray.cc

#include "nDarray_rep.h"
#include <iostream>

//! @brief Initialize dimensions vector.
void XC::nDarray_rep::init_dim(const size_t &sz, const int &default_dim)
  {
     dim= std::vector<int>(sz);// array for dimensions.
     this->total_numb= 1;
     for(int i = 0 ; i<get_nDarray_rank() ; i++ )
       {
	 dim[i]= default_dim;
	 this->total_numb*= default_dim;
       }
  }

//! @brief Initialize dimensions vector.
void XC::nDarray_rep::init_dim(const std::vector<int> &pdim)
  {
    dim= pdim; // array for dimensions.
    this->total_numb= 1;
    for(int i= 0 ; i<get_nDarray_rank() ; i++ )
      this->total_numb*= dim[i];
  }

//! @brief Initialize data vector.
void XC::nDarray_rep::init_data(void)
  {
    pd_nDdata= std::vector<double>(static_cast<size_t>(this->total_numb));
  }

//! @brief Initialize data vector.
void XC::nDarray_rep::init_data(const double &initval)
  {
    pd_nDdata= std::vector<double>(static_cast<size_t>(this->total_numb),initval);
  }

//! @brief Initialize data vector.
void XC::nDarray_rep::init_data(const double *values)
  {
    init_data();
    for(size_t i=0 ; i<this->total_numb ; i++ )
      pd_nDdata[i] = values[i];
  }

//! @brief Initialize data vector.
void XC::nDarray_rep::init_data(const std::vector<double> &values)
  {
    init_data();
    const size_t sz= values.size();
    size_t mn= this->total_numb;
    if(sz<this->total_numb)
      {
	mn= sz;
        std::cerr << "ERROR; not enough values ("
	  	  << sz << "<" << mn << ")."
	          << std::endl;
      }
    for(size_t i=0 ; i<mn ; i++ )
      pd_nDdata[i]= values[i];
  }

//! @brief Initialize data vector.
void XC::nDarray_rep::init_data(const boost::python::list &l)
  {
    init_data();
    const size_t sz= boost::python::len(l);
    size_t mn= this->total_numb;
    if(sz<this->total_numb)
      {
	mn= sz;
        std::cerr << "ERROR; not enough values ("
	  	  << sz << "<" << mn << ")."
	          << std::endl;
      }
    for(size_t i=0 ; i<this->total_numb ; i++ )
      pd_nDdata[i] =boost::python::extract<double>(l[i]);
  }

void XC::nDarray_rep::reset_data_to(const double &d)
  {
    for(size_t i=0 ; i<this->total_numb ; i++ )
      pd_nDdata[i] = d;
  }

void XC::nDarray_rep::sum_data(const std::vector<double> &rval)
  {
    for(size_t i=0 ; i<this->total_numb; i++ )
      pd_nDdata[i]+= rval[i];
  }

void XC::nDarray_rep::substract_data(const std::vector<double> &rval)
  {
    for(size_t i=0 ; i<this->total_numb; i++ )
      pd_nDdata[i]-= rval[i];
  }

void XC::nDarray_rep::neg(void)
  {
    for(size_t i=0 ; i<this->total_numb; i++ )
      pd_nDdata[i]= -pd_nDdata[i];
  }

double XC::nDarray_rep::sum() const
  {
    double retval= 0.0;
    for(size_t memb=0; memb<this->total_numb; memb++ )
      retval+= pd_nDdata[memb];
    return retval;
  }

bool XC::nDarray_rep::equal_data(const std::vector<double> &rval) const
  {
    bool retval= true;
    const size_t sz1= pd_nDdata.size();
    const size_t sz2= rval.size();
    if(sz1!=sz2)
      {
	std::cerr << "nDarray_rep::" << __FUNCTION__
	          << "; arrays of different sizes: "
	          << sz1 << "!=" << sz2
	          << std::endl;
      }
    const size_t sz= std::min(sz1,sz2);
    const double sqrt_d_macheps = sqrt(d_macheps());
    const double tolerance = sqrt_d_macheps;
    for(size_t i= 0;i<sz; i++)
      {
	const double &v1= pd_nDdata[i];
	const double &v2= rval[i];
        if(fabs(v1-v2) >= tolerance)
	  {
	    retval= false;
	    break;
	  }
      }
    return retval;
  }

bool XC::nDarray_rep::operator==(const nDarray_rep &rval) const
  {
    bool retval= (this->get_nDarray_rank()== rval.get_nDarray_rank());


    if(!retval)
      {
	std::clog << "nDarray_rep::" << __FUNCTION__
	          << "; WARNING: arrays of different ranks."
	          << std::endl;
      }
    else
      {
	retval= this->equal_dim(rval.dim);
	if(!retval)
	  {
	    std::clog << "nDarray_rep::" << __FUNCTION__
	              << "; WARNING: arrays of different dimensions."
	              << std::endl;
	  }
	else
	  retval= this->equal_data(rval.pd_nDdata);
      }
    return retval;    
  }

void XC::nDarray_rep::print(std::ostream &os) const
  {
    os << "dim= ";
    for(int i: this->dim)
      os << i << ' ';
    os << std::endl;
    os << "total_numb= " << total_numb << std::endl;
    os << "values= ";
    for(double d: this->pd_nDdata)
      os << d << ' ';
  }

std::ostream& XC::operator<<(std::ostream &os, const XC::nDarray_rep &a)
  {
    a.print(os);
    return os;
  }
