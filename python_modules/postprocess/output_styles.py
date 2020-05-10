# -*- coding: utf-8 -*-

from __future__ import print_function
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import locale
from misc_utils import log_messages as lmsg
from postprocess import output_units
from postprocess.xcVtk.CAD_model import vtk_CAD_graphic

class OutputStyle(object):
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
       :ivar language: english, spanish, french 
       :ivar directionDescr: text list to identify each direction (as ['vertical reinforcement', 'horizontal reinforcement']).
    '''
    def __init__(self, cameraParameters= None, outputUnits= output_units.OutputUnits(), constraintsScaleFactor= 0.4, localAxesVectorScaleFactor= 0.5, language= None):
        '''Defines the dimension of the space and the number 
         of DOFs for each node.

        :param cameraParameters: camera position and orientation.
        :param outputUnits: output units and unit conversion.
        :param constraintsScaleFactor: scale factor to display DOF constraints.
        :param localAxesVectorScaleFactor: scale factor to display local axes vectors.
        :param language: english, spanish, french 
        '''
        # Graphic stuff.
        self.cameraParameters= cameraParameters
        self.outputUnits= outputUnits
        self.constraintsScaleFactor= constraintsScaleFactor
        self.localAxesVectorsScaleFactor= localAxesVectorScaleFactor
        self.reactionVectorsScaleFactor= 1.0
        self.eigenvectorsScaleFactor= 1.0
        self.equivalentLoadVectorsScaleFactor= 0.0
        self.internalForcesDiagramScaleFactor= 1.0
        self.loadVectorsScaleFactor= 1.0
        self.loadDiagramsScaleFactor= 1.0
        self.nodalLoadBarOrientation= 'V'
        self.showLoadsPushing= True
        self.multLoadsByElemArea=False
        if(language):
            self.language= language
        else:
            self.language= locale.getdefaultlocale()
        self.directionDescription= ['dir. 1', 'dir. 2']
            
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

    def getPressureUnitsScaleFactor(self):
        ''' Return the scale factor for the displacement units.'''
        return self.outputUnits.getPressureUnitsScaleFactor()
    
    def getPressureUnitsDescription(self):
        ''' Return the description for the displacement units.'''
        return self.outputUnits.getPressureUnitsDescription()

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
        elif(itemToDisp=='CF'):
            fConvUnits= 1.0
            unitDescription=''
        elif(itemToDisp.startswith('sigma_')):
            fConvUnits= self.getPressureUnitsScaleFactor()
            unitDescription= self.getPressureUnitsDescription()
        elif(itemToDisp.startswith('epsilon_')):
            fConvUnits= 1.0
            unitDescription= '-'
        elif(itemToDisp.startswith('chi')): # strength reduction factors
            fConvUnits= 1.0
            unitDescription= '-'
        else:
            fConvUnits= self.getForceUnitsScaleFactor()
            unitDescription= self.getForceUnitsDescription()
        return fConvUnits, unitDescription
    
    def getCaptionTextsDict(self):
        ''' Return a dictionary with the caption texts to use 
            in graphic output.'''
        retval= sp_capTexts
        lang_id= self.language[0][:2].lower()
        if lang_id=='en':
            retval= en_capTexts
        elif lang_id=='fr':
            retval= fr_capTexts
        return retval

#Spanish caption texts
sp_capTexts={
    'uX':'desplazamiento en dirección X',
    'uY':'desplazamiento en dirección Y',
    'uZ':'desplazamiento en dirección Z',
    'rotX':"rotación en torno al eje X",
    'rotY':"rotación en torno al eje Y",
    'rotZ':"rotación en torno al eje Z",
    'CF':'factor de capacidad',
    'getCF':'factor de capacidad',
    'N':'esfuerzo normal asociado al factor de capacidad',
    'N1':'esfuerzo normal dirección 1',
    'N2':'esfuerzo normal dirección 2',
    'M1':'momento flector dirección 1',
    'M2':'momento flector dirección 2',
    'Q1':'esfuerzo cortante dirección 1',
    'Q2':'esfuerzo cortante dirección 2',
    'Qy':'esfuerzo cortante dirección y',
    'Qz':'esfuerzo cortante dirección z',
    'Vy':'esfuerzo cortante dirección y',
    'Vz':'esfuerzo cortante dirección z',
    'My':'momento flector asociado al factor de capacidad',
    'Mz':'momento flector asociado al factor de capacidad',
    'Mu':'valor último del momento flector',
    'theta':'',
    'Vy':'esfuerzo cortante asociado al factor de capacidad',
    'Vz':'esfuerzo cortante asociado al factor de capacidad',
    'Vcu':'',
    'Vsu':'',
    'Vu':"valor último del esfuerzo cortante",
    'LocalAxes': 'ejes locales',
    'FEmesh': 'malla de elementos',
    'ULS_normalStressesResistance': 'Comprobación ELU tensiones normales',
    'normalStressCheck': 'Comprobación ELU tensiones normales',
    'ULS_shearResistance': 'Comprobación ELU esfuerzo cortante',
    'getMaxSteelStress': "tensión máxima en la armadura",
    'SLS_frequentLoadsCrackControl': 'Comprobación ELS fisuración, casos de carga frecuentes',
    'SLS_quasiPermanentLoadsLoadsCrackControl': 'Comprobación ELS fisuración, casos de carga quasi-permanentes',
    
  }

#English caption texts
en_capTexts={
    'CF': 'efficiency',
    'uX':'displacement in global X direction',
    'uY':'displacement in global Y direction',
    'uZ':'displacement in global Z direction',
    'rotX':"rotation about global X axis",
    'rotY':"rotation about global Y axis",
    'rotZ':"rotation about global Z axis",
    'N1':'internal axial force in local direction 1',
    'N2':'internal axial force in local direction 2',
    'M1':'bending moment about local axis 1',
    'M2':'bending moment about local axis 2',
    'Q1':'internal shear force in local direction 1',
    'Q2':'internal shear force in local direction 2',
    'N':'internal axial force',
    'Qy':'internal shear force in local direction y',
    'Qz':'internal shear force in local direction z',
    'Vy':'internal shear force in local direction y',
    'Vz':'internal shear force in local direction z',
    'My':'bending moment about local axis y',
    'Mz':'bending moment about local axis z',
    'T':'internal torsional moment',
    'FEmesh': 'FE mesh',
    'ULS_normalStressesResistance': 'ULS normal stresses check',
    'ULS_shearResistance': 'ULS shear check',
    'getMaxSteelStress': 'steel maximum stress',
    'SLS_frequentLoadsCrackControl': 'SLS cracking, frequent actions',
    'SLS_quasiPermanentLoadsLoadsCrackControl': 'SLS cracking, quasi-permanent actions',
    'LocalAxes': 'local axes',
    'wk': 'crack width',
    's_rmax': 'maximum crack distance',
    'eps_sm': 'mean steel deformation over maximum crack distance',
    'Ncrd': 'design resistance to axial compression',
    'McRdy': 'design moment resistance about Y (weak) axis',
    'McRdz': 'design moment resistance about Z (strong) axis',
    'MvRdz': 'reduced design moment resistance about Z (strong) axis for shear interaction',
    'MbRdz':'reduced design moment resistance about Z (strong) axis for lateral-torsional bucking',
    'chiLT': 'reduction factor for lateral-torsional buckling ',
    
  }

#Caption texts in French
fr_capTexts={
    'uX':'déplacement en direction X',
    'uY':'déplacement en direction Y',
    'uZ':'déplacement en direction Z',
    'rotX':"rotation autour de l'axe X",
    'rotY':"rotation autour de l'axe Y",
    'rotZ':"rotation autour de l'axe Z",
    'CF':'facteur de capacité',
    'getCF':'facteur de capacité',
    'N':'effort normal associé au facteur de capacité',
    'N1':'effort normal direction 1',
    'N2':'effort normal direction 2',
    'M1':'moment de flexion direction 1',
    'M2':'moment de flexion adirection 2',
    'Q1':'effort tranchant direction 1',
    'Q2':'effort tranchant direction 2',
    'Qy':'effort tranchant direction y',
    'Qz':'effort tranchant direction z',
    'My':'moment de flexion associé au facteur de capacité',
    'Mz':'moment de flexion associé au facteur de capacité',
    'Mu':'valeur ultime du moment de flexion',
    'theta':'',
    'Vy':'effort tranchant associé au facteur de capacité',
    'Vz':'effort tranchant associé au facteur de capacité',
    'Vcu':'',
    'Vsu':'',
    'Vu':"valeur ultime de l'effort tranchant",
    'LocalAxes': 'axes locaux',
    'FEmesh': 'maillage',
    'ULS_normalStressesResistance': 'Vérification ELU contraintes normales',
    'normalStressCheck': 'Vérification ELU contraintes normales',
    'ULS_shearResistance': 'Vérification ELU effort tranchant',
    'getMaxSteelStress': "contrainte maximale dans l'armature",
    'SLS_frequentLoadsCrackControl': 'Vérification ELS fissuration, cas de charge fréquents',
    'SLS_quasiPermanentLoadsLoadsCrackControl': 'Vérification ELS fissuration, cas de charge quasi-permanents',
    'ULS_fatigueResistance': 'Vérification ELU fatigue',
    'getAbsSteelStressIncrement': "vérification de l'armature. Différence de contrainte $\delta_{sd}(Q_{fat})$ sous les actions de fatigue",
    'concreteBendingCF':'vérification du béton. Facteur de capacité contraintes de compression',
    'concreteLimitStress':'vérification du béton. Limites contraintes de compression',
    'concreteShearCF':'vérification du béton. Facteur de capacité effort tranchant',
    'shearLimit': 'vérification du béton. Limites effort tranchant',
    
  }
