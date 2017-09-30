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
//TransfCooLoader.h

#ifndef TRANSFCOOLOADER_H
#define TRANSFCOOLOADER_H

#include "Loader.h"
#include <map>

namespace XC {
class Domain;
class CrdTransf;
class LinearCrdTransf2d;
class LinearCrdTransf3d;
class PDeltaCrdTransf2d;
class PDeltaCrdTransf3d;
class CorotCrdTransf2d;
class CorotCrdTransf3d;


//! @ingroup Ldrs
//! 
//! @brief Manager for the creation/deletion of coordinate transformations
class TransfCooLoader: public Loader
  {
  public:
    typedef std::map<std::string,CrdTransf *> map_transfcoo;
    typedef map_transfcoo::const_iterator const_iterator;
    typedef map_transfcoo::iterator iterator;
  private:
    map_transfcoo transfcoo; //!< Coordinate transformation container.
    int tag_trf; //!< Tag de la coordinate transformation.
  protected:
    void libera(void);
    TransfCooLoader(const TransfCooLoader &otro);
    TransfCooLoader &operator=(const TransfCooLoader &otro);
  public:
    TransfCooLoader(Preprocessor *owr);
    const map_transfcoo &Map(void) const;

    template<class T>
    T *newCrdTransf(const std::string &);
    LinearCrdTransf2d *newLinearCrdTransf2d(const std::string &);
    LinearCrdTransf3d *newLinearCrdTransf3d(const std::string &);
    PDeltaCrdTransf2d *newPDeltaCrdTransf2d(const std::string &);
    PDeltaCrdTransf3d *newPDeltaCrdTransf3d(const std::string &);
    CorotCrdTransf2d *newCorotCrdTransf2d(const std::string &);
    CorotCrdTransf3d *newCorotCrdTransf3d(const std::string &);

    const_iterator begin(void) const;
    const_iterator end(void) const;
    iterator begin(void);
    iterator end(void);
    const_iterator find(const std::string &str) const;
    iterator find(const std::string &str);
    CrdTransf *find_ptr(const std::string &str);
    CrdTransf *find_ptr(const int &tag);
    const CrdTransf *find_ptr(const std::string &str) const;
    const CrdTransf *find_ptr(const int &tag) const;
    std::string getName(const int &tag) const;
    
    ~TransfCooLoader(void);

    void clearAll(void);
  };

template<class T>
T *TransfCooLoader::newCrdTransf(const std::string &trfName)
  {
    T *retval= nullptr;
    CrdTransf *ptr= find_ptr(trfName);
    if(ptr)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "transformation: '" << trfName
	          << "' aready exists. I do nothing." << std::endl;
	retval= dynamic_cast<T *>(ptr);
      }
    else
      {
        retval= new T(tag_trf++);
        assert(retval);
        retval->set_owner(this);
        transfcoo[trfName]= retval;
      }
    return retval;    
  }
} // end of XC namespace

#endif
