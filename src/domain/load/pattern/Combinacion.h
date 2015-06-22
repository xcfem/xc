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
//! @brief Combinación de pautas (casos) de carga (1.5*PP+1.0*CP+1.6*SC ...).
class Combinacion: public ForceReprComponent
  {
    LoadLoader *loader; //!< Puntero al objeto que conoce los nombres de las combinaciones.
    //! @brief Sumando de una combinación de casos de carga.
    class sumando: public EntCmd
      {
        float factor; //!< Factor de ponderación del caso de carga.
        LoadPattern *lpattern; //!< Caso de carga.
        void neg(void);
      public:
        sumando(const float &f= 1.0,LoadPattern *lp= NULL);
        //! @brief Devuelve el factor que multiplica al sumando.
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
        virtual any_const_ptr GetProp(const std::string &cod) const;
      };
  public:
    typedef std::deque<sumando> TDescomp; //!< Tipo contenedor para la descomposición de la combinación (1.5*PP+1.0*CP+1.6*SC ...).
    typedef TDescomp::iterator iterator;
    typedef TDescomp::const_iterator const_iterator;
  private:
    std::string nombre;
    TDescomp descomp;
    //sólo se usa para recibir datos.
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
    bool procesa_comando(CmdStatus &status);

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
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

std::ostream &operator<<(std::ostream &os,const Combinacion &);

} // fin namespace XC

#endif
