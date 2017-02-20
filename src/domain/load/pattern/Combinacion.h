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
//Combinacion.h

#ifndef COMBINACION_H
#define COMBINACION_H

#include "domain/component/ForceReprComponent.h"

namespace XC {
class MapLoadPatterns;
class LoadPattern;
class LoadLoader;
class GrupoCombinaciones;

//! @ingroup LPatterns
//
//! @brief Load pattern combination (1.5*PP+1.0*CP+1.6*SC ...).
class Combinacion: public ForceReprComponent
  {
    LoadLoader *loader; //!< Puntero al objeto que conoce los nombres de las combinaciones.
    //! @brief Each of the terms (factor*LoadPattern) of the combination.
    class sumando: public EntCmd
      {
        float factor; //!< Factor that multiplies the load pattern effect.
        LoadPattern *lpattern; //!< Load pattern.
        void neg(void);
      public:
        sumando(const float &f= 1.0,LoadPattern *lp= NULL);
        //! @brief Returns the factor that multiplies the load pattern.
        const float &Factor(void) const;
        const LoadPattern *Caso(void) const;
        LoadPattern *Caso(void);
	const std::string &getNombreCaso(const MapLoadPatterns &lps) const;

        sumando getNeg(void) const;
        const sumando &suma(const sumando &otro);
        const sumando &resta(const sumando &otro);
        const sumando &multiplica(const float &otro);
        const sumando &divide(const float &otro);

	std::string getString(const MapLoadPatterns &,const std::string &fmt) const;
        void Print(std::ostream &os) const;
    
      };
  public:
    typedef std::deque<sumando> TDescomp; //!< Tipo contenedor para la descomposición de la combinación (1.5*PP+1.0*CP+1.6*SC ...).
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
    void agrega_sumando(const sumando &);
    void interpreta_descomp(const std::string &str);
    void limpia_ceros(void);
    void clear(void);
    const_iterator buscaCaso(const LoadPattern *) const;
    iterator buscaCaso(const LoadPattern *);

    iterator begin(void)
      { return descomp.begin(); }
    iterator end(void)
      { return descomp.end(); }

    Combinacion &suma(const Combinacion &otro);
    Combinacion &resta(const Combinacion &otro);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    int recvDescomp(void);


    friend class GrupoCombinaciones;
    Combinacion(GrupoCombinaciones *owr= NULL,const std::string &nmb= "",int tag= 0,LoadLoader *ll= NULL);
    inline void setNombre(const std::string &nmb)
      { nombre= nmb;}
    inline void setLoader(LoadLoader *ll)
      { loader= ll; }

  public:
    ~Combinacion(void);

    virtual void setDomain(Domain *theDomain);
    bool addToDomain(void);
    void removeFromDomain(void);

    inline const std::string &getNombre(void) const
      { return nombre; }

    inline void setDescomp(const std::string &descomp)
      { interpreta_descomp(descomp); }

    const GrupoCombinaciones *getGrupo(void) const;
    GrupoCombinaciones *getGrupo(void);

    const_iterator begin(void) const
      { return descomp.begin(); }
    const_iterator end(void) const
      { return descomp.end(); }
    size_t size(void) const
      { return descomp.size(); }
    bool empty(void) const
      { return descomp.empty(); }

    float getCoefCaso(const LoadPattern *) const;

    Combinacion &multiplica(const float &otro);
    Combinacion &divide(const float &otro);
    Combinacion &suma(const std::string &);
    Combinacion &resta(const std::string &);
    Combinacion &asigna(const std::string &);
    inline Combinacion &operator+=(const Combinacion &c)
      { return suma(c); }
    Combinacion &operator-=(const Combinacion &c)
      { return resta(c); }
    Combinacion &operator*=(const float &f)
      { return multiplica(f); }
    Combinacion &operator/=(const float &f)
      { return divide(f); }
    Combinacion operator+(const Combinacion &) const;
    Combinacion operator-(const Combinacion &) const;
    Combinacion operator*(const float &) const;
    Combinacion operator/(const float &) const;
    bool operator==(const Combinacion &) const;
    bool operator!=(const Combinacion &) const;
    bool dominaA(const Combinacion &otra) const;

    const Combinacion *getPtrCombPrevia(void) const;
    const std::string getNombreCombPrevia(void) const;
    int getTagCombPrevia(void) const;
    const std::string getDescompCombPrevia(void) const;
    const std::string getDescompRestoSobrePrevia(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    std::string getString(const std::string &fmt= "") const;
    virtual void Print(std::ostream &s, int flag =0) const;

  };

std::ostream &operator<<(std::ostream &os,const Combinacion &);

} // end of XC namespace

#endif
