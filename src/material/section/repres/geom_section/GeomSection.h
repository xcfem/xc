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
//GeomSection.h

#ifndef GeomSection_h 
#define GeomSection_h 

#include "material/section/repres/geom_section/region/ListRegiones.h"
#include "material/section/repres/geom_section/reinfLayer/ListReinfLayer.h"
#include <list>
#include "boost/lexical_cast.hpp"
#include "Axis.h"
#include "material/section/repres/SectionMassProperties.h"

class Semiplano2d;
class Segmento2d;

namespace XC {
class SectionReferenceFrame;
class Spot;
class Segment;
class RegionSecc;
class ReinfLayer;
class MaterialHandler;
class Material;
class Vector;
class Matrix;
class CrossSectionProperties3d;
class CrossSectionProperties2d;

//! @ingroup MATSCCRepres
//
//! @defgroup MATSCCRepresGeom Geometric representation of a cross-section.
//
//! @ingroup MATSCCRepresGeom
//
//! @brief Cross section geometry.
class GeomSection: public SectionMassProperties
  {
  public:

    typedef std::map<size_t,SectionReferenceFrame *> lst_sis_ref;//!< reference systems container.
    typedef std::map<size_t,Spot *> lst_spots; //!< point container.
    typedef std::map<size_t,Axis *> axes_container; //!< line container.

  protected:
    MaterialHandler *material_handler; //!< Material handler (searching,...).

    ListRegiones regiones; //!< Region container.
    ListReinfLayer reinforcement_layers; //!< Rebar layers container.

    lst_sis_ref reference_systems; //!< Spatial reference systems.
    size_t tag_sis_ref; //!< Default identifier for next spatial reference system.

    lst_spots spots; //!< Point container.
    size_t tag_spot; //!< Default identifier for next point.
    axes_container axes; //!< Axis container.
    size_t axis_tag; //!< Default identifier for next axis.

  public:
    //Constructores
    GeomSection(MaterialHandler *ml);    
    inline virtual ~GeomSection(void) {}

    inline int getReferenceFrameTag(void) const
      { return tag_sis_ref; }
    void setReferenceFrameTag(int i)
      { tag_sis_ref= i; }
    SectionReferenceFrame *get_reference_system(const size_t &id);
    const SectionReferenceFrame *get_reference_system(const size_t &id) const;
    inline SectionReferenceFrame *current_reference_system(void)
      { return get_reference_system(tag_sis_ref); }
    const SectionReferenceFrame *current_reference_system(void) const
      { return get_reference_system(tag_sis_ref); }
    SectionReferenceFrame *createReferenceFrame(const std::string &); 

    // Section edition functions
    inline int getTagSpot(void) const
      { return tag_spot; }
    void setTagSpot(int i)
      { tag_spot= i; }
    Spot *busca_spot(const size_t &);
    const Spot *busca_spot(const size_t &) const;
    Spot *creaSpot(const Pos2d &);
    Spot *newSpot(const Pos2d &);

    inline int getAxisTag(void) const
      { return axis_tag; }
    void setAxisTag(int i)
      { axis_tag= i; }
    Axis *find_axis(const size_t &);
    const Axis *find_axis(const size_t &) const;
    template <class E>
    Axis *createAxis(void);
    Segment *newSegment(size_t, size_t);
    
    // Section inquiring functions
    double DistSpots(const size_t &i,const size_t &j) const;
    Poligono2d getRegionsContour(void) const;
    Poligono2d getCompressedZoneContour(const Semiplano2d &) const;
    double getLongCorte(const Recta2d &r) const;
    std::vector<double> getLongsCorte(const std::list<Recta2d> &lr) const;
    double getCompressedZoneDepth(const Semiplano2d &) const;
    double getTensionedZoneDepth(const Semiplano2d &) const;
    double getLeverArm(const Recta2d &) const;
    double getAnchoMecanico(const Recta2d &) const;
    double getCover(const Pos2d &) const;
    double getCompressedStrutWidth(const Segmento2d &) const;

    //Access to containers.
    inline const ListRegiones &getRegiones(void) const
      { return regiones; }
    inline ListRegiones &getRegiones(void)
      { return regiones; }
    inline const ListReinfLayer &getReinforcementLayers(void) const
      { return reinforcement_layers; }
    inline ListReinfLayer &getReinforcementLayers(void)
      { return reinforcement_layers; }
    GeomSection getGMRegiones(void) const;
    GeomSection getGMReinforcementLayers(void) const;
    GeomSection getCrackedSection(const Semiplano2d &) const;
    size_t getNumFiberData(void) const;

    double getAreaGrossSection(void) const;
    Vector getCdgGrossSection(void) const;
    double getIyGrossSection(void) const;
    double getIzGrossSection(void) const;
    double getPyzGrossSection(void) const;

    Vector getCdgHomogenizedSection(const double &E0) const;
    double getAreaHomogenizedSection(const double &E0) const;
    double getIyHomogenizedSection(const double &E0) const;
    double getIzHomogenizedSection(const double &E0) const;
    double getPyzHomogenizedSection(const double &E0) const;

    void Print(std::ostream &s, int flag =0);
    friend std::ostream &operator<<(std::ostream &s, GeomSection &fiberSectionRepr);    
  };

//! @brief Creates a new axis.
template <class E>
Axis *XC::GeomSection::createAxis(void)
  {
    Axis *retval= find_axis(axis_tag);
    if(!retval) //It doesn't already exist
      {
        retval= new E(this);
        if(retval)
          {
            retval->Nombre()= "l"+boost::lexical_cast<std::string>(axis_tag);
            axes[axis_tag]= retval;
            axis_tag++;
	  }
        else
	  std::cerr << "Can't create axis with tag: "
                    << axis_tag << ".\n";
      }
    return retval;
  }

} // end of XC namespace


#endif

