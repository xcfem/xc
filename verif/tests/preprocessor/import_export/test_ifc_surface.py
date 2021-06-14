# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

from import_export import freecad_reader
from import_export import neutral_mesh_description as nmd
import os
import xc_base
import geom
import xc

def getRelativeCoo(pt):
    return [pt[0]/1000.0,pt[1]/1000.0,pt[2]/1000.0]

groupsToImport= ['IFC.*']

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
filePath= pth+'/../../aux/freecad' 
baseName= 'test_ifc_surface'
freeCADFileName= baseName+'.FCStd'
freeCADImport= freecad_reader.FreeCADImport(filePath+'/'+freeCADFileName, groupsToImport, getRelativeCoo, threshold= 0.001)

#Block topology
blocks= freeCADImport.exportBlockTopology('test')

ieData= nmd.XCImportExportData()
ieData.outputFileName= '/tmp/'+baseName+'_blocks'
ieData.problemName= 'FEcase'
ieData.blockData= blocks

ieData.writeToXCFile()

FEcase= xc.FEProblem()
FEcase.title= 'Test IFC points.'
exec(open(ieData.outputFileName+'.py').read())

xcTotalSet= preprocessor.getSets.getSet('total')

numPoints= len(xcTotalSet.points)
ratio1= abs(numPoints-4)

face= xcTotalSet.surfaces[0]
thk= face.getProp('thickness')
ratio2= (thk-0.2)/0.2

material= face.getProp('attributes')['matId']
matOk= (material=='C25')

'''
print(numPoints)
print(ratio1)
print(thk)
print(ratio2)
print(material)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1==0 and matOk and ratio2==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
