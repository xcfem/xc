# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import numpy
from model.mesh import element_vectors
from postprocess.quick_inquiry import nodal_reactions

movN2= modelSpace.getDisplacementVector(2) # Node 2 displacement
vI= element_vectors.getVectorIElem(preprocessor, 1) # Vector I of element 1
vProj= movN2.dot(vI)
delta= abs(vProj)

nodes.calculateNodalReactions(True)
reacN1= nodal_reactions.vectorReacUVW(preprocessor, 1) # Node 1 reaction
vReacProj= reacN1.dot(vI)
RN= abs(vReacProj)

elem1= elements.getElement(1)
elem1.getResistingForce()
scc0= elem1.getSections()[0]
N0= scc0.getStressResultantComponent("N")

F= (f*L)
deltateor= (f*L**2/(2*E*A))
ratio1= (abs((delta-deltateor)/deltateor))
ratio2= (abs((N0-F)/F))
ratio3= (abs((RN-F)/F))

