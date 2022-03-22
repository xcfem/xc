# -*- coding: utf-8 -*-
''' Check that support points are correctly imported.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

from import_export import dxf_reader
from import_export import neutral_mesh_description as nmd

import os
import sys
import xc_base
import geom
import xc
import logging

layerNamesToImport= ['xc_*']

#inputFileName= sys.argv[1]
#outputFileName= sys.argv[2]

outputFileName= '/tmp/xc_model_blocks'

def getRelativeCoo(pt):
    return [pt[0],pt[1],pt[2]] #No modification.

# There are some misplaced diagonals threshold= 0.05 is NEEDED.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
dxfFilePath= pth+'/../../../aux/dxf/stair_tower.dxf' 
logger= logging.getLogger('ezdxf')
logger.setLevel(logging.WARNING)
dxfImport= dxf_reader.DXFImport(dxfFilePath, layerNamesToImport,getRelativeCoo, importLines= True, polylinesAsSurfaces= False, threshold= 0.05, tolerance= .001)

#Block topology
blocks= dxfImport.exportBlockTopology('test')

fileName= 'xc_model_blocks'
ieData= nmd.XCImportExportData()
ieData.outputFileName= outputFileName
ieData.problemName= 'FEcase'
ieData.blockData= blocks

ieData.writeToXCFile()

FEcase= xc.FEProblem()
FEcase.title= 'Stair tower structure'
exec(open(outputFileName+'.py').read())

# Import block topology information
supportSet= preprocessor.getSets.defSet('supportSet')
stairSupportsSet= preprocessor.getSets.defSet('stairSupportsSet')

setsFromLabels= {'xc_supports':supportSet, 'xc_supports_stairs':stairSupportsSet}

xcTotalSet= preprocessor.getSets.getSet('total')

## Supports
for p in xcTotalSet.getPoints:
    if(p.hasProp('labels')):
        labels= p.getProp('labels')
        if(labels.count('xc_supports')>0):
           supportSet.getPoints.append(p)
        if(labels.count('xc_supports_stairs')>0):
            stairSupportsSet.getPoints.append(p)

nSupports= len(supportSet.getPoints)
nStairsSupports= len(stairSupportsSet.getPoints)

ratio1= abs(nSupports-4)
ratio2= abs(nStairsSupports-2)

'''
print('nSupports= ', nSupports)
print('nStairsSupports= ', nStairsSupports)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0) & (ratio2==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
