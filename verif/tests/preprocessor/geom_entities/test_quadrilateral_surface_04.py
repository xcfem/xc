# -*- coding: utf-8 -*-
''' Check that the elements of a surface are oriented like the
    surface itself. Home made test'''

from __future__ import division
from __future__ import print_function

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
#from postprocess import output_handler

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

feProblem.logFileName= "/tmp/erase.log" # Nicely avoid warning messages.
numDivI= 1
numDivJ= 1
L= 10
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density




matPrb= typical_materials.defElasticMembranePlateSection(preprocessor, "matPrb",E,nu,rho,0.25)

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= matPrb.name
quad4n= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

#                         3
#  4 +--------------------+--------------------+ 6
#    |                    |                    |
#    |                    |                    |
#    |                    |                    |
#  1 +--------------------+--------------------+ 5
#                         2

pt1= modelSpace.newKPoint(0,0,0)
pt2= modelSpace.newKPoint(L,0,0)
pt3= modelSpace.newKPoint(L,L,0)
pt4= modelSpace.newKPoint(0,L,0)
pt5= modelSpace.newKPoint(2*L,0,0.3*L)
pt6= modelSpace.newKPoint(2*L,L,0.3*L)

s1= modelSpace.newSurface([pt1,pt2,pt3,pt4])
s1.nDivI= numDivI; s1.nDivJ= numDivJ
s2= modelSpace.newSurface([pt2,pt5,pt6,pt3])
s2.nDivI= numDivI; s2.nDivJ= numDivJ

s1.genMesh(xc.meshDir.I)
s2.genMesh(xc.meshDir.I)

offset= 2.5*L
pt11= modelSpace.newKPoint(offset,0,0)
pt12= modelSpace.newKPoint(offset+L,0,0)
pt13= modelSpace.newKPoint(offset+L,L,0)
pt14= modelSpace.newKPoint(offset,L,0)
pt15= modelSpace.newKPoint(offset+2*L,0,L)
pt16= modelSpace.newKPoint(offset+2*L,L,L)

s11= modelSpace.newSurface([pt11,pt12,pt13,pt14])
s11.nDivI= numDivI; s11.nDivJ= numDivJ
s12= modelSpace.newSurface([pt12,pt15,pt16,pt13])
s12.nDivI= numDivI; s12.nDivJ= numDivJ

s11.genMesh(xc.meshDir.I)
s12.genMesh(xc.meshDir.I)

offset= 5*L
pt21= modelSpace.newKPoint(offset,0,0)
pt22= modelSpace.newKPoint(offset+L,0,0)
pt23= modelSpace.newKPoint(offset+L,L,0)
pt24= modelSpace.newKPoint(offset,L,0)
pt25= modelSpace.newKPoint(offset+2*L,0,1.5*L)
pt26= modelSpace.newKPoint(offset+2*L,L,1.5*L)

s21= modelSpace.newSurface([pt21,pt22,pt23,pt24])
s21.nDivI= numDivI; s21.nDivJ= numDivJ
s22= modelSpace.newSurface([pt22,pt25,pt26,pt23])
s22.nDivI= numDivI; s22.nDivJ= numDivJ

s21.genMesh(xc.meshDir.I)
s22.genMesh(xc.meshDir.I)


surfaces= [s1, s2, s11, s12, s21, s22]
error= 0.0
for s in surfaces:
    sK= s.getKVector
    elem= s.elements[0]
    eK= elem.getKVector3d(True)
    dot= sK.dot(eK)
    error+= (dot-1.0)**2

error= math.sqrt(error)

'''
print(error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(error<1e-6):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
  
# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)

#oh.displayBlocks()#setToDisplay= )
#oh.displayFEMesh()#setsToDisplay=[])
#oh.displayLocalAxes()
