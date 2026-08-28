# -*- coding: utf-8 -*-
''' Section aggregator class verification test. Check 
getElasticShearSection3d method.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import xc
import geom
from materials import typical_materials

def torsional_inertia_approx(b: float, h: float) -> float:
    '''
    Standard engineering approximation formula for a rectangle's torsion 
    constant. Accurate to within ~4% for any aspect ratio.

    :param b: rectangle side length.
    :param h: rectangle side length.
    '''
    short_dim, long_dim = (b, h) if b <= h else (h, b)
    return long_dim * (short_dim**3) * (1/3 - 0.21 * (short_dim / long_dim) * (1 - (short_dim**4) / (12 * long_dim**4)))

# Define section geometry
b= .01 # Cross section width [cm]
h= .02 # Cross section depth [cm]

# Define XC problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Materials definition
E= 2.1e9 # initial elastic tangent [N/m2].
G= E/(2*(1+0.3))
refJ= torsional_inertia_approx(b, h)
refArea= b*h
rho= 7850 # material density [kg/m^3].
alpha_y= alpha_z= 5/6.0
elasticMat= typical_materials.defElasticMaterial(preprocessor, "elasticMat",E, rho= rho)
respT= typical_materials.defElasticMaterial(preprocessor, "respT", G*refJ) # Torsion response.
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy", alpha_y*G*refArea) # Shear response in y direction.
respVz= typical_materials.defElasticMaterial(preprocessor, "respVz", alpha_z*G*refArea) # Shear response in y direction.

# Define section geometry
materialHandler= preprocessor.getMaterialHandler
sectionGeometry= materialHandler.newSectionGeometry("sectionGeometry")
regions= sectionGeometry.getRegions
elasticRegion= regions.newQuadRegion(elasticMat.name)
elasticRegion.pMin= geom.Pos2d(-b/2.0, -h/2.0)
elasticRegion.pMax= geom.Pos2d(b/2.0, h/2.0)
elasticRegion.setDiscretization(20,20)

# Create 3D fiber section.
fiberSection= sectionGeometry.getFiberSection3d("fiberSection")

# Define section aggregator
sectionAggregator= materialHandler.newMaterial("section_aggregator","sectionAggregator")
sectionAggregator.setSection(fiberSection.name)
sectionAggregator.setAdditions(["T","Vy","Vz"],["respT","respVy","respVz"])

# Get 3D elastic shear section from the aggregation.
Iw= 0.0
elasticShearSection3d= sectionAggregator.getElasticShearSection3d('elasticShearSection3d', E, Iw, G)

# Check results.
fiberSectionRho= fiberSection.linearRho
## Linear density.
refLinearRho= b*h*rho
ratio1= abs(fiberSectionRho-refLinearRho)/refLinearRho
sectionAggregatorRho= sectionAggregator.linearRho
ratio2= abs(sectionAggregatorRho-refLinearRho)/refLinearRho
elasticShearSection3dRho= elasticShearSection3d.linearRho
ratio3= abs(elasticShearSection3dRho-refLinearRho)/refLinearRho
### Mass properties.
sp= elasticShearSection3d.sectionProperties
## Area.
area= sp.A
ratio4= ((area-refArea)/refArea)
## Inertia.
Iy= sp.Iy # Second moment inertia around y axis.
refIy= b*h**3/12.0
ratio5= (abs(Iy-refIy)/refIy)
Iz= sp.Iz # Second moment inertia around y axis.
refIz= h*b**3/12.0
ratio6= (abs(Iz-refIz)/refIz)
Iyz= sp.Iyz # Product of inertia.
refIyz= 0.0
ratio7= abs(Iyz)
JJ= sp.J # Torsional inertia.
ratio8= abs(JJ-refJ)/refJ
alphaY= sp.AlphaY
ratio9= abs(alphaY-alpha_y)/alpha_y
alphaZ= sp.AlphaZ
ratio10= abs(alphaZ-alpha_z)/alpha_z

'''
print('Elastic material rho: ', elasticMat.rho)
print('Fiber section linear rho: ', fiberSectionRho)
print('ratio1= ', ratio1)
print('Section aggregation linear rho: ', sectionAggregatorRho)
print('ratio2= ', ratio2)
print('Elastic shear section 3D properties:')
print('  rho: ', elasticShearSection3dRho)
print('  ratio3= ', ratio3)
print('  area: ', area)
print('  ratio4= ', ratio4)
print('  Iy: ', Iy)
print('  ratio5= ', ratio5)
print('  Iz: ', Iz)
print('  ratio6= ', ratio6)
print('  Iyz: ', Iyz)
print('  ratio7= ', ratio7)
print('  J: ', JJ)
print('  ratio8= ', ratio8)
print('  alpha_y: ', alpha_y)
print('  ratio9= ', ratio9)
print('  alpha_z: ', alpha_z)
print('  ratio10= ', ratio10)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-12) & (abs(ratio2)<1e-12) & (abs(ratio3)<1e-12) & (abs(ratio4)<1e-12) & (abs(ratio5)<1e-2) & (abs(ratio6)<1e-2) & (abs(ratio7)<1e-12) & (abs(ratio8)<1e-12) & (abs(ratio9)<1e-12) & (abs(ratio10)<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
