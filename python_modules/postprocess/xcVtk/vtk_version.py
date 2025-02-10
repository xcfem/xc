# -*- coding: utf-8 -*-
''' Get VTK version. '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2025, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from vtk.vtkCommonCore import vtkVersion

ver= vtkVersion()
vtk_major= ver.GetVTKMajorVersion()
vtk_minor= ver.GetVTKMinorVersion()
vtk_build= ver.GetVTKBuildVersion()
