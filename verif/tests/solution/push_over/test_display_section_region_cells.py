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
from materials.sections.fiber_section import plot_fiber_section as pfs
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

# Import local modules.
import core_cover_rc_section

# Define problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   

# Some parameters
colWidth= 15
colDepth= 24
cover= 1.5
As= 0.60  # area of no. 7 bars    
columnFiberSection= core_cover_rc_section.def_core_cover_rc_section(preprocessor, colWidth= colWidth, colDepth= colDepth, cover= cover, As= As)

sectionGeometry= columnFiberSection.getSectionGeometry
colors= mcolors.TABLEAU_COLORS
pfs.append_region_cells_to_plot(plt, sectionGeometry, colors)

plt.show()
