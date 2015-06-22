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
//UniformGrid.h
//Malla con los nodos equiespaciados en x, y, y z.

#ifndef UNIFORMGRID_H
#define UNIFORMGRID_H

#include "EntMdlr.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

namespace XC {

//!  @ingroup CadEnt
//!
//!  @brief Malla con los nodos equiespaciados en x, y, y z.
class UniformGrid: public EntMdlr
  {
  private:
    Pos3d org; //!< Posición del origen de la malla.
    double Lx; //!< Dimensión de la malla paralela al eje x.
    double Ly; //!< Dimensión de la malla paralela al eje y.
    double Lz; //!< Dimensión de la malla paralela al eje z.
    size_t ndiv_x; //! Número de divisiones en el eje x.
    size_t ndiv_y; //! Número de divisiones en el eje y.
    size_t ndiv_z; //! Número de divisiones en el eje z.
  protected:
    virtual bool procesa_comando(CmdStatus &status);
    void actualiza_topologia(void);
    void crea_nodos(void);
  public:
    UniformGrid(Modelador *m,const size_t &i= 0);
    UniformGrid(const std::string &nombre= "",Modelador *m= NULL,const size_t &i= 0);
    SetEstruct *getCopy(void) const;
    virtual unsigned short int GetDimension(void) const;
    virtual any_const_ptr GetProp(const std::string &cod) const;

    inline void setOrg(const Pos3d &p)
      { org= p; }
    inline const Pos3d &getOrg(void) const
      { return org; }

    inline void setLx(const double &l)
      { Lx= l; }
    inline double getLx(void) const
      { return Lx; }
    inline void setLy(const double &l)
      { Ly= l; }
    inline double getLy(void) const
      { return Ly; }
    inline void setLz(const double &l)
      { Lz= l; }
    inline double getLz(void) const
      { return Lz; }

    inline void setNDivX(const size_t &l)
      { ndiv_x= l; }
    inline size_t getNDivX(void) const
      { return Lx; }
    inline void setNDivY(const size_t &l)
      { ndiv_y= l; }
    inline size_t getNDivY(void) const
      { return Ly; }
    inline void setNDivZ(const size_t &l)
      { ndiv_z= l; }
    inline size_t getNDivZ(void) const
      { return Lz; }

    virtual BND3d Bnd(void) const;
    Pos3d GetCentro(void) const;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    virtual void Malla(dir_mallado dm);
  };

} //fin namespace XC

#endif
