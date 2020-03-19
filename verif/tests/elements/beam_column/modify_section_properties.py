# -*- coding: utf-8 -*-
# Home made test
'''Modification of the section properties of an element (area, inertia,...)'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
# Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0.0)
n2= nodes.newNodeXY(1,0.0)

lin= modelSpace.newLinearCrdTransf("lin")
# Materials
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= 7.64e-4; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= 8.49e-8; 
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin"
elements.defaultMaterial= "section"
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))

secProp= beam2d.sectionProperties
secProp.A*=10.0
secProp.I*=100.0
beam2d.sectionProperties= secProp

ratio1= abs(beam2d.sectionProperties.A/sectionProperties.A-10.0)/10.0
ratio2= abs(beam2d.sectionProperties.I/sectionProperties.I-100.0)/100.0

'''
print(ratio1)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-10) and (ratio2<1e-10)):
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')
