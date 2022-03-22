# -*- coding: utf-8 -*-
''' Import problem geometry from FreeCAD.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from import_export import freecad_reader
from import_export import neutral_mesh_description as nmd
from model import predefined_spaces
from materials import typical_materials
import os
import xc_base
import geom
import xc

# Input data
def getRelativeCoo(pt):
    return [pt[0]/1000.0,pt[1]/1000.0,pt[2]/1000.0]

groupsToImport= ['.*Wall.*']

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
filePath= pth+'/../../../aux/freecad' 
baseName= 'test_ifc_surface_02'
freeCADFileName= baseName+'.FCStd'

# Finite element problem.
FEcase= xc.FEProblem()
FEcase.title= 'Test IFC points.'
preprocessor= FEcase.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Import block topology from FreeCAD.
#FEcase.errFileName= '/tmp/err.txt'
#FEcase.logFileName= '/tmp/log.txt'
FEcase.outputFileName= '/tmp/out.txt' # Avoid message about topology version.
modelSpace.importFromFreeCAD(fileName= filePath+'/'+freeCADFileName, groupsToImport= groupsToImport, getRelativeCoo= getRelativeCoo, threshold= 0.001)
xc_base.resetStandardOutput()


xcTotalSet= modelSpace.getTotalSet()

# Define material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, "mat",E=2.1e9,nu=0.3,rho= 7850,h= 0.015)

# Generate mesh

## Define template element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= mat.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

xcTotalSet.useGmsh= True
#meshSet.setVerbosityLevel(10) # Avoid warning about set element tag
xcTotalSet.genMesh(xc.meshDir.I)

area= 0.0
for e in xcTotalSet.elements:
    area+= e.getArea(True)
refArea= 19.772912078409664
ratio1= abs(area-refArea)/refArea

'''
print(area)
print(ratio1)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks()
# oh.displayFEMesh()

