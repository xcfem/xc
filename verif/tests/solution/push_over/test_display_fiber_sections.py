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

# Import local modules.
import rc_column_fiber_section

# Define problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   

As= 0.60  # area of no. 7 bars    
columnFiberSection= rc_column_fiber_section.def_rc_column_fiber_section(preprocessor, As= As)

sectionGeometry= columnFiberSection.getSectionGeometry
regions= sectionGeometry.getRegions

regionCellCenters= list()
for region in regions:
    cells= region.getCells()
    tmp= list()
    for i, cell in enumerate(cells):
        tmp.append(cell.getCentroidPosition())
    regionCellCenters.append(tmp)


import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
colors= mcolors.TABLEAU_COLORS
for color, cellCenters in zip(colors, regionCellCenters):
    xi= list()
    yi= list()
    for pt in cellCenters:
        xi.append(pt[0])
        yi.append(pt[1])
    plt.plot(xi, yi, 'ro')
plt.show()
