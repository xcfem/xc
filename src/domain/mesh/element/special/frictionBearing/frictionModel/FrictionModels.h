// -*-c++-*-
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
//FrictionModels.h

#ifndef FrictionModels_h
#define FrictionModels_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/kernel/CommandEntity.h"
#include "FrictionModel.h"

namespace XC {

class Response;
  
//! @brief Friction model container.
class FrictionModels: public CommandEntity, public MovableObject
  {
  public:
    typedef std::vector<FrictionModel *> friction_model_vector;
  protected:
    friction_model_vector theFrictionModels; //!< pointers to the friction model objects.
    

    void clearAll();
    void alloc(const std::vector<FrictionModel *> &);
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    typedef friction_model_vector::iterator iterator;
    typedef friction_model_vector::const_iterator const_iterator;
    
    FrictionModels(const size_t &nMat= 0,const FrictionModel *fModel= nullptr);
    FrictionModels(const size_t sz, const FrictionModel &);
    explicit FrictionModels(const FrictionModel &);
    FrictionModels(const FrictionModels &);
    FrictionModels &operator=(const FrictionModels &);    
    ~FrictionModels();
    
    inline iterator begin(void)
      { return theFrictionModels.begin(); }
    inline iterator end(void)
      { return theFrictionModels.end(); }
    inline const_iterator begin(void) const
      { return theFrictionModels.begin(); }
    inline const_iterator end(void) const
      { return theFrictionModels.end(); }
    inline size_t size() const
      { return theFrictionModels.size(); }
    inline FrictionModel *at(const size_t &i)
      { return theFrictionModels.at(i); }
    inline bool empty() const
      { return theFrictionModels.empty(); } 
    inline friction_model_vector &getFrictionModelsVector(void)
      { return theFrictionModels; }
    inline const friction_model_vector &getFrictionModelsVector(void) const
      { return theFrictionModels; }
    void setPyList(const boost::python::list &);
    boost::python::list getPyList() const;

    void clearFrictionModels(void);
    void setFrictionModel(const FrictionModel *);
    void setFrictionModel(size_t i,const FrictionModel *);
    void setFrictionModel(size_t i,const FrictionModel &);
    void copyFrictionModelsFrom(const FrictionModels &other);

    // public methods to set the state of the properties
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);


    inline FrictionModel *operator[](const size_t &i)
      { return theFrictionModels[i]; }
    inline const FrictionModel *operator[](const size_t &i) const
      { return theFrictionModels[i]; }

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    int getResponse(int responseID, Information &eleInformation);
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);

    virtual void Print(std::ostream &s, int) const;
  };


 
} //end of XC namespace
#endif
