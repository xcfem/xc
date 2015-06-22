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
//MapLineas.h

#ifndef MAPLineas_H
#define MAPLineas_H

#include "MapEnt.h"
#include "Edge.h"

namespace XC {

class Edge;
class Linea;
class LineaTramos;
class CmbEdge;
class ArcoCircunf;

//! @ingroup Cad
//
//! @brief Contenedor de puntos del modelo.
class MapLineas: public MapEnt<Edge>
  {
  private:
    void UpdateSets(Edge *) const;
  protected:
    template <class E>
    Edge *Crea(void);

    virtual bool procesa_comando(CmdStatus &status);
  public:
    MapLineas(Cad *cad= NULL);

    Vector3d getVector(const Indice &i,const Indice &j) const;
    Recta3d getRecta(const Indice &i,const Indice &j) const;
    Plano3d getPlano(const Indice &i,const Indice &j,const Indice &k) const;

    template <class E>
    Edge *Nueva(void);
    template <class E>
    Edge *Nueva(CmdStatus &);
    Edge *Line(Pnt *,Pnt *);
    Edge *DividedLine(Pnt *,Pnt *);
    Edge *Arc(Pnt *,Pnt *,Pnt *);
    Edge *LineSequence(void);
    Edge *Copia(const Edge *l);
 
    Linea *newLine(const size_t &, const size_t &);
    LineaTramos *newDividedLine(const size_t &, const size_t &);
    ArcoCircunf *newCircleArc(const size_t &, const size_t &, const size_t &);
    CmbEdge *newLineSequence(void);

    any_const_ptr GetProp(const std::string &cod) const;
  };

//! @brief Crea una nueva línea.
template <class E>
Edge *XC::MapLineas::Crea(void)
  {
    Modelador *mdlr= getModelador();
    assert(mdlr);
    Edge *retval= new E(mdlr);
    retval->Nombre()= "l"+boost::lexical_cast<std::string>(getTag());
    (*this)[getTag()]= retval;
    UpdateSets(retval);
    tag++;
    return retval;
  }

//! @brief Crea un nuevo Edge.
template <class E>
Edge *MapLineas::Nueva(void)
  {
    Edge *retval= busca(getTag());
    if(!retval) //El edge es nuevo.
      retval= Crea<E>();
    return retval;
  }

//! @brief Lee un Edge.
template <class E>
Edge *MapLineas::Nueva(CmdStatus &status)
  {
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    bool nueva= true;
    size_t old_tag= getTag();
    Edge *retval= NULL;
    if(fnc_indices.size()>0)
      {
        setTag(convert_to_size_t(fnc_indices[0])); //Identificador del punto.
        retval= busca(getTag());
      }
    if(retval)
      nueva= false;
    else
      retval= Crea<E>();
    if(!nueva)
      setTag(old_tag);
    retval->LeeCmd(status);
    return retval;
  }

} //fin namespace XC
#endif
