# -*- coding: utf-8 -*-
''' Check access to section fibers.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
import geom
from model import predefined_spaces
from postprocess.reports import common_formats as cf

# Import local modules.
import rc_column_fiber_section

# Define problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   

As= 0.60  # area of no. 7 bars    
columnFiberSection= rc_column_fiber_section.def_rc_column_fiber_section(preprocessor, As= As)

sectionGeometry= columnFiberSection.getSectionGeometry
diagonalLength= sectionGeometry.getBnd().diagonal.getModulus()
offset= diagonalLength/1e2
fontsize= 6
regions= sectionGeometry.getRegions

import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
colors= mcolors.TABLEAU_COLORS
materials= regions.getMaterials()
materialColor= dict()
for material, color in zip(materials,colors):
    materialColor[material.name]= color

materialPositions= dict()
for region in regions:
    regionMaterialName= region.getMaterial().name
    xi= list()
    yi= list()
    areas= list()
    regionCells= region.getCells()
    for cell in regionCells:
        centroid= cell.getCentroidPosition()
        x= centroid[0]
        y= centroid[1]
        xi.append(x)
        yi.append(y)
        areas.append(cell.getArea())
    if(regionMaterialName in materialPositions):
        materialPositions[regionMaterialName]['xi'].extend(xi)
        materialPositions[regionMaterialName]['yi'].extend(yi)
        materialPositions[regionMaterialName]['ai'].extend(areas)
    else:
        materialPositions[regionMaterialName]= {'xi':xi, 'yi':yi, 'ai': areas}

for regionMaterialName in materialPositions:
    color= materialColor[regionMaterialName]
    xi= materialPositions[regionMaterialName]['xi']
    yi= materialPositions[regionMaterialName]['yi']
    ai= materialPositions[regionMaterialName]['ai']
    for x, y, area in zip(xi, yi, ai):
        plt.annotate(cf.Area.format(area), (x, y - offset), fontsize= fontsize)
    plt.plot(xi, yi, 'o', color= color, label= regionMaterialName)
plt.legend()
plt.axis('equal')
plt.show()
