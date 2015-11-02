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
//Body.h
//Class for block entities. (XXX: rename as Block?)

#ifndef BODY_H
#define BODY_H

#include "EntMdlr.h"
#include "CmbEdge.h"


class Pos3d;

namespace XC {

class Face;
class Pnt;

//! \ingroup CadEnt
//!
//! @brief Orden en de los lados de una cara del hexaedro.
struct SecuenciaLados
  {
    size_t l1; //!< Índice lado 1.
    size_t l2; //!< Índice lado 2.
    size_t l3; //!< Índice lado 3.
    size_t l4; //!< Índice lado 4.
    bool dirt; //!< Directo o inverso.

    SecuenciaLados(const size_t primero= 1,const bool &directo= true);
    //! @brief Devuelve verdadero si la secuencia de lados es directa.
    const bool &Directo(void) const
      { return dirt; }
  };

//! \ingroup CadEnt
//!
//! @brief Representación de un sólido de seis caras.
class Body: public EntMdlr
  {
  protected:
    //! \ingroup Geom
    //!
    //! @brief Esta clase sirve para almacenar cada una de las superficies que limitan el cuerpo.
    class Cara
      {
        Face *superficie; //!< Puntero a superficie.
        SecuenciaLados sec_lados; //!< Secuencia de lados.
      public:
        Cara(Face *ptr=NULL,const size_t &p=1,const bool &d=true);
	Face *Superficie(void);
        const Face *Superficie(void) const;
        void SetSurf(Face *s);

        const std::string &GetNombre(void) const;
        bool Vacia(void) const;
        size_t NumLineas(void) const;
        size_t NumVertices(void) const;
        const CmbEdge::Lado *GetLado(const size_t &) const;
        CmbEdge::Lado *GetLado(const size_t &);
        const Pnt *GetVertice(const size_t &) const;
        Pnt *GetVertice(const size_t &);
        virtual MatrizPos3d get_posiciones(void) const;
        void crea_nodos(void);
        bool checkNDivs(void) const;
        Node *GetNodo(const size_t &,const size_t &);
      };

    void set_surf(Face *s);
    bool procesa_comando(CmdStatus &status);
    virtual Cara *GetCara(const size_t &i)= 0;
  public:
    Body(Preprocessor *m,const std::string &nombre= "");
    //! @brief Devuelve la dimensión del objeto
    inline virtual unsigned short int GetDimension(void) const
      { return 3; }
    //! @brief Devuelve el número de líneas del objeto.
    virtual size_t NumLineas(void) const= 0;
    //! @brief Devuelve el número de vértices del objeto.
    virtual size_t NumVertices(void) const= 0;
    //! @brief Devuelve el número de vértices del objeto.
    virtual size_t NumCaras(void) const= 0;
    virtual std::set<const Face *> GetSuperficies(void)= 0;
    virtual const Cara *GetCara(const size_t &i) const= 0;
    virtual const CmbEdge::Lado *GetArista(const size_t &i) const= 0;
    virtual const Pnt *GetVertice(const size_t &i) const= 0;
    std::vector<int> getIndicesVertices(void) const;
    virtual BND3d Bnd(void) const;

    virtual bool checkNDivs(void) const= 0;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    any_const_ptr GetProp(const std::string &cod) const;
  };

std::set<const Body *> GetCuerposTocan(const Face &s);

} //fin namespace XC

#endif
