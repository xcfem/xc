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
//MapSet.h
//Contenedor de conjuntos.

#ifndef MAPSET_H
#define MAPSET_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include <map>
#include <deque>


namespace XC {

class Domain;
class SetBase;
class Set;
class SetEstruct;
class EntMdlr;
class Preprocessor;
class Pnt;
class Edge;
class Face;
class Body;
class UniformGrid;

//!  \ingroup Set
//! 
//!  \brief Contenedor de conjuntos
//!  
class MapSet: public EntCmd, public MovableObject
  {
    static ID setsDbTags;//! dbTags para los conjuntos.
    static std::deque<std::string> setsClassNames; //! nombres de clase de conjuntos.
  public:
    typedef std::map<std::string,SetBase *> map_sets;
    typedef map_sets::iterator iterator;
    typedef map_sets::const_iterator const_iterator;
    typedef std::map<std::string,EntMdlr *> map_ent_mdlr;
  private:
    Preprocessor *preprocessor; //!< Pointer to preprocessor.

    map_sets sets; //!< Conjuntos de entidades.
    map_ent_mdlr entidades; //! Entidades del preprocessor (puntos, lineas, superficies,...).
    Set *total; //!< Puntero al conjunto universal (Se crea en el constructor).
    map_sets abiertos; //!< Conjuntos abiertos (aquellos a los que se añade cada nueva entidad creada en Cad).

    bool existe(const std::string &nmb) const;
    friend class EntMdlr;

    Set *crea_set(const std::string &nmb);
    SetEstruct *crea_set_estruct(const SetEstruct &);
    SetBase *broke_set(const std::string &,const std::string &);

    SetBase *busca_set(const std::string &nmb);
    SetEstruct *busca_set_estruct(const std::string &nmb);
    void setup_total(void);
    void clearSets(void);
    void clearAll(void);
    void reset(void);
    friend class Cad;
    friend class SetMeshComp;
    friend class Set;
    void abre_set(const std::string &nmb);
    void cierra_set(const std::string &nmb);
    iterator begin(void);
    iterator end(void);
      
    MapSet(const MapSet &otro);
    MapSet &operator=(const MapSet &otro);
  protected:
    const ID &getSetsDBTags(CommParameters &cp);
    const std::deque<std::string> &getSetsClassNames(void);
    DbTagData &getDbTagData(void) const;
    int sendSetsDbTags(int posDbTag,CommParameters &cp);
    int sendSetsClassNames(int posDbTag,CommParameters &cp);
    int receiveSetsDbTags(int posDbTag,int size,const CommParameters &cp);
    int receiveSetsClassNames(int pDbTg,int sz,const CommParameters &cp);
    int sendSets(int posDbTag1, int posDbTag2, int posDbTag3,CommParameters &cp);
    int receiveSets(int posDbTag1, int posDbTag2, int posDbTag3,const int &,const CommParameters &cp);
    int sendAbiertos(int posDbTag1, int posDbTag2,CommParameters &cp);
    int receiveAbiertos(int posDbTag1, int posDbTag2,const CommParameters &cp);

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    virtual bool procesa_comando(CmdStatus &);
    friend class Preprocessor;
    MapSet(Preprocessor *preprocessor= NULL);
  public:
    virtual any_const_ptr GetProp(const std::string &cod) const;
    virtual ~MapSet(void);

    EntMdlr *inserta_ent_mdlr(EntMdlr *ent_mdlr);

    Set *get_set_total(void)
      { return total; }
    const Set *get_set_total(void) const
      { return total; }
    inline const map_sets &get_sets_abiertos(void) const
      { return abiertos; }
    inline map_sets &get_sets_abiertos(void)
      { return abiertos; }
    Set *defSet(const std::string &);

    const SetBase *busca_set(const std::string &nmb) const;
    SetBase &getSet(const std::string &nmb);
    const_iterator begin(void) const;
    const_iterator end(void) const;
    std::set<SetBase *> get_sets(const Node *);
    std::set<SetBase *> get_sets(const Element *);
    std::set<SetBase *> get_sets(const Pnt *);
    std::set<SetBase *> get_sets(const Edge *);
    std::set<SetBase *> get_sets(const Face *);
    std::set<SetBase *> get_sets(const Body *);
    std::set<SetBase *> get_sets(const UniformGrid *);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
