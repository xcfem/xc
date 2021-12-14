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
baseName= 'test_ifc_lines'
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
FEcase.title= 'Test IFC lines.'
exec(open(ieData.outputFileName+'.py').read())

xcTotalSet= preprocessor.getSets.getSet('total')

num_points= len(xcTotalSet.points)
ratio1= abs(num_points-8)

num_lines= len(xcTotalSet.lines)
ratio2= abs(num_lines-7)


from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1==0 and  ratio2==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
