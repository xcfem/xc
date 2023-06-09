# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy
from model.mesh import element_vectors
from postprocess.quick_inquiry import nodal_reactions

def check_values(modelSpace, f, E, A, L, n1, n2, el):
    movN2= modelSpace.getDisplacementVector(n2.tag) # Node 2 displacement
    vI= element_vectors.getVectorIElem(modelSpace.preprocessor, el.tag) # Vector I of element 1
    vProj= movN2.dot(vI)
    delta= abs(vProj)

    modelSpace.calculateNodalReactions(True,1e-7)
    reacN1= nodal_reactions.vectorReacUVW(modelSpace.preprocessor, n1.tag) # Node 1 reaction
    vReacProj= reacN1.dot(vI)
    RN= abs(vReacProj)

    el.getResistingForce()
    scc0= el.getSections()[0]
    N0= scc0.getStressResultantComponent("N")

    F= (f*L)
    deltateor= (f*L**2/(2*E*A))
    ratio1= (abs((delta-deltateor)/deltateor))
    ratio2= (abs((N0-F)/F))
    ratio3= (abs((RN-F)/F))
    return ratio1, ratio2, ratio3


