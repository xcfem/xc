# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import xc
from model import predefined_spaces

def section_model(preprocessor, sectionName):
    ''' Defines a model to test a 3D fiber section.

    :param preprocessor: pre processor of the finite element problem.
    :param sectionName: name of the section material.
    '''
    # Define nodes.
    nodes= preprocessor.getNodeHandler

    modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
    nodA= nodes.newNodeXYZ(1,0,0)
    nodB= nodes.newNodeXYZ(1,0,0)

    # Define element.
    elementHandler= preprocessor.getElementHandler
    elementHandler.dimElem= 1
    elementHandler.defaultMaterial= sectionName
    zls= elementHandler.newElement("ZeroLengthSection",xc.ID([nodA.tag,nodB.tag]))
    return zls, nodA, nodB


