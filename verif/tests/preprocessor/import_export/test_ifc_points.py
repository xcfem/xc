# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

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
baseName= 'test_ifc_points'
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

error= False
for p in xcTotalSet.points:
    attributes= p.getProp('attributes')
    pos= p.getPos
    ifc_type= attributes['IfcType']
    if(ifc_type!='Structural Point Connection'):
        error= True

bnd= xcTotalSet.points.getBnd()
pMax= bnd.pMax
pMin= bnd.pMin
ratio2= pMax.dist(geom.Pos3d(1,1,0))+pMin.dist(geom.Pos3d(0,0,0))

'''
print(numPoints)
print(error)
print(bnd)
print(centroid)
print(ratio2)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1==0 and (not error) and ratio2<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
