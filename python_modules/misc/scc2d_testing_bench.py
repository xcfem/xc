# -*- coding: utf-8 -*-
'''Defines a model to test a 2D fiber section..'''
import xc
from model import predefined_spaces

def sectionModel(preprocessor,sectionName):
    ''' Defines a model to test a 2D fiber section.'''
    nodes= preprocessor.getNodeHandler

    modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
    n1= nodes.newNodeXY(1,0)
    n2= nodes.newNodeXY(1,0)

    elementos= preprocessor.getElementHandler
    elementos.dimElem= 1
    elementos.defaultMaterial= sectionName
    zls= elementos.newElement("ZeroLengthSection",xc.ID([n1.tag,n2.tag]))
    return zls, n1, n2

