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
//Block.h
//Objeto superficie del preprocesador.

#ifndef BLOCK_H
#define BLOCK_H

#include "Body.h"

namespace XC {

//! \ingroup CadEnt
//!
//! @brief Representación de un sólido de seis caras.
class Block: public Body
  {
    Cara sups[6];
    size_t indice(Face *s) const;
    void coloca(const size_t &i,Face *s);
  protected:
    void inserta(const size_t &);
    //XXX void add_puntos(const std::vector<size_t> &);
    void add_caras(const std::vector<size_t> &);
    void crea_nodos_caras(void);
    TritrizPos3d get_posiciones(void) const;
    bool procesa_comando(CmdStatus &status);
    virtual Cara *GetCara(const size_t &i);
  public:
    Block(Preprocessor *m,const std::string &nombre= "");
    SetEstruct *getCopy(void) const;
    //! @brief Devuelve el número de líneas del objeto.
    virtual size_t NumLineas(void) const;
    virtual size_t NumVertices(void) const;
    virtual size_t NumCaras(void) const;
    void actualiza_topologia(void);
    virtual std::set<const Face *> GetSuperficies(void);
    const Cara *GetCara(const size_t &i) const;
    const CmbEdge::Lado *GetArista(const size_t &i) const;
    Pnt *GetVertice(const size_t &i);
    const Pnt *GetVertice(const size_t &i) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    size_t NDivI(void) const;
    size_t NDivJ(void) const;
    size_t NDivK(void) const;
    bool checkNDivs(void) const;
    void crea_nodos(void);
    void Malla(dir_mallado dm);

    any_const_ptr GetProp(const std::string &cod) const;
  };


} //fin namespace XC

#endif
