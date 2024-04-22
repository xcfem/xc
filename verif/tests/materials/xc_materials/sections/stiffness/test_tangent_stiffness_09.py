# -*- coding: utf-8 -*-
''' Verifies that stiffness properties are returned from the correct element
    of the stiffness matrix. Section type: FiberSectionGJ. 
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

    EIy1= material.E*rectSection.Iy()
    EIy2= xcSection.EIy()
    retval+= (EIy2-EIy1)**2/EIy2**2

    EIzy1= 0.0
    EIzy2= xcSection.EIyz()
    retval+= (EIzy2-EIzy1)**2

    GJ1= material.E/(2*(1+0.3))*rectSection.J()
    GJ2= xcSection.GJ()
    retval+= (GJ2-GJ1)**2
    
    retval= math.sqrt(retval)
    return retval
    

# Define problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define section geometry
b= 0.4
h= 0.8
nDivIJ= 100
nDivJK= 100
y0= 0
z0= 0
rectangularSection= section_properties.RectangularSection(name= 'test', b= b, h= h)

# Define material.
fy= 275e6 # Yield stress of the steel.
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

# Define section.
quadFibersGeom= preprocessor.getMaterialHandler.newSectionGeometry("quadFibersGeom")
y1= h/2.0
z1= b/2.0
regions= quadFibersGeom.getRegions
steelRegion= regions.newQuadRegion(steel.name)
steelRegion.nDivIJ= nDivIJ
steelRegion.nDivJK= nDivJK
steelRegion.pMin= geom.Pos2d(y0-y1,z0-z1)
steelRegion.pMax= geom.Pos2d(y0+y1,z0+z1)
quadFibers= preprocessor.getMaterialHandler.newMaterial("fiber_section_GJ","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed(quadFibersGeom.name)
quadFibers.setupFibers()
quadFibers.setGJ(947003076.9230773)
fibers= quadFibers.getFibers()
A= fibers.getArea

err= compare_values(material= steel, rectSection= rectangularSection, xcSection= quadFibers)

#print('err= ', err)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-3):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

