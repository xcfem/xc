# -*- coding: utf-8 -*-
''' Display objects from the geom module (polygons, polylines, etc.). '''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2025, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

from vtk.vtkCommonCore import vtkPoints
from vtk.vtkFiltersSources import vtkArrowSource
from vtk.vtkFiltersCore import vtkGlyph3D

from vtk.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData,
    vtkPolyLine,
    vtkHexahedron,
    vtkUnstructuredGrid
)
from vtk.vtkRenderingCore import (
    vtkActor,
    vtkDataSetMapper,
    vtkPolyDataMapper,
)

from vtk.vtkCommonCore import (
    vtkDoubleArray
)

def get_vtk_polyline_actor(pline, color):
    ''' Return a VTK actor from the given polyline.

    :param pline: polyline to build the actor from.
    :param color: color of the polyline.
    '''
    # Create a vtkPoints object and store the points in it
    points= vtkPoints()
    vertices= pline.getVertices()
    for pt in vertices:
        points.InsertNextPoint([pt.x, pt.y, pt.z])

    sz= len(vertices)

    # Build vtk polyline object.
    polyLine= vtkPolyLine()
    polyLine.GetPointIds().SetNumberOfIds(sz)
    for i in range(0, sz):
        polyLine.GetPointIds().SetId(i, i)
    
    # Create a cell array to store the lines in and add the lines to it
    cells= vtkCellArray()
    cells.InsertNextCell(polyLine)
    
    # Create a polydata to store everything in
    polyData= vtkPolyData()

    # Add the points to the dataset
    polyData.SetPoints(points)

    # Add the lines to the dataset
    polyData.SetLines(cells)

    # Setup actor and mapper
    mapper= vtkPolyDataMapper()
    mapper.SetInputData(polyData)

    retval= vtkActor()
    retval.SetMapper(mapper)
    retval.GetProperty().SetColor(color)
    return retval

def get_vtk_vector_field_actor(positions, vectors, scaleFactor= 1.0):
    ''' Return a VTK actor from the given vector field.

    :param positions: positions of the vectors.
    :param vectors: vectors to display.
    :param scaleFactor: overall (multiplicative) scaling factor.
    '''
    points= vtkPoints()
    for pos in positions:
        points.InsertNextPoint([pos.x, pos.y, pos.z])
    
    # Create a polydata to store everything in
    polyData= vtkPolyData()
    # Add the points to the dataset
    polyData.SetPoints(points)
    
    # Vectors at the points.
    vtkVectors= vtkDoubleArray()
    vtkVectors.SetName('Vectors')
    vtkVectors.SetNumberOfComponents(3)
    for v in vectors:
        vtkVectors.InsertNextTuple3(v.x, v.y, v.z)
    polyData.GetPointData().AddArray(vtkVectors)

    # Lengths of the vectors.
    sz= len(vectors)
    lengths= vtkDoubleArray()
    lengths.SetName('Length')
    lengths.SetNumberOfValues(sz);
    for i, length in enumerate(vectors):
        lengths.SetValue(i, v.getModulus())
    polyData.GetPointData().AddArray(lengths)

    # Generate the arrow for the glyphs
    arrow = vtkArrowSource()
    #arrow.SetRadius(0.1)
    #arrow.SetHeight(0.5)
    
    # Set up the glyph filter
    glyph = vtkGlyph3D()
    glyph.SetInputData(polyData)
    glyph.SetSourceConnection(arrow.GetOutputPort())
    glyph.ScalingOn()
    glyph.SetScaleModeToScaleByScalar()
    glyph.SetVectorModeToUseVector()
    glyph.OrientOn()
    # Set the overall (multiplicative) scaling factor
    glyph.SetScaleFactor(scaleFactor)

 
    # Tell glyph which attribute arrays to use for what
    glyph.SetInputArrayToProcess(0,0,0,0,'Length') # scalars
    glyph.SetInputArrayToProcess(1,0,0,0,'Vectors') # vectors
    # glyph.SetInputArrayToProcess(2,0,0,0,'nothing') # normals
    # glyph.SetInputArrayToProcess(3,0,0,0,'Length') # colors
    
    # Calling update because I'm going to use the scalar range to set the color map range
    glyph.Update()

    coloring_by = 'Length'
    mapper = vtkPolyDataMapper()
    mapper.SetInputConnection(glyph.GetOutputPort())
    mapper.SetScalarModeToUsePointFieldData()
    mapper.SetColorModeToMapScalars()
    mapper.ScalarVisibilityOn()
    mapper.SetScalarRange(glyph.GetOutputDataObject(0).GetPointData().GetArray(coloring_by).GetRange())
    mapper.SelectColorArray(coloring_by)
    actor = vtkActor()
    actor.SetMapper(mapper)
    return actor

def get_vtk_cube_actor(hexahedron, color):
    ''' Return a VTK actor from the given hexahedron.

    :param pline: hexahedron to build the actor from.
    :param color: color of the hexahedron.
    '''
    # For the hexahedron setup the coordinates of eight points.
    # The two faces must be in counter clockwise order as viewed from the
    # outside.
    vertexPoints= hexahedron.getVertices()
    #  
    #       4 +---------+ 7     
    #        /|        /|       
    #       / |       / |       
    #    5 +---------+6 |       
    #      |  |      |  |       
    #      |0 +------|--+ 3     
    #      | /       | /
    #      |/        |/
    #    1 +---------+ 2
    pointCoordinates = list()
    for i in range(0,8):
        v= vertexPoints[i]
        pointCoordinates.append([v.x, v.y, v.z])

    # Create the points.
    points = vtkPoints()
    # Create a hexahedron from the points.
    hexahedron = vtkHexahedron()

    for i in range(0, len(pointCoordinates)):
        points.InsertNextPoint(pointCoordinates[i])
        hexahedron.GetPointIds().SetId(i, i)

    # Add the hexahedron to a cell array.
    hexs = vtkCellArray()
    hexs.InsertNextCell(hexahedron)
    
    # Add the points and hexahedron to an unstructured grid.
    uGrid = vtkUnstructuredGrid()
    uGrid.SetPoints(points)
    uGrid.InsertNextCell(hexahedron.GetCellType(), hexahedron.GetPointIds())

    # Visualize.
    mapper = vtkDataSetMapper()
    mapper.SetInputData(uGrid)

    actor = vtkActor()
    actor.GetProperty().SetColor(color)
    actor.SetMapper(mapper)

    return actor
