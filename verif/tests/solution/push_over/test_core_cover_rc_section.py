# -*- coding: utf-8 -*-
''' Check cross-section for concrete frame pushover analysis. 

Example based on the OpenSees one:
See https://openseespydoc.readthedocs.io/en/latest/src/RCFramePushOver.html
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
import geom
from model import predefined_spaces
from postprocess.reports import common_formats as cf

# Import local modules.
import core_cover_rc_section

# Define problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   

# Some parameters
colWidth= 15
colDepth= 24
cover= 1.5
As= 0.60  # area of no. 7 bars    
columnFiberSection= core_cover_rc_section.def_core_cover_rc_section(preprocessor, colWidth= colWidth, colDepth= colDepth, cover= cover, As= As)

# Check some cross-section values computed from section fibers.
fiber_section_area= columnFiberSection.getArea()
fiber_section_reference_area= colWidth*colDepth+8*As
ratio_fs_1= abs(fiber_section_area-fiber_section_reference_area)/fiber_section_reference_area
fiber_section_yG= columnFiberSection.getCenterOfMassY()
fiber_section_zG= columnFiberSection.getCenterOfMassZ()
ratio_fs_2= math.sqrt(fiber_section_yG**2+fiber_section_zG**2)
print('XXX Continue here, compute fiber_section_Iz and fiber_section_Iy')

testOK= (ratio_fs_1<1e-6) and (ratio_fs_2<1e-6)

# Check some cross-section values computed from section regions.
sectionGeometry= columnFiberSection.getSectionGeometry
regions= sectionGeometry.getRegions

regions_area= regions.getAreaGrossSection()
regions_reference_area= colWidth*colDepth
ratio_rg_1= abs(regions_area-regions_reference_area)/regions_reference_area
regions_G= regions.getCenterOfMassGrossSection()
regions_yG= regions_G[1]
regions_zG= regions_G[0]
ratio_rg_2= math.sqrt(regions_yG**2+regions_zG**2)
regions_Iz= regions.getIzGrossSection()
regions_Iy= regions.getIyGrossSection()
regions_reference_Iz= 1/12*colWidth*colDepth**3
regions_reference_Iy= 1/12*colWidth**3*colDepth
ratio_rg_3= math.sqrt((regions_Iz-regions_reference_Iz)**2+(regions_Iy-regions_reference_Iy)**2)

testOK= testOK and (ratio_rg_1<1e-6) and (ratio_rg_2<1e-6) and (ratio_rg_3<1e-6)


# Extract concrete materials and check concrete area.
coreConcrete= None
coverConcrete= None
steel= None
coreArea= 0.0
coverArea= 0.0
steelArea= 0.0
for r in regions:
    area= r.getArea()
    if(area>100.0): # core region.
        coreConcrete= r.getMaterial()
        coreArea+= area
    else: # cover region.
        coverConcrete= r.getMaterial()
        coverArea+= area
reinforcementLayers= sectionGeometry.getReinfLayers
for reinfLayer in reinforcementLayers:
    for bar in reinfLayer.getReinfBars:
        steel= bar.getMaterial()
        steelArea+= bar.area
totalRegionsArea= coreArea+coverArea
refArea= colWidth*colDepth
testOK= abs(totalRegionsArea-refArea)<1e-6

testOK= testOK and abs(steelArea-8*As)<1e-6
# Extract fibers and check concrete and steel areas.
steelEs= steel.getTangent()
fibers= columnFiberSection.getFibers()
concreteFibersArea= 0.0
steelFibersArea= 0.0
for f in fibers:
    mat= f.getMaterial()
    E= mat.getTangent()
    if(E<0.9*steelEs): # Concrete.
        concreteFibersArea+= f.getArea()
    else:
        steelFibersArea+= f.getArea()
testOK= testOK and abs(concreteFibersArea-(colWidth*colDepth))<1e-6
testOK= testOK and abs(steelFibersArea-8*As)<1e-6


print('cover area: ', coverArea)
print('core area: ', coreArea)
print('fiber section area: ', fiber_section_area)
print('fiber section reference area: ', fiber_section_reference_area)
print('ratio_fs_1= ', ratio_fs_1)
print('fiber section yG= ', fiber_section_yG)
print('fiber section zG= ', fiber_section_zG)
print('ratio_fs_2= ', ratio_fs_2)

print('\nregions area: ', regions_area)
print('regions reference area: ', regions_reference_area)
print('ratio_rg_1= ', ratio_rg_1)
print('regions yG= ', regions_yG)
print('regions zG= ', regions_zG)
print('ratio_rg_2= ', ratio_rg_2)
print('regions Iz= ', regions_Iz)
print('regions reference Iz: ', regions_reference_Iz)
print('regions Iy= ', regions_Iy)
print('regions reference Iy: ', regions_reference_Iy)
print('ratio_rg_3= ', ratio_rg_3)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
