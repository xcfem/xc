# -*- coding: utf-8 -*-
import xc_base
import geom
import xc
from model import predefined_spaces

def sectionModel(preprocessor,sectionName):
  ''' Defines a model to test a 3D fiber section.'''
  nodes= preprocessor.getNodeLoader

  modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
  nodes.defaultTag= 1 #El número del próximo nodo será 1.
  nodA= nodes.newNodeXYZ(1,0,0)
  nodB= nodes.newNodeXYZ(1,0,0)

  elementos= preprocessor.getElementLoader
  elementos.dimElem= 1
  elementos.defaultMaterial= sectionName
  elementos.defaultTag= 1 #Tag for the next element.
  zls= elementos.newElement("ZeroLengthSection",xc.ID([nodA.tag,nodB.tag]));
  return zls


