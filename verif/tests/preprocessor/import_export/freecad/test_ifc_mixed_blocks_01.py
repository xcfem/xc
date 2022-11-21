# -*- coding: utf-8 -*-
''' Import problem geometry from FreeCAD.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2022, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from misc_utils import log_messages as lmsg
from import_export import freecad_reader
from import_export import neutral_mesh_description as nmd
from model import predefined_spaces
import os
import xc_base
import xc

# Input data
def getRelativeCoo(pt):
    return [pt[0]/1000.0,pt[1]/1000.0,pt[2]/1000.0]

groupsToImport= ['ifc.*']

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
filePath= pth+'/../../../aux/freecad' 
baseName= 'test_ifc_mixed_blocks_01'
freeCADFileName= baseName+'.FCStd'
freeCADImport= freecad_reader.FreeCADImport(filePath+'/'+freeCADFileName, groupsToImport, getRelativeCoo, threshold= 0.001)

#Block topology
lineCount= 0
faceCount= 0
blockData= freeCADImport.exportBlockTopology('test')
for key in blockData.blocks:
    value= blockData.blocks[key]
    tp= value.getType()
    if (tp == 'line'):
        lineCount+= 1
    if (tp == 'face'):
        faceCount+= 1

ratio1= abs(lineCount-3)
ratio2= abs(faceCount-9)

'''
print(lineCount)
print(ratio1)
print(faceCount)
print(ratio2)
'''

fname= os.path.basename(__file__)
if(ratio1==0 and ratio2==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
