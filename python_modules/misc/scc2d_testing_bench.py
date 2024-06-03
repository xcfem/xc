# -*- coding: utf-8 -*-
'''Defines a model to test a 2D fiber section..'''
import xc
from model import predefined_spaces

def section_model(preprocessor,sectionName):
    ''' Defines a model to test a 2D fiber section.

    :param preprocessor: pre processor of the finite element problem.
    :param sectionName: name of the section material.
    '''
    # Define nodes.
    nodes= preprocessor.getNodeHandler

    modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
    n1= nodes.newNodeXY(1,0)
    n2= nodes.newNodeXY(1,0)

    # Define element.
    elementHandler= preprocessor.getElementHandler
    elementHandler.dimElem= 1
    elementHandler.defaultMaterial= sectionName
    zls= elementHandler.newElement("ZeroLengthSection",xc.ID([n1.tag,n2.tag]))
    return zls, n1, n2

