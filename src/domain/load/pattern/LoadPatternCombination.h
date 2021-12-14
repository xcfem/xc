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
//LoadPatternCombination.h

#ifndef LOADPATTERNCOMBINATION_H
#define LOADPATTERNCOMBINATION_H

#include "domain/component/ForceReprComponent.h"

namespace XC {
class MapLoadPatterns;
class LoadPattern;
class LoadHandler;
class LoadCombinationGroup;

//! @ingroup LPatterns
//
//! @brief Base class for load pattern combinations
//! (1.5*selfWeight+1.0*permanentLoad+1.6*trafficLoad ...).
class LoadPatternCombination: public ForceReprComponent
  {
  protected:
    std::string name;
    LoadHandler *handler; //!< Pointer to the load case manager.
    //used only to receive data.
    static std::map<int,std::string> map_str_descomp;

    //! @brief Each of the terms (factor*LoadPattern) of the combination.
    class summand: public CommandEntity
      {
        float factor; //!< Factor that multiplies the load pattern effect.
        LoadPattern *lpattern; //!< Load pattern.
        void neg(void);
      public:
        summand(const float &f= 1.0,LoadPattern *lp= nullptr);
        //! @brief Returns the factor that multiplies the load pattern.
        const float &getFactor(void) const;
        const LoadPattern *getLoadPattern(void) const;
        LoadPattern *getLoadPattern(void);
	const std::string &getLoadPatternName(const MapLoadPatterns &lps) const;

	bool set_gamma_f(void);
        summand getNeg(void) const;
        const summand &add(const summand &);
        const summand &subtract(const summand &);
        const summand &multiplica(const float &);
        const summand &divide(const float &);

	std::string getString(const MapLoadPatterns &,const std::string &fmt) const;
        void Print(std::ostream &os) const;
    
      };
  public:
    typedef std::deque<summand> TDescomp; //!< Container type for the combination expression (1.5*PP+1.0*CP+1.6*SC ...).
    typedef TDescomp::iterator iterator;
    typedef TDescomp::const_iterator const_iterator;
  protected:
    TDescomp descomp;

    void set_gamma_f(TDescomp &);
    void set_domain(void);
    
    void clear(void);
    void add_component(const summand &);
    void interpreta_descomp(const std::string &str);
    void limpia_ceros(void);
    const_iterator findLoadPattern(const LoadPattern *) const;
    iterator findLoadPattern(const LoadPattern *);

    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

    iterator begin(void)
      { return descomp.begin(); }
    iterator end(void)
      { return descomp.end(); }
    
    inline void setNombre(const std::string &nm)
      { name= nm;}
    bool add_to_domain(TDescomp &);
  public:
    LoadPatternCombination(int tag= 0,int classTag= LOAD_TAG_LoadPatternCombination,const std::string &nm= "", LoadHandler *ll= nullptr);
    ~LoadPatternCombination(void);
    inline void setHandler(LoadHandler *ll)
      { handler= ll; }
    
    inline const std::string &getName(void) const
      { return name; }

    virtual void setDomain(Domain *theDomain);

    bool addToDomain(void);
    bool addToDomain(const std::set<std::string> &filter);
    bool addToDomain(boost::python::list &filter);
    bool isActive(void) const;
    void removeFromDomain(void);

    inline void setDescomp(const std::string &descomp)
      { interpreta_descomp(descomp); }
    
    float getLoadPatternFactor(const LoadPattern *) const;

    const_iterator begin(void) const
      { return descomp.begin(); }
    const_iterator end(void) const
      { return descomp.end(); }
    size_t size(void) const
      { return descomp.size(); }
    bool empty(void) const
      { return descomp.empty(); }

    std::string getString(const std::string &fmt= "") const;
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);    
    virtual void Print(std::ostream &s, int flag =0) const;

  };

std::ostream &operator<<(std::ostream &os,const LoadPatternCombination &);

} // end of XC namespace

#endif
