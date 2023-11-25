# -*- coding: utf-8 -*-
''' Modification of the section properties (area, inertia,...) of an elastic section. 
    Home made test.
'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2023, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import xc
from materials import typical_materials

# Material properties
E= 2.1e6*9.81/1e-4 # Elastic modulus (Pa)
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define section.
# preprocessor: preprocessor of the finite element problem.
# name:         name identifying the section
# A:            cross-sectional area of the section
# E:            Young’s modulus of the material
# G:            Shear modulus of the material          
# I:            second moment of area about the local z-axis
# alpha:        shear shape factor
section= typical_materials.defElasticShearSection2d(preprocessor, name= "section", A= 7.64e-4, E= E, G= G, I= 8.49e-8,alpha= 5/6.0)
A0= section.sectionProperties.A

# Duplicate section area.
section.sectionProperties.A= 2*section.sectionProperties.A

EA= section.sectionProperties.EA()
ratio= abs(EA/A0/E-2)/2

# print(EA, ratio)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
