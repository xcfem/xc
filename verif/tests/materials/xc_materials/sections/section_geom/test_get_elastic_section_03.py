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
sys.path.append(pth+"/../../../../aux/sections/")
import pier_section_geometry as sg
pierSectionGeometry= sg.define_pier_section_geometry(preprocessor= preprocessor,
                                                     name= "P32_01",
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
# Reference values.
E0= concrete.getEcm() # Reference elastic modulus.
nu= 0.2 # Poisson's ratio
G= E0/(2*(1+nu)) # Shear modulus.
psA= pierSectionGeometry.getAreaHomogenizedSection(E0)
psEA= E0*psA
Iw= 0.0 # Warping constant (assumed zero).
alpha_y= 5/6.0 # Factor for shear response along local y axis.
alpha_z= 0.5*alpha_y # Factor for shear response along local z axis
                     # (0.5 only to make it different from the previous one,
                     # so no physical meaning)).
Iy= pierSectionGeometry.getIyHomogenizedSection(E0)
Iz= pierSectionGeometry.getIzHomogenizedSection(E0)
J= 3.141592 # Just a number (no physical meaning).
majorI= max(Iy, Iz)
minorI= min(Iy, Iz)

# Create some elastic sections from the section geometry.
es1d= pierSectionGeometry.getElasticSection1d('elasticSection1d', E0, Iw)
# 2D elastic section (inertia taken from the strong axis).
es2dStrong= pierSectionGeometry.getElasticSection2d('es2dStrong', E0, Iw, True)
# 2D elastic section (inertia taken from the weak axis).
es2dWeak= pierSectionGeometry.getElasticSection2d('es2dWeak', E0, Iw, False)
# 2D shear elastic section (inertia taken from the strong axis).
es2dStrongVy= pierSectionGeometry.getElasticShearSection2d('es2dStrongVy', E0, Iw, G, 5/6.0, True)
# 2D shear elastic section (inertia taken from the weak axis).
es2dWeakVy= pierSectionGeometry.getElasticShearSection2d('es2dWeakVy', E0, Iw, G, alpha_y, False)
# 3D elastic section.
es3d= pierSectionGeometry.getElasticSection3d('es3d', E0, Iw, G, J)
# 3D elastic shear section.
es3dVyVz= pierSectionGeometry.getElasticShearSection3d('es3dVyVz', E0, Iw, G, J, alpha_y, alpha_z)

elasticSections= [es1d, es2dStrong, es2dWeak, es2dStrongVy, es2dWeakVy, es3d, es3dVyVz]

# Check area results.
aError= 0.0
for es in elasticSections:
    esA= es.sectionProperties.A
    aError+= (esA-psA)**2
aError= math.sqrt(aError)

# Check Iz inertia results.
refIz= [majorI, minorI, majorI, minorI, majorI, majorI]
izSections= [es2dStrong, es2dWeak, es2dStrongVy, es2dWeakVy, es3d, es3dVyVz]
izError= 0.0
for es, ref_i in zip(izSections, refIz):
    if(ref_i is not None):
        esI= es.sectionProperties.I
        izError+= (esI-ref_i)**2
izError= math.sqrt(izError)

# Check Iy inertia results.
refIy= [minorI, minorI]
iySections= [es3d, es3dVyVz]
iyError= 0.0
for es, ref_i in zip(iySections, refIy):
    if(ref_i is not None):
        esI= es.sectionProperties.Iy
        iyError+= (esI-ref_i)**2
iyError= math.sqrt(iyError)

# Check J results.
refJ= [J, J]
jSections= iySections
jError= 0.0
for es, ref_i in zip(jSections, refJ):
    if(ref_i is not None):
        esI= es.sectionProperties.J
        jError+= (esI-ref_i)**2
jError= math.sqrt(jError)

# Check Vy results.
ref_alpha_y= [0.0, 0.0]+2*[alpha_y]+[0.0, alpha_y]
alpha_y_sections= [es2dStrong, es2dWeak, es2dStrongVy, es2dWeakVy, es3d, es3dVyVz]
alpha_y_error= 0.0
for es, ref_alpha_y in zip(alpha_y_sections, ref_alpha_y):
    if(ref_alpha_y is not None):
        es_alpha_y= es.sectionProperties.Alpha
        alpha_y_error+= (es_alpha_y-ref_alpha_y)**2
alpha_y_error= math.sqrt(alpha_y_error)

# Check Vz results.
ref_alpha_z= [0.0, alpha_z]
alpha_z_sections= [es3d, es3dVyVz]
alpha_z_error= 0.0
for es, ref_alpha_z in zip(alpha_z_sections, ref_alpha_z):
    if(ref_alpha_z is not None):
        es_alpha_z= es.sectionProperties.AlphaZ
        alpha_z_error+= (es_alpha_z-ref_alpha_z)**2
alpha_z_error= math.sqrt(alpha_z_error)

'''
print('Error on areas: ', aError)
print('Error on Iz: ', izError)
print('Error on Iy: ', iyError)
print('Error on J: ', jError)
print('Error on alpha_y_error: ', alpha_y_error)
print('Error on alpha_z_error: ', alpha_z_error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(aError)<1e-10 and abs(izError)<1e-10 and abs(iyError)<1e-10 and abs(jError)<1e-10 and abs(alpha_y_error)<1e-10 and abs(alpha_z_error)<1e-10:
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
