# -*- coding: utf-8 -*-
''' Block topology displaying routines using VTK.'''

from __future__ import print_function

import sys
from vtk.vtkCommonCore import (
    vtkIdList,
    vtkPoints
    )
from vtk.vtkFiltersSources import vtkSphereSource
from vtk.vtkFiltersCore import (
    vtkGlyph3D,
    vtkIdFilter
)
from vtk.vtkFiltersCore import vtkGlyph3D
from vtk.vtkRenderingCore import (
    vtkActor,
    vtkActor2D,
    vtkPolyDataMapper,
    vtkDataSetMapper,
    vtkSelectVisiblePoints
    )
from vtk.vtkRenderingLabel import vtkLabeledDataMapper
from vtk.vtkFiltersGeneral import vtkCellCenters
import xc_base
from misc_utils import log_messages as lmsg
from postprocess.xcVtk.CAD_model import create_array_set_data
import random as rd 

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es  ana.Ortega@ciccp.es"


def vtk_define_kpoint_actor(recordGrid, renderer, radius):
    '''Returns a vtkActor to represent key-points in a rendering scene.
    It defines the scale, orientation, rendering properties, textures, ...

    :ivar recordGrid: unstructured grid (generic data set) to which incorporate
                      the actor KPoint
    :ivar renderer:   name of the renderer (lights, views, ...) to be used in 
                      the display
    :ivar radius:     radius of the spheres to be employed in the KPoints 
                      representation
    '''
    sphereSource= vtkSphereSource()
    sphereSource.SetRadius(radius)
    sphereSource.SetThetaResolution(5)
    sphereSource.SetPhiResolution(5)

    markKPts= vtkGlyph3D()
    markKPts.SetInputData(recordGrid.uGrid) 
    markKPts.SetSourceData(sphereSource.GetOutput())
    markKPts.ScalingOff()
    markKPts.OrientOff()

    mappKPts= vtkPolyDataMapper()
    mappKPts.SetInputData(markKPts.GetOutput())

    visKPts= vtkActor()
    visKPts.SetMapper(mappKPts)
    visKPts.GetProperty().SetColor(.7, .5, .5)

    renderer.AddActor(visKPts)

def vtk_define_cells_actor(gridRecord, renderer, reprType, color= [rd.random(),rd.random(),rd.random()]):
    ''' Actor for lines and surfaces.

    :param gridRecord: 
    :param rendered: VTK renderer.
    :param reprType: representation type (points, wireframe or surface).
    '''
    uGridMapper= vtkDataSetMapper()
    uGridMapper.SetInputData(gridRecord.uGrid)
    cellActor= vtkActor()
    cellActor.SetMapper(uGridMapper)
    cellActor.GetProperty().SetColor(color[0],color[1],color[2])
    if(reprType=="points"):
        cellActor.GetProperty().SetRepresentationToPoints()
    elif(reprType== "wireframe"):
        cellActor.GetProperty().SetRepresentationToWireframe()
    elif(reprType== "surface"):
        cellActor.GetProperty().SetRepresentationToSurface()
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(methodName+"; error: "+reprType+" not implemented.")
    renderer.AddActor(cellActor) # Actor para las celdas

def VtkCargaIdsKPts(uGrid, setToDraw):
    etiqKPt=  create_array_set_data.VtkCreaStrArraySetData(setToDraw,'pnts','name')
    uGrid.GetPointData().AddArray(etiqKPt)

def VtkCargaIdsCells(uGrid, setToDraw, entTypeName):
    etiqCells= create_array_set_data.VtkCreaStrArraySetData(setToDraw,entTypeName,'name')
    uGrid.GetCellData().AddArray(etiqCells)

def VtkDibujaIdsKPts(uGrid, setToDraw, renderer):
    '''Draw the point labels.'''
    numKPtsDI= setToDraw.getPoints.size
    if(numKPtsDI>0):
        ids= vtkIdFilter()
        ids.SetInput(uGrid)
        ids.CellIdsOff()
        ids.PointIdsOff()

        VtkCargaIdsKPts(uGrid,setToDraw)

        visPts= vtkSelectVisiblePoints()
        visPts.SetInputConnection(ids.GetOutputPort())
        visPts.SetRenderer(renderer)
        visPts.SelectionWindowOff()

        #Create the mapper to display the point ids.  Specify the format to
        #  use for the labels.  Also create the associated actor.
        ldm= vtkLabeledDataMapper()
        ldm.SetInputConnection(visPts.GetOutputPort())
        ldm.GetLabelTextProperty().SetColor(0.1,0.1,0.1)

        pointLabels= vtkActor2D()
        pointLabels.SetMapper(ldm)

        renderer.AddActor2D(pointLabels)
    else:
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+"; no kPoints in set : '"+setToDraw.name+"'.")

def VtkDibujaIdsCells(uGrid, setToDraw, entTypeName, renderer):
    ''' Create cell labels.'''
    ids= vtkIdFilter()
    ids.SetInput(uGrid)
    ids.CellIdsOff()
    ids.PointIdsOff()

    VtkCargaIdsCells(uGrid,setToDraw,entTypeName)

    # Dibuja las etiquetas de las líneas.
    cc= vtkCellCenters()
    cc.SetInputConnection(ids.GetOutputPort()) #  Centroides de las celdas. 

    visCells= vtkSelectVisiblePoints()
    visCells.SetInputConnection(cc.GetOutputPort())
    visCells.SetRenderer(renderer)
    visCells.SelectionWindowOff()

    #Create the mapper to display the cell ids.  Specify the format to
    # use for the labels.  Also create the associated actor.
    cellMapper= vtkLabeledDataMapper()
    cellMapper.SetInputConnection(visCells.GetOutputPort())
    cellMapper.GetLabelTextProperty().SetColor(0,0,0.9)

    cellLabels= vtkActor2D()
    cellLabels.SetMapper(cellMapper)

    renderer.AddActor2D(cellLabels)

def vtk_load_mesh_data(gridRecord):
    '''Creates an VTK unstructured grid from the block topology entities.

    :param gridRecord: instance of the RecordDefGrid class.
    '''
    def dump_cells(cellSet):
        '''Dumps the cells in the given set into the VTK unstructured grid to
           be represented later.

        '''
        for c in cellSet:
            vertices= xc_base.vector_int_to_py_list(c.getIdxVertices)
            vtx= vtkIdList()
            for vIndex in vertices:
                vtx.InsertNextId(vIndex)
            gridRecord.uGrid.InsertNextCell(c.getVtkCellType,vtx)        
    kpoints= vtkPoints()
    # Grid definition
    gridRecord.uGrid.SetPoints(kpoints)
    setToDraw= gridRecord.xcSet
    setToDraw.numerate()
    pnts= setToDraw.getPoints
    kpoints.SetDataTypeToDouble()
    nPoints= len(pnts)
    kpoints.SetNumberOfPoints(nPoints)
    for p in pnts:
        kpoints.InsertPoint(p.getIdx,p.getPos.x,p.getPos.y,p.getPos.z)
    if('lines' in gridRecord.cellTypes):
        dump_cells(cellSet= setToDraw.getLines)
    if('faces' in gridRecord.cellTypes):
        dump_cells(cellSet= setToDraw.getSurfaces)
    if('bodies' in gridRecord.cellTypes):
        dump_cells(cellSet= setToDraw.getBodies)




