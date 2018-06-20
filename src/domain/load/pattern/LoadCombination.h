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
//LoadCombination.h

#ifndef LOADCOMBINATION_H
#define LOADCOMBINATION_H

#include "domain/component/ForceReprComponent.h"

namespace XC {
class MapLoadPatterns;
class LoadPattern;
class LoadHandler;
class LoadCombinationGroup;

//! @ingroup LPatterns
//
//! @brief Load pattern combination (1.5*PP+1.0*CP+1.6*SC ...).
class LoadCombination: public ForceReprComponent
  {
    LoadHandler *handler; //!< Pointer to the load case manager.
    //! @brief Each of the terms (factor*LoadPattern) of the combination.
    class summand: public EntCmd
      {
        float factor; //!< Factor that multiplies the load pattern effect.
        LoadPattern *lpattern; //!< Load pattern.
        void neg(void);
      public:
        summand(const float &f= 1.0,LoadPattern *lp= nullptr);
        //! @brief Returns the factor that multiplies the load pattern.
        const float &Factor(void) const;
        const LoadPattern *Caso(void) const;
        LoadPattern *Caso(void);
	const std::string &getNombreCaso(const MapLoadPatterns &lps) const;

        summand getNeg(void) const;
        const summand &add(const summand &otro);
        const summand &substract(const summand &otro);
        const summand &multiplica(const float &otro);
        const summand &divide(const float &otro);

	std::string getString(const MapLoadPatterns &,const std::string &fmt) const;
        void Print(std::ostream &os) const;
    
      };
  public:
    typedef std::deque<summand> TDescomp; //!< Container type for the combination expression (1.5*PP+1.0*CP+1.6*SC ...).
    typedef TDescomp::iterator iterator;
    typedef TDescomp::const_iterator const_iterator;
  private:
    std::string nombre;
    TDescomp descomp;
    //used only to receive data.
    static std::map<int,std::string> map_str_descomp;
  protected:
    void set_gamma_f(void);
    void set_domain(void);
    friend class Domain;
    friend class FEM_ObjectBroker;
    void add_component(const summand &);
    void interpreta_descomp(const std::string &str);
    void limpia_ceros(void);
    void clear(void);
    const_iterator buscaCaso(const LoadPattern *) const;
    iterator buscaCaso(const LoadPattern *);

    iterator begin(void)
      { return descomp.begin(); }
    iterator end(void)
      { return descomp.end(); }

    LoadCombination &add(const LoadCombination &otro);
    LoadCombination &substract(const LoadCombination &otro);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    int recvDescomp(void);


    friend class LoadCombinationGroup;
    LoadCombination(LoadCombinationGroup *owr= nullptr,const std::string &nmb= "",int tag= 0,LoadHandler *ll= nullptr);
    inline void setNombre(const std::string &nmb)
      { nombre= nmb;}
    inline void setHandler(LoadHandler *ll)
      { handler= ll; }

  public:
    ~LoadCombination(void);

    virtual void setDomain(Domain *theDomain);
    bool addToDomain(void);
    void removeFromDomain(void);

    inline const std::string &getName(void) const
      { return nombre; }

    inline void setDescomp(const std::string &descomp)
      { interpreta_descomp(descomp); }

    const LoadCombinationGroup *getGrupo(void) const;
    LoadCombinationGroup *getGrupo(void);

    const_iterator begin(void) const
      { return descomp.begin(); }
    const_iterator end(void) const
      { return descomp.end(); }
    size_t size(void) const
      { return descomp.size(); }
    bool empty(void) const
      { return descomp.empty(); }

    float getCoefCaso(const LoadPattern *) const;

    LoadCombination &multiplica(const float &otro);
    LoadCombination &divide(const float &otro);
    LoadCombination &add(const std::string &);
    LoadCombination &substract(const std::string &);
    LoadCombination &asigna(const std::string &);
    inline LoadCombination &operator+=(const LoadCombination &c)
      { return add(c); }
    LoadCombination &operator-=(const LoadCombination &c)
      { return substract(c); }
    LoadCombination &operator*=(const float &f)
      { return multiplica(f); }
    LoadCombination &operator/=(const float &f)
      { return divide(f); }
    LoadCombination operator+(const LoadCombination &) const;
    LoadCombination operator-(const LoadCombination &) const;
    LoadCombination operator*(const float &) const;
    LoadCombination operator/(const float &) const;
    bool operator==(const LoadCombination &) const;
    bool operator!=(const LoadCombination &) const;
    bool dominaA(const LoadCombination &otra) const;

    const LoadCombination *getPtrCombPrevia(void) const;
    const std::string getNombreCombPrevia(void) const;
    int getTagCombPrevia(void) const;
    const std::string getDescompCombPrevia(void) const;
    const std::string getDescompRestoSobrePrevia(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    std::string getString(const std::string &fmt= "") const;
    virtual void Print(std::ostream &s, int flag =0) const;

  };

std::ostream &operator<<(std::ostream &os,const LoadCombination &);

} // end of XC namespace

#endif
