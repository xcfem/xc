# -*- coding: utf-8 -*-

import xc

def nodal_load_on_point(preprocessor, load_pattern, id_point, load):
  '''create a point load on the node associated with a point'''
  nodeTag= preprocessor.getMultiBlockTopology.getPoints.get(id_point).getTagNode
  load_pattern.newNodalLoad(nodeTag,xc.Vector([load[0],load[1],load[2],load[3],load[4],load[5]]))


def nodal_load_on_lstNodes(load_pattern, nodeTags, load):
  '''create a point load on each node in a list of node-tags'''
  for i in nodeTags:
    load_pattern.newNodalLoad(i,xc.Vector([load[0],load[1],load[2],load[3],load[4],load[5]]))

def nodal_load_on_lstPoints(preprocessor, load_pattern,lst_points, load):
  '''create a point load on each node associated with a point in a list of point-tags'''
  for i in lst_points:
    nodal_load_on_point(preprocessor,load_pattern,i,load)


def load_on_nodes_in_line(setLinea, load_pattern, load):
  '''create a point load on each node belonging to a line'''
  tags= setLinea.getNodeLayers.getLayer(0).getTagsInteriorNodes()
  for i in tags:
    load_pattern.newNodalLoad(i,load)

def load_on_nodes_in_face(face, load_pattern, load):
  '''create a point load on each node belonging to a face'''
  capa= face.getNodeLayers.getLayer(0)
  tagsIntNodes= capa.getTagsInteriorNodes()
  for i in tagsIntNodes:
    load_pattern.newNodalLoad(i,xc.Vector(load))

