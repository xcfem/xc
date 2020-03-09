# -*- coding: utf-8 -*-
''' Verification test of a steel rectangular section modelized with fibers with
elasto-plastic material.
   written from  «Nociones de cálculo plástico de vigas rectas y 
   pórticos simples». Carlos Benito Hernández.
   ({https://books.google.ch/books?id=v7bbMwEACAAJ})
   page 26 ff. '''

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
from materials.sections import section_properties

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages

# Rectangular cross-section definition
b= 10 # Cross section width  [cm]
h= 20 # Cross section depth [cm]
scc10x20= section_properties.RectangularSection('scc10x20',b,h)
scc10x20.nDivIJ= 32 # number of cells in IJ direction  
scc10x20.nDivJK= 32 # number of cells in JK direction

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/fiber_section_test_macros.py")

fy= 2600 # Yield stress of the material expressed in kp/cm2.
E= 2.1e6 # Young modulus of the material en kp/cm2.

# Materials definition:
epp= typical_materials.defElasticPPMaterial(preprocessor, "epp",E,fy,-fy)

# Section geometry
# creation
geomRectang= preprocessor.getMaterialHandler.newSectionGeometry("geomRectang")
#generation of a quadrilateral region of the scc10x20 sizes and number of
#divisions made of material nmbMat
reg= scc10x20.getRegion(gm=geomRectang,nmbMat="epp")
rectang= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","rectang")
fiberSectionRepr= rectang.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomRectang")
rectang.setupFibers()
fibers= rectang.getFibers()


extractFiberSectionProperties(rectang,scc10x20)
curvM= 0.005
rectang.setTrialSectionDeformation(xc.Vector([0.0,curvM,0.0]))
rectang.commitState()
Mp1= rectang.getStressResultantComponent("Mz")
rectang.revertToStart()

curvM= 0.008
rectang.setTrialSectionDeformation(xc.Vector([0.0,0.0,curvM]))
rectang.commitState()
Mp2= rectang.getStressResultantComponent("My")

referenceCenterOfMassY= 0.0
referenceCenterOfMassZ= 0.0
ratio1= ((sumAreas-scc10x20.A())/scc10x20.A())
ratio2= (centerOfMassY-referenceCenterOfMassY)
ratio3= (centerOfMassZ-referenceCenterOfMassZ)
ratio4= ((I1-scc10x20.Iz())/scc10x20.Iz())
ratio5= ((I2-scc10x20.Iy())/scc10x20.Iy())
ratio6= (i1-scc10x20.iz())/scc10x20.iz()
ratio7= (i2-scc10x20.iy())/scc10x20.iy()
ratio8= (Me1-scc10x20.getYieldMomentZ(fy))/scc10x20.getYieldMomentZ(fy)
ratio9= (Me2-scc10x20.getYieldMomentY(fy))/scc10x20.getYieldMomentY(fy)
ratio10= (SzPosG-scc10x20.getPlasticSectionModulusZ())/scc10x20.getPlasticSectionModulusZ()
ratio11= (SyPosG-scc10x20.getPlasticSectionModulusY())/scc10x20.getPlasticSectionModulusY()
ratio12= ((scc10x20.getPlasticMomentZ(fy)-Mp1)/scc10x20.getPlasticMomentZ(fy))
ratio13= (scc10x20.getPlasticMomentY(fy)-Mp2)/scc10x20.getPlasticMomentY(fy)

'''
print 'ratio1= ', ratio1
print 'ratio2= ', ratio2
print 'ratio3= ', ratio3
print 'ratio4= ', ratio4
print 'ratio6= ', ratio6
print 'ratio7= ', ratio7
print 'ratio10= ', ratio10
print 'ratio11= ', ratio11
print 'ratio12= ', ratio12
print 'ratio13= ', ratio13
'''

from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-3) & (abs(ratio5)<1e-2) & (abs(ratio6)<1e-3) & (abs(ratio7)<1e-2) & (abs(ratio8)<1e-3) & (abs(ratio9)<1e-3) & (abs(ratio10)<1e-5) & (abs(ratio11)<1e-5) & (abs(ratio12)<1e-5) & (abs(ratio13)<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
