# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
from model import predefined_spaces
from model import fix_node_6dof

def sectionModel(preprocessor,nmbS):
  ''' Defines a model to test a fiber section.'''
  nodes= preprocessor.getNodeLoader

  modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
  nodes.defaultTag= 1 #El número del próximo nodo será 1.
  nodA= nodes.newNodeXYZ(1,0,0)
  nodB= nodes.newNodeXYZ(1,0,0)

  elementos= preprocessor.getElementLoader
  elementos.dimElem= 1
  elementos.defaultMaterial= nmbS
  elementos.defaultTag= 1 #Tag for the next element.
  zls= elementos.newElement("zero_length_section",xc.ID([nodA.tag,nodB.tag]));
  return zls


