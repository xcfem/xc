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
offset= diagonalLength/2e2
fontsize= 5
regions= sectionGeometry.getRegions

import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
colors= mcolors.TABLEAU_COLORS
for color, region in zip(colors, regions):
    xi= list()
    yi= list()
    regionCells= region.getCells()
    regionMaterialName= region.getMaterial().name
    for cell in regionCells:
        centroid= cell.getCentroidPosition()
        x= centroid[0]
        y= centroid[1]
        xi.append(x)
        yi.append(y)
        area= cell.getArea()
        matName= regionMaterialName
        plt.annotate(matName, (x, y + offset), fontsize= fontsize)
        plt.annotate(cf.Area.format(area), (x, y - offset), fontsize= fontsize)
    plt.plot(xi, yi, 'o', color= color)
plt.axis('equal')
plt.show()
