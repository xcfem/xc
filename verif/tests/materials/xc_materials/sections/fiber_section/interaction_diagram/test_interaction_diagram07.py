# -*- coding: utf-8 -*-
''' Build an irregular pier section and check the values of its mass properties.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import sys
import math
import geom
import xc
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import geom_fiber_sect as gfs
from misc_utils import log_messages as lmsg

concrete= EC2_materials.C40
steel= EC2_materials.S500B

# Section geometry data.
thickness= .35

p9= geom.Pos2d(215e-2, 140e-2)
p9i= p9+geom.Vector2d(0.0, -thickness)
p10= geom.Pos2d(-p9.x, p9.y)
p10i= p10+geom.Vector2d(0.0, -thickness)
p8= p9+geom.Vector2d(.1, .1)
p7= geom.Pos2d(300e-2, 75e-2)
p6= geom.Pos2d(p7.x, -p7.y)
p5i= geom.Pos2d(265e-2, 60.5e-2)
p4i= geom.Pos2d(p5i.x, -p5i.y)
p6i= geom.Pos2d(220.5e-2, 105e-2)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

tagConcreteDiag= concrete.defDiagD(preprocessor)

# Import section definition function from file.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
sys.path.append(pth+"/../../../../../aux/sections/")
import pier_section_geometry as sg
sectionName= "P32_01"
pierSectionGeometry= sg.define_pier_section_geometry(preprocessor= preprocessor,
                                                     name= sectionName,
                                                     concrete= concrete,
                                                     steel= steel,
                                                     p4i= p4i,
                                                     p5i= p5i, p6= p6,
                                                     p6i= p6i, p7= p7,
                                                     p8= p8, p9= p9, p9i= p9i,
                                                     p10= p10, p10i= p10i,
                                                     r1Reinf= (30, 25e-3),
                                                     r2Reinf= (8, 25e-3),
                                                     r3Reinf= (9, 25e-3),
                                                     r9Reinf= (31, 16e-3),
                                                     r10Reinf= (4, 16e-3),
                                                     r11Reinf= (10, 16e-3))

# Create fiber section.
matHandler= preprocessor.getMaterialHandler
pierFiberSection= gfs.create_fiber_section(materialHandler= matHandler, sectionGeometry= pierSectionGeometry, fiberSectionType= "fiber_section_3d")

# Compute interaction diagram
feProblem.errFileName= "/tmp/erase.err" # Don't print(errors.)
diagInt= gfs.compute_interaction_diagram(materialHandler= matHandler, fiberSection= pierFiberSection)
feProblem.errFileName= "cerr" # Display errors if any.

sys.path.append(pth+"/../../../../../aux/internal_forces/")
import uls_internal_forces_on_pier
eluLoads= uls_internal_forces_on_pier.ulsLoads[sectionName]

capacityFactors= list()
difference= 0.0
for eluNMyMz in eluLoads:
    N= eluNMyMz[2]*1e3
    My= eluNMyMz[3]*1e3
    Mz= eluNMyMz[4]*1e3
    CF= diagInt.getCapacityFactor(geom.Pos3d(N, My, Mz))
    difference+= (CF-eluNMyMz[-1])**2
    capacityFactors.append(CF)
difference= math.sqrt(difference)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if difference<0.1:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section as pfs
# fig = plt.figure()
# ax= fig.add_subplot(111)
# pfs.mplot_section_geometry(ax, sectionGeometry= pierSectionGeometry, rotate= False)
# plt.show()
