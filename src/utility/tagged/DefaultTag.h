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
                                                                        
#ifndef DefaultTag_h
#define DefaultTag_h


namespace XC {
//! @brief Tag por defecto.
class DefaultTag
  {
  private:
    int default_tag; //<! tag para el nuevo nodo.
  public:
    // constructors
    inline DefaultTag(int tag= 0)
      : default_tag(tag) {}

    inline DefaultTag &operator=(const int &i)
      {
        default_tag= i;
        return *this;
      }
    inline void setTag(const int &tag)
      { default_tag= tag; }
    inline const int &getTag(void) const
      { return default_tag; }
    inline void incTag(void)
      { default_tag++; }
    inline int operator++(int)
      {
	int retval= default_tag;
        default_tag++;
        return retval;
      }
    inline int operator++(void)
      { return ++default_tag; }
  };


} // fin namespace XC

#endif

