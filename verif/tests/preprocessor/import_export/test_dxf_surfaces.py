# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
from import_export import dxf_reader
from import_export import neutral_mesh_description as nmd
import xc_base
import geom
import xc
import logging

layerNamesToImport= ['xc_*']

def getRelativeCoo(pt):
    return [pt[0],pt[1],pt[2]] #No modification.

pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
dxfFilePath= pth+'/../../aux/dxf/knife_plate.dxf' 
logger= logging.getLogger('ezdxf')
logger.setLevel(level=logging.WARNING) #Avoid logging info messages.
dxfImport= dxf_reader.DXFImport(dxfFilePath, layerNamesToImport,getRelativeCoo, importLines= False, polylinesAsSurfaces= True, threshold= 0.001, tolerance= .001)

#Block topology
blocks= dxfImport.exportBlockTopology('test')

fileName= '/tmp/knife_plate_model_blocks'
ieData= nmd.XCImportExportData()
ieData.outputFileName= fileName
ieData.problemName= 'FEcase'
ieData.blockData= blocks

ieData.writeToXCFile()

FEcase= xc.FEProblem()
FEcase.title= 'Knife plate model'
exec(open(fileName+'.py').read())

xcTotalSet= preprocessor.getSets.getSet('total')

numSurfaces= len(xcTotalSet.getSurfaces)
avgSize= xcTotalSet.getEntities.getAverageSize()

ratio1= numSurfaces-42
ratio2= abs(avgSize-0.0384251796329)/0.0384251796329

'''
print(numSurfaces)
print(avgSize)
print(ratio2)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1==0 and ratio2<1e-11):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

