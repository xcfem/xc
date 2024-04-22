# -*- coding: utf-8 -*-
''' Verifies that stiffness properties are returned from the correct element
    of the stiffness matrix. Section type: FiberSectionShear2d. 
    Home made test.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import xc
from materials import typical_materials
from materials.sections import section_properties

def compare_values(material, rectSection, xcSection):
    ''' Compare the stiffness values of both sections.

    :param material: section material.
    :param rectSection: rectangular section object.
    :param xcSection: XC FiberSectionGJ object.
    '''
    retval= 0.0
    EA1= material.E*rectSection.A()
    EA2= xcSection.EA()
    retval+= (EA2-EA1)**2/EA1

    EIz1= material.E*rectSection.Iz()
    EIz2= xcSection.EIz()
    retval+= (EIz2-EIz1)**2/EIz1**2

    GAy1= material.E/(2*(1+0.3))*rectSection.Ay()
    GAy2= xcSection.GAy()
    retval+= (GAy1-GAy2)**2/GAy1**2


    retval= math.sqrt(retval)
    return retval
    

# Define problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define section geometry
b= 0.4
h= 0.8
nDiv= 100
y0= 0
z0= 0

# Define material.
fy= 275e6 # Yield stress of the steel.
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",21538461538.461544) # Shear response in y direction.

# Define section.
quadFibersGeom= preprocessor.getMaterialHandler.newSectionGeometry("quadFibersGeom")
y1= h/2.0
z1= b/2.0
regions= quadFibersGeom.getRegions
steelRegion= regions.newQuadRegion(steel.name)
steelRegion.nDivIJ= 1 # 2D discretization
steelRegion.nDivJK= nDiv # number of cells in JK direction
steelRegion.pMin= geom.Pos2d(z0-z1,y0-y1)
steelRegion.pMax= geom.Pos2d(z0+z1,y0+y1)
quadFibers= preprocessor.getMaterialHandler.newMaterial("fiber_section_shear_2d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(quadFibersGeom.name)
quadFibers.setupFibers()
quadFibers.setRespVyByName(respVy.name) # Set shear response in y direction.

rectangularSection= section_properties.RectangularSection(name= 'test', b= b, h= h)

err= compare_values(material= steel, rectSection= rectangularSection, xcSection= quadFibers)

# print('err= ', err)
# print(quadFibers.getTangentStiffness())


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

