# -*- coding: utf-8 -*-
# Home made test
from __future__ import print_function

A= 7.64e-4 # Cross section area (m2)
E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
G= E/(2*(1+nu)) # Shear modulus
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define materials
elasticMembranePlateSectionTest= typical_materials.defElasticMembranePlateSection(preprocessor, "elasticMembranePlateSectionTest",E,nu,dens,h)
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
section3D= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section2D",sectionProperties)
section3D= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section3D",sectionProperties)

materialNames= modelSpace.getMaterialNames()
materialNamesRef= ['elasticMembranePlateSectionTest', 'section2D', 'section3D']
ratio= (materialNames==materialNamesRef)

# print('material names: ', materialNames)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

