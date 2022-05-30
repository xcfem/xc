# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import geom
import xc
from model import predefined_spaces

def sectionModel(preprocessor,sectionName):
    ''' Defines a model to test a 3D fiber section.'''
    nodes= preprocessor.getNodeHandler

    modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
    nodA= nodes.newNodeXYZ(1,0,0)
    nodB= nodes.newNodeXYZ(1,0,0)

    elementos= preprocessor.getElementHandler
    elementos.dimElem= 1
    elementos.defaultMaterial= sectionName
    zls= elementos.newElement("ZeroLengthSection",xc.ID([nodA.tag,nodB.tag]))
    return zls, nodA, nodB


