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
//MapLoadPatterns.h

#ifndef MAPLOADPATTERNS_H
#define MAPLOADPATTERNS_H

#include "preprocessor/loaders/LoadLoaderMember.h"
#include "domain/load/pattern/LoadPattern.h"
#include "domain/load/pattern/TimeSeries.h"
#include <map>

namespace XC {
class TimeSeries;
class LoadLoader;
class Domain;

//! @ingroup LPatterns
//
//! @brief Contenedor de pautas de carga.
class MapLoadPatterns: public LoadLoaderMember
  {
    typedef std::map<std::string,TimeSeries *> map_timeseries;
    map_timeseries tseries; //!< Modulación de cargas/desplaz. en el tiempo.
    std::string nmb_ts; //!< Identificacor del TimeSeries para los nuevos load pattern.

    typedef std::map<std::string,LoadPattern *> map_loadpatterns; //!< Casos de carga.
    map_loadpatterns loadpatterns; //!< Contenedor de casos de carga.
    std::string lpcode; //!< Identificador del load pattern para las nuevas cargas.
    int tag_el; //!< Tag por defecto para la nueva carga sobre elemento.
    int tag_nl; //!< Tag por defecto para la nueva carga sobre nodo.
    int tag_eel; //!< OBSOLETO Tag por defecto para la nueva carga sobre borde de elemento.
    int tag_spc; //!< Tag por defecto para el nuevo desplazamiento impuesto sobre nodo.

    template <class TS>
    TimeSeries *crea_time_series(const std::string &);
    template <class LP>
    LoadPattern *crea_load_pattern(const std::string &);
  public:
    typedef map_loadpatterns::iterator iterator;
    typedef map_loadpatterns::const_iterator const_iterator;
  protected:
    friend class LoadLoader;
    void clear_time_series(void);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    MapLoadPatterns(LoadLoader *owr);
    ~MapLoadPatterns(void);

    void clear(void);

    const_iterator begin(void) const
      { return loadpatterns.begin(); }
    const_iterator end(void) const
      { return loadpatterns.end(); }
    iterator begin(void)
      { return loadpatterns.begin(); }
    iterator end(void)
      { return loadpatterns.end(); }
    size_t size(void) const
      { return loadpatterns.size(); }
    bool empty(void) const
      { return loadpatterns.empty(); }

    const std::string getCurrentLoadPatternId(void) const;
    LoadPattern *getCurrentLoadPatternPtr(void);
    const LoadPattern *getCurrentLoadPatternPtr(void) const;
    inline const std::string &getCurrentLoadPattern(void) const
      { return lpcode; }
    inline void setCurrentLoadPattern(const std::string &nmb)
      { lpcode= nmb; }
    LoadPattern *newLoadPattern(const std::string &,const std::string &);
    void addToDomain(const std::string &);
    void removeFromDomain(const std::string &);
    void removeAllFromDomain(void);

    TimeSeries *buscaTS(const int &);
    const TimeSeries *buscaTS(const int &) const;
    TimeSeries *buscaTS(const std::string &);
    const TimeSeries *buscaTS(const std::string &) const;
    LoadPattern *buscaLoadPattern(const std::string &);
    const LoadPattern *buscaLoadPattern(const std::string &) const;
    LoadPattern *buscaLoadPattern(const int &);
    const LoadPattern *buscaLoadPattern(const int &) const;
    const std::string &getNombreLoadPattern(const LoadPattern *) const;
    TimeSeries *newTimeSeries(const std::string &,const std::string &);
    inline const std::string &getCurrentTimeSeries(void) const
      { return nmb_ts; }
    inline void setCurrentTimeSeries(const std::string &nmb)
      { nmb_ts= nmb; }

    std::deque<std::string> getListaNombres(void) const;
    boost::python::list getKeys(void) const;

    inline const int &getCurrentElementLoadTag(void) const
      { return tag_el; }
    inline void setCurrentElementLoadTag(const int &n)
      { tag_el= n; }
    inline const int &getCurrentNodeLoadTag(void) const
      { return tag_nl; }
    inline void setCurrentNodeLoadTag(const int &n)
      { tag_nl= n; }

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);


  };

//! @brief Crea un objeto TimeSeries.
template <class TS>
TimeSeries *XC::MapLoadPatterns::crea_time_series(const std::string &cod_ts)
  {
    TimeSeries *ts= buscaTS(cod_ts);
    if(!ts) //No existe.
      {
        TS *nts= new TS();
        assert(nts);
        tseries[cod_ts]= nts;
        ts= nts;
      }
    nmb_ts= cod_ts;
    return ts;
  }

//! @brief Carga un objeto LoadPattern desde archivo
template <class LP>
LoadPattern *XC::MapLoadPatterns::crea_load_pattern(const std::string &cod_lp)
  {
    int &tag_lp= this->getTagLP();
    LoadPattern *lp= buscaLoadPattern(cod_lp);
    if(!lp) //No existe.
      {
	std::map<std::string,TimeSeries *>::const_iterator its= tseries.find(nmb_ts);
        if(its!= tseries.end())
          {
            lp= new LP(tag_lp);
            tag_lp++;
            if(lp)
              {
                lp->setTimeSeries(its->second);
                lp->set_owner(this);
                loadpatterns[cod_lp]= lp;
                //Si no hay mas lo convertimos en el caso por defecto.
                if(loadpatterns.empty())
                  lpcode= cod_lp;
              }
          }
        else
	  std::cerr << "MapLoadPatterns; ERROR " 
                    << ", no se encontró la función de modulación: " << nmb_ts << std::endl;
      }
    return lp;
  }



} // end of XC namespace

#endif
