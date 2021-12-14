//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//BspTree3d.cc

#include "BspTree3d.h"
#include "utility/geom/pos_vec/Pos3d.h"


void Split_Polygon(const BspTree3d::polygon &pg,const Plane &particion,
                   BspTree3d::polygons_list &front_pieces,BspTree3d::polygons_list &back_pieces)
  {
    BspTree3d::polygons_list inter= corta(pg,particion);

    for(BspTree3d::polygons_list::const_iterator i= inter.begin();i!=inter.end();i++)
      {
        if(particion.classifyPolygon(*i) == Plane::AHEAD)
          front_pieces.push_back(*i);
        else
          back_pieces.push_back(*i);
      }
  }

void Build_BSP_Tree(BspTree3d *tree,const BspTree3d::BspTree3d::polygons_list &polygons)
  {
    typedef BspTree3d::BspTree3d::polygons_list::const_iterator const_pol_iterator;

    const_pol_iterator pol_iter= polygons.begin();
    const_pol_iterator root = pol_iter; pol_iter++;
    tree->particion = root->getPlane();
    tree->polygons.push_back(*root);
    BspTree3d::BspTree3d::polygons_list frontlist,backlist;

    for(;pol_iter!=polygons.end();pol_iter++)
      {
	Plane::polygon_classification result= tree->particion.classifyPolygon(*pol_iter);
        switch(result)
          {
	   case Plane::INSIDE:
              tree->polygons.push_back(*pol_iter);
              break;
           case Plane::BEHIND:
              backlist.push_back(*pol_iter);
              break;
           case Plane::AHEAD:
              frontlist.push_back(*pol_iter);
              break;
           case Plane::CROSSES:
              BspTree3d::polygons_list front_pieces,back_pieces;
              Split_Polygon(*pol_iter,tree->particion,front_pieces,back_pieces);
              backlist.insert(backlist.end(),back_pieces.begin(),back_pieces.end());
              frontlist.insert(frontlist.end(),front_pieces.begin(),front_pieces.end());
              break;
          }
      }
    if(!frontlist.empty())
      {
        tree->front = new BspTree3d;
        Build_BSP_Tree(tree->front, frontlist);
      }
    if(!backlist.empty())
      {
        tree->back = new BspTree3d;
        Build_BSP_Tree(tree->back, backlist);
      }
  }
