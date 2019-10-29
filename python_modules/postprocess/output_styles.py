# -*- coding: utf-8 -*-

from __future__ import print_function
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

from miscUtils import LogMessages as lmsg
from postprocess import output_units
from postprocess.xcVtk.CAD_model import vtk_CAD_graphic

class OutputStyle:
    ''' Pararameters used in the output routines (graphics, etc.)

       :ivar cameraParameters: camera position and orientation.
       :ivar outputUnits: output units and unit conversion.
       :ivar constraintsScaleFactor: scale factor to display DOF constraints.
       :ivar localAxesVectorScaleFactor: scale factor to display local axes vectors.
       :ivar reactionVectorScaleFactor: scale factor to display reactions.
       :ivar eigenvectorScaleFactor: scale factor to display eigenvectors.
       :ivar equivalentLoadVectorsScaleFactor: factor to apply to the vectors 
                length in the representation of equivalent loads on nodes. If 
                vectorEqLoadScale equals 0, equivalent static loads are not 
                represented.
                (defaults to 0.0 in which case only deformed shape is depicted)
       :ivar loadVectorsScaleFactor: factor to apply to the vectors length in the 
                                    representation of element loads (defaults to 1).
       :ivar loadDiagramsScaleFactor:  scale factor to apply to the diagram display 
                                       of element loads (defaults to 1).
       :ivar showLoadsPushing: true if the vector ends in the loaded node (push
                               the node).
       :iver nodalLoadBarOrientation: orientation of scale bar for nodal loads
                                      'H' -> horizontal, 'V' -> vertical
       :ivar multByElemAreaLoads: boolean value that must be True if we want to 
                        represent the total load on each element 
                        (=load multiplied by element area) and False if we 
                        are going to depict the value of the uniform load 
                        per unit area (defaults to False)
    '''
    def __init__(self, cameraParameters= None, outputUnits= output_units.OutputUnits(), scaleConstr= 0.2, localAxesVectorScale= 0.5):
        '''Defines the dimension of the space and the number 
         of DOFs for each node.

        :param cameraParameters: camera position and orientation.
        :param outputUnits: output units and unit conversion.
        '''
        # Graphic stuff.
        self.cameraParameters= cameraParameters
        self.outputUnits= outputUnits
        self.constraintsScaleFactor= scaleConstr
        self.localAxesVectorsScaleFactor= localAxesVectorScale
        self.reactionVectorsScaleFactor= 1.0
        self.eigenvectorsScaleFactor= 1.0
        self.equivalentLoadVectorsScaleFactor= 0.0
        self.internalForcesDiagramScaleFactor= 1.0
        self.loadVectorsScaleFactor= 1.0
        self.loadDiagramsScaleFactor= 1.0
        self.nodalLoadBarOrientation= 'V'
        self.showLoadsPushing= True
        self.multLoadsByElemArea=False
   
    def getDisplacementUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getDisplacementUnitsScaleFactor()
    
    def getDisplacementUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.outputUnits.getDisplacementUnitsDescription()
    
    def getRotationUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getRotationUnitsScaleFactor()
    
    def getRotationUnitsDescription(self):
        ''' Return the descrioption for the displacement units.'''
        return self.outputUnits.getRotationUnitsDescription()

    def getForceUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getForceUnitsScaleFactor()
    
    def getForceUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.outputUnits.getForceUnitsDescription()

    def getUnitParameters(self, itemToDisp):
        ''' Return the pair (scale factor, description) for
            the argument parameter.

        :param itemToDisp: item to display (uX, uY, N, Mz,...).
        '''
        if(itemToDisp=='uX' or itemToDisp=='uY' or itemToDisp=='uZ'):
            fConvUnits= self.getDisplacementUnitsScaleFactor()
            unitDescription= self.getDisplacementUnitsDescription()
        elif(itemToDisp=='rotX' or itemToDisp=='rotY' or itemToDisp=='rotZ'):
            fConvUnits= self.getRotationUnitsScaleFactor()
            unitDescription= self.getRotationUnitsDescription()
        else:
            fConvUnits= self.getForceUnitsScaleFactor()
            unitDescription= self.getForceUnitsDescription()
        return fConvUnits, unitDescription

