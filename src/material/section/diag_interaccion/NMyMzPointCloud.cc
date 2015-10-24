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
//NMyMzPointCloud.cc

#include "NMyMzPointCloud.h"
#include "NMPointCloud.h"


//! @brief Constructor.
XC::NMyMzPointCloud::NMyMzPointCloud(const double &u)
  : NMPointCloudBase(u), GeomObj::list_Pos3d()
  {
    lastInserted= nullptr;
  }

void XC::NMyMzPointCloud::clear(void)
  {
    GeomObj::list_Pos3d::clear();
    NMPointCloudBase::clear();
    lastInserted= nullptr;
  }

//! @brief Inserta en la lista de esfuerzos que se pasa como parámetro
//! la resultante de tensiones normales en la sección.
const Pos3d *XC::NMyMzPointCloud::append(const Pos3d &NMyMz)
  {
    if(lastInserted)
      {
        if(dist(NMyMz,*lastInserted)>umbral)
          lastInserted= Agrega(NMyMz);
      }
    else
      lastInserted= Agrega(NMyMz);
    return lastInserted;  
  }

XC::NMPointCloud XC::NMyMzPointCloud::getNMy(void) const
  {
    NMPointCloud retval(umbral);
    for(const_iterator i= begin();i!=end();i++)
      {
        Pos3d p3d= *i;
        retval.push_back(Pos2d(p3d.x(),p3d.y()));
      }
    return retval;
  }

XC::NMPointCloud XC::NMyMzPointCloud::getNMz(void) const
  {
    NMPointCloud retval(umbral);
    for(const_iterator i= begin();i!=end();i++)
      {
        Pos3d p3d= *i;
        retval.push_back(Pos2d(p3d.x(),p3d.z()));
      }
    return retval;
  }

XC::NMPointCloud XC::NMyMzPointCloud::getNM(const double &theta) const
  {
    NMPointCloud retval(umbral);
    const double PImed= M_PI/2.0; 
    if(theta==PImed)
      retval= getNMy();
    else if(theta==0.0)
      retval= getNMz();
    else
      {
        const double c= cos(theta);
	const double s= sin(theta);
        double M= 0.0;
        for(const_iterator i= begin();i!=end();i++)
          {
            Pos3d p3d= *i;
            M= p3d.y()*s+p3d.z()*c; //My*sin(theta)+Mz*cos(theta)
            retval.push_back(Pos2d(p3d.x(),M));
          }
      }
    return retval; 
  }
