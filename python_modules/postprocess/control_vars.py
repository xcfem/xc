# -*- coding: utf-8 -*-
'''Classes to store limit state control variables (internal forces, 
   strains, stresses,...) calculated in the analysis.
   THESE CLASSES ARE INTENDED TO REPLACE THE PROPERTIES DEFINED
   IN def_vars_control.py WHICH MUST DISSAPEAR.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2016,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"


import os
import sys
import json
import scipy
from misc_utils import log_messages as lmsg
from postprocess.reports import common_formats as fmt
from postprocess import extrapolate_elem_attr as ext
from materials import stresses

__all__= ['AxialForceControlVars', 'BiaxialBendingControlVars', 'BiaxialBendingStrengthControlVars', 'CFN', 'CFNMy', 'CFNMyMz', 'CFVy', 'ControlVarsBase', 'CrackControlBaseVars', 'CrackControlVars', 'FatigueControlBaseVars', 'FatigueControlVars', 'N', 'NMy', 'NMyMz', 'RCCrackControlVars', 'RCCrackStraightControlVars', 'RCShearControlVars', 'SIATypeRCShearControlVars', 'ShVy', 'ShearYControlVars', 'SteelShapeUniaxialBendingControlVars', 'SteelShapeBiaxialBendingControlVars', 'UniaxialBendingControlVars', 'VonMisesControlVars', 'RCBucklingControlVars', 'SteelBucklingControlVars', 'StrutAndTieControlVars', 'extrapolate_control_var', 'getControlVarImportModuleStr', 'get_diagram_direction', 'get_element_internal_force_component_data', 'write_control_vars_from_elements', 'write_control_vars_from_elements_for_ansys', 'write_control_vars_from_phantom_elements']

def get_diagram_direction(elem, component, defaultDirection):
    '''Return the direction vector to represent the diagram over the element

    :param elem: element to deal with.
    :param component: component to show. 
           Valid components are: 'N', 'Qy', 'Vy', 'Qz', 'Vz', 'My', 'Mz', 
           'T'
    :param defaultDirection: default direction of the diagram (J: element local
                             j vector or K: element local K vector).
    '''
    if(defaultDirection=='J'): # local J vector.
        elemVDir= elem.getJVector3d(True) #initialGeometry= True
    else: # local K vector.
        elemVDir= elem.getKVector3d(True) #initialGeometry= True
    if((component == 'Qy') or (component == 'Vy')):
        elemVDir= elem.getJVector3d(True) # initialGeometry= True 
    elif((component == 'Qz') or (component == 'Vz')):
        elemVDir= elem.getKVector3d(True) # initialGeometry= True 
    elif(component == 'My'):
        elemVDir= elem.getKVector3d(True) # initialGeometry= True 
    elif(component == 'Mz'):
        elemVDir= -elem.getJVector3d(True) # initialGeometry= True 
    return elemVDir
    

def get_element_internal_force_component_data(elem, component, defaultDirection):
    '''Return the data to use to represent the diagram over the element

    :param elem: element to deal with.
    :param component: component to show. 
           Valid components are: 'N', 'Qy', 'Vy', 'Qz', 'Vz', 'My', 'Mz', 
           'T'
    :param defaultDirection: default direction of the diagram (J: element local
                             j vector or K: element local K vector). If None
                             ignore the direction information of the diagram.
    '''
    # default values
    elemVDir= None
    if(defaultDirection):
        elemVDir= get_diagram_direction(elem, component, defaultDirection)
    value1= 0.0
    value2= 0.0
    values= elem.getValuesAtNodes(component, False)
    if(len(values)>1): # component found.
        value1= values[0]; value2= values[1]
    return [elemVDir,value1,value2]

#        ___         _           _        
#       / __|___ _ _| |_ _ _ ___| |       
#      | (__/ _ \ ' \  _| '_/ _ \ |       
#       \___\___/_||_\__|_| \___/_|       
#      __ ____ _ _ _(_)__ _| |__| |___ ___
#      \ V / _` | '_| / _` | '_ \ / -_|_-<
#       \_/\__,_|_| |_\__,_|_.__/_\___/__/
# Classes defining control variables.

class ControlVarsBase(object):
    '''Base class for the control of variables (internal forces, 
     strains, stresses,...) calculated in the analysis.

    :ivar combName: name of the load combination to deal with
    '''
    def __init__(self, combName= 'nil'):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        '''
        self.combName= combName #Name of the corresponding load combination

    def getCF(self):
        ''' Return the capacity factor.'''
        return -1.0

    def getKnownArguments(self):
        ''' Return a list of the arguments known by this object.'''
        tmp= self.getDict()
        return list(tmp.keys())

    def __call__(self,arguments):
        ''' Allow the object behave like a funcion.

        :param arguments: function arguments.
        '''
        retval= None
        obj= self
        args= arguments.split('.')
        for arg in args:
            if(hasattr(obj,arg)):
                obj= getattr(obj,arg)
                if(hasattr(obj,'__call__')):
                    obj= obj.__call__()
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                msg= className+'.'+methodName+'; argument: '+ arg+' not found.'
                msg+= ' Candidates are:'+str(self.getKnownArguments())
                lmsg.error(msg)
                exit(1)
        retval= obj
        return retval
    
    def getFieldNames(self,parent= ''):
        ''' Return the object field names.

        :param parent: parent object name.
        '''
        retval= list()
        for key in self.__dict__:
            obj= getattr(self,key)
            try:
                retval.extend(obj.getFieldNames(key+'.'))
            except AttributeError:
                retval.append(parent+key)
        return retval

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        dct= self.getDict()
        retval= ''
        for key in dct:
            value= dct[key]
            initial= key[0]
            if(initial in ['N', 'M','V']): # internal force
                value= fmt.Esf.format(float(value)*factor)
            if('Stress' in key):
                value= fmt.Stress.format(value*factor*factor)
            if('stress' in key):
                value= fmt.Stress.format(value*factor*factor)
            retval+= str(value)+' & '
        retval= retval.strip(' &')
        return retval

    def getLaTeXString(self,eTag,factor= 1e-3):
        ''' Returns a string that we can insert in a LaTeX table.

        :param eTag:   element identifier.
        :param factor: factor for units (default 1e-3 -> kN)'''
        return str(eTag)+" & "+self.getLaTeXFields(factor)+" & "+fmt.Esf.format(self.getCF())+"\\\\\n"

    def getAnsysStrings(self,eTag, axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag:   element identifier.
        :param axis:   section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= []
        retval.append("detab,"+str(eTag)+",CF" +axis+","+str(self.getCF())+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        return {'combName': self.combName, 'CF':self.getCF()}

    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.combName= dct['combName']
        self.CF= dct['CF']
       
    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.

        '''
        dct= self.getDict()
        retval= ''
        for key in dct:
            value= dct[key]
            if(isinstance(value, str)):
                value= '"'+str(value)+'"'
            else:
                value= str(value)
            retval+= str(key)+'= '+value+','
        retval= retval.strip(',')
        return retval

    def getModuleImportString(self):
        ''' Return the string to import the module where the control 
            var is defined.'''
        retval= 'from '+ self.__module__
        retval+= ' import ' + self.__class__.__name__
        return retval
        
    def getStrConstructor(self):
        ''' Return a python sentence that can construct a clone
            of this object: kin of naïve serialization.'''
        retval= self.__class__.__name__ 
        retval+= '(' + self.getStrArguments() + ')'
        return retval

    def strElementProp(self,eTag,nmbProp):
        '''Writes a string that will serve to read the element property from a file.

        :param eTag:    element identifier.
        :param nmbProp: name of the element property
        '''
        retval= 'preprocessor.getElementHandler.getElement('
        retval+= str(eTag)
        retval+= ").setProp("
        retval+= '"' + nmbProp + '"'
        retval+= ',' + self.getStrConstructor()
        retval+= ")\n"
        return retval
  
class N(ControlVarsBase):
    '''Uniaxial tension or compression. Internal force [N] for a combination.

    :ivar N:        axial force (defaults to 0.0)
    '''
    def __init__(self, combName= 'nil', NN= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param NN:        axial force (defaults to 0.0)
        '''
        super(N,self).__init__(combName)
        self.N= NN # Axial force.

    def getAnsysStrings(self, eTag, axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)
        '''
        retval= super(N,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",N" +axis+","+str(self.N*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(N,self).getDict()
        retval.update({'N':self.N})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.'''
        super(N,self).setFromDict(dct)
        self.N= dct['N']
  
class NMy(N):
    '''Uniaxial bending. Internal forces [N,My] for a combination.

    :ivar My:       bending moment about Y axis (defaults to 0.0)
    '''
    def __init__(self,combName= 'nil',N= 0.0,My= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        '''
        super(NMy,self).__init__(combName, N)
        self.My= My # Bending moment about y axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= super(NMy,self).getAnsysStrings(eTag,axis,factor)
      retval.append("detab,"+str(eTag)+",My" +axis+","+str(self.My*factor)+"\n")
      return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(NMy,self).getDict()
        retval.update({'My':self.My})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(NMy,self).setFromDict(dct)
        self.My= dct['My']

class NMyMz(NMy):
    '''Biaxial bending. Internal forces [N,My,Mz] for a combination.

    :ivar Mz:       bending moment about Z axis (defaults to 0.0)
    '''
    def __init__(self,combName= 'nil',N= 0.0,My= 0.0, Mz= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        '''
        super(NMyMz,self).__init__(combName,N,My)
        self.Mz= Mz #Bending moment about z axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= super(NMyMz,self).getAnsysStrings(eTag,axis,factor)
      retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
      return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(NMyMz,self).getDict()
        retval.update({'Mz':self.Mz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.'''
        super(NMyMz,self).setFromDict(dct)
        self.Mz= dct['Mz']

class CFN(N):
    '''Uniaxial tension or compression. Normal stresses limit state variables.

    :ivar CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param N:        axial force (defaults to 0.0)
        '''
        super(CFN,self).__init__(combName,N)
        self.CF= CF # Capacity factor or efficiency

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

class CFNMy(NMy):
    '''Uniaxial bending. Normal stresses limit state variables.

    :ivar CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0,My= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        '''
        super(CFNMy,self).__init__(combName,N,My)
        self.CF= CF # Capacity factor or efficiency

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

class ShVy(ControlVarsBase):
    '''Shear along Y axis. Internal forces [Vy] for a combination.

    :ivar combName: name of the load combinations to deal with
    :ivar Vy:       shear along Y axis (defaults to 0.0)
    '''
    def __init__(self, combName= 'nil', Vy= 0.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param Vy:       shear along Y axis (defaults to 0.0)
        '''
        super(ShVy,self).__init__(combName)
        self.Vy= Vy #Shear along y axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(ShVy,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(ShVy,self).getDict()
        retval.update({'Vy':self.Vy})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(ShVy,self).setFromDict(dct)
        self.Vy= dct['Vy']

class CFVy(ShVy):
    '''Uniaxial bending. Shear stresses limit state variables.

    :ivar CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil',CF= -1.0,Vy= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param Vy:       shear along Y axis (defaults to 0.0)
        '''
        super(CFVy,self).__init__(combName,Vy)
        self.CF= CF # Capacity factor or efficiency

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

class ShearYControlVars(CFVy):
    '''Shear along Y axis. Limit state variables [CF,Vy].

    :ivar idSection: section identifier
    '''
    def __init__(self, idSection= 'nil',combName= 'nil',CF= -1.0,Vy= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param Vy:       shear along Y axis (defaults to 0.0)
        '''
        super(ShearYControlVars,self).__init__(combName,CF,Vy)
        self.idSection= idSection # Section identifier.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(ShearYControlVars,self).getDict()
        retval.update({'idSection':self.idSection})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(ShearYControlVars,self).setFromDict(dct)
        self.idSection= dct['idSection']

class UniaxialBendingControlVars(CFNMy):
    '''Uniaxial bending. Normal stresses limit state variables [CF,N,M].

    :ivar idSection: section identifier.
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0, My= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with.
        :param CF: capacity factor (efficiency) (defaults to -1; CF<1.0 -> Ok; CF>1.0 -> KO).
        :param N: axial force (defaults to 0.0).
        :param My: bending moment about Y axis (defaults to 0.0).
        '''
        super(UniaxialBendingControlVars,self).__init__(combName,CF,N, My)
        self.idSection= idSection # Section identifier.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(UniaxialBendingControlVars,self).getDict()
        retval.update({'idSection':self.idSection})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the data values.
        '''
        super(UniaxialBendingControlVars,self).setFromDict(dct)
        self.idSection= dct['idSection']

class CFNMyMz(CFNMy):
    '''Biaxial bending. Normal stresses limit state variables. [CF,N,My,Mz].

    :ivar Mz:       bending moment about Z axis (defaults to 0.0)
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        '''
        super(CFNMyMz,self).__init__(combName,CF,N,My)
        self.Mz= Mz #Bending moment about z axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CFNMyMz,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(CFNMyMz,self).getDict()
        retval.update({'Mz':self.Mz})
        return retval
       
    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(CFNMyMz,self).setFromDict(dct)
        self.Mz= dct['Mz']

class AxialForceControlVars(CFN):
    '''Axial force. Internal forces [N] for a combination.

    :ivar idSection: section identifier
    '''
    def __init__(self,idSection= 'nil',combName= 'nil', N= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param N:        axial force (defaults to 0.0)
        '''
        super(AxialForceControlVars,self).__init__(combName= combName, N= N)
        self.idSection= idSection # Section identifier.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= 'idSection= "' + self.idSection +'", ' 
      retval+= super(AxialForceControlVars,self).getAnsysStrings(eTag,axis,factor)
      return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(AxialForceControlVars,self).getDict()
        retval.update({'idSection':self.idSection})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(AxialForceControlVars,self).setFromDict(dct)
        self.idSection= dct['idSection']



#       ___              _ _           
#      | _ ) ___ _ _  __| (_)_ _  __ _ 
#      | _ \/ -_) ' \/ _` | | ' \/ _` |
#      |___/\___|_||_\__,_|_|_||_\__, |
#                                |___/ 
# Bending control vars.
        
class BiaxialBendingControlVars(UniaxialBendingControlVars):
    '''Biaxial bending. Normal stresses limit state variables. [CF,N,My,Mz].

    :ivar Mz:       bending moment about Z axis (defaults to 0.0)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        '''
        super(BiaxialBendingControlVars,self).__init__(idSection,combName,CF,N,My)
        self.Mz= Mz #Bending moment about z axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(BiaxialBendingControlVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(BiaxialBendingControlVars,self).getDict()
        retval.update({'Mz':self.Mz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BiaxialBendingControlVars,self).setFromDict(dct)
        self.Mz= dct['Mz']

class BiaxialBucklingControlVars(BiaxialBendingControlVars):
    '''Control variables for buckling limit state verification 
       verification.

    :ivar Leff: effective length.
    :ivar mechLambda: mechanical slenderness.
    :ivar efY: additional eccentricity for buckling around Y axis.
    :ivar efZ: additional eccentricity for buckling around Z axis.
    :ivar mode: buckling mode corresponding to the previous parameters.
    :ivar alpha_cr: factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)).
    :ivar strongAxisBucklingPercent: dot product (projection) of the buckling eigenvector over the strong axis.
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0, Leff= -1.0, mechLambda= -1.0, efY= 0.0, efZ= 0.0, mode= None, alpha_cr= None, strongAxisBucklingPercent= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Leff: effective length.
        :param mechLambda: mechanical slenderness.
        :param efY: additional eccentricity around Y axis.
        :param efZ: additional eccentricity around Z axis.
        :param mode: buckling mode that corresponds to the previous parameters.
        :param alpha_cr: factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)).
        :param strongAxisBucklingPercent: dot product (projection) of the buckling eigenvector over the strong axis.
        '''
        super(BiaxialBucklingControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, My= My, Mz= Mz)
        self.setBucklingParameters(Leff= Leff, mechLambda= mechLambda, efZ= efZ, efY= efY, mode= mode, alpha_cr= alpha_cr, strongAxisBucklingPercent= strongAxisBucklingPercent)

    def setBucklingParameters(self, Leff, mechLambda, efZ, efY, mode, alpha_cr, strongAxisBucklingPercent):
        ''' Assigns values to the buckling parameters.

        :param Leff: effective length.
        :param mechLambda: mechanical slenderness.
        :param efZ: additional eccentricity around Z axis.
        :param efY: additional eccentricity around Y axis.
        :param mode: buckling mode that corresponds to the previous parameters.
        :param alpha_cr: factor by which the design loading would have to be increased to cause elastic instability (see Eurocode 4:2004 cl.5.2.1(2)).
        :param strongAxisBucklingPercent: dot product (projection) of the buckling eigenvector over the strong axis.
        '''
        self.Leff= Leff # effective length.
        self.mechLambda= mechLambda # mechanical slenderness.
        self.efY= efY # additional eccentricity around Y axis.
        self.efZ= efZ # additional eccentricity around Z axis.
        self.mode= mode # buckling mode that corresponds to the previous parameters.
        self.alpha_cr= alpha_cr
        self.strongAxisBucklingPercent= strongAxisBucklingPercent # dot product (projection) of the buckling eigenvector over the strong axis.
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(BiaxialBucklingControlVars, self).getDict()
        retval.update({'Leff': self.Leff, 'mechLambda': self.mechLambda, 'efY': self.efY, 'efZ': self.efZ, 'mode': self.mode, 'alpha_cr': self.alpha_cr, 'strongAxisBucklingPercent':self.strongAxisBucklingPercent})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BiaxialBendingStrengthControlVars,self).setFromDict(dct)
        self.Leff= dct['Leff'] # effective length.
        self.mechLambda= dct['mechLambda'] # mechanical slenderness.
        self.efY= dct['efY'] # additional eccentricity around Y axis.
        self.efZ= dct['efZ'] # additional eccentricity around Z axis.
        self.mode= dct['mode'] # buckling mode that corresponds to the previous parameters.
        self.alpha_cr= dct['alpha_cr'] # axial load divided by the critical axial load.
        self.strongAxisBucklingPercent= dct['strongAxisBucklingPercent'] # dot product (projection) of the buckling eigenvector over the strong axis.

class UniaxialBucklingControlVars(UniaxialBendingControlVars):
    '''Control variables for buckling limit state verification 
       verification.

    :ivar Leff: effective length for buckling.
    :ivar mechLambda: mechanical slenderness for buckling.
    :ivar ef: additional eccentricity for buckling.
    :ivar mode: buckling mode corresponding to the previous parameters.
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,M= 0.0, Leff= -1.0, mechLambda= -1.0, ef= 0.0, mode= None):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param M:       bending moment (defaults to 0.0)
        :param Leff: effective length for buckling.
        :param mechLambda: mechanical slenderness for buckling.
        :param ef: additional eccentricity for buckling.
        :param mode: buckling mode corresponding to the previous parameters.
        '''
        super(UniaxialBucklingControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, M= M)
        self.setBucklingParameters(Leff= Leff, mechLambda= mechLambda, ef= ef, mode= mode)
        
    def setBucklingParameters(self, Leff, mechLambda, ef, mode):
        ''' Assigns values to the buckling parameters.

        :param Leff: effective length for buckling.
        :param mechLambda: mechanical slenderness for buckling.
        :param ef: additional eccentricity for buckling.
        :param mode: buckling mode corresponding to the previous parameters.
        '''
        self.Leff= Leff # effective length for buckling.
        self.mechLambda= mechLambda # mechanical slenderness for buckling.
        self.ef= ef # additional eccentricity for buckling.
        self.mode= mode # buckling mode that corresponds to the previous parameters.
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(UniaxialBucklingControlVars, self).getDict()
        retval.update({'Leff': self.Leff, 'mechLambda': self.mechLambda, 'ef': self.ef, 'mode':self.mode})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BiaxialBendingStrengthControlVars,self).setFromDict(dct)
        self.Leff= dct['Leff'] #effective length for buckling.
        self.mechLambda= dct['mechLambda'] #mechanical slenderness for buckling.
        self.ef= dct['ef'] #additional eccentricity for buckling.
        self.mode= dct['mode'] # buckling mode that corresponds to the previous parameters.
    
class UniaxialBendingStrengthControlVars(UniaxialBendingControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification on steel-shape elements.

    :ivar Ncrd:  design strength to axial compression
    :ivar McRdz: design moment strength about Z (strong) axis
    :ivar chiLT: reduction factor for lateral-torsional buckling (defaults to 1)
    :ivar chiN:  reduction factor for compressive strength (defaults to 1)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0, N= 0.0, Mz= 0.0,Ncrd=0.0, McRdz=0.0,chiLT=1.0, chiN= 1.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdz:    design moment strength about Z (strong) axis
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(UniaxialBendingStrengthControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, My= Mz)
        self.Ncrd=Ncrd
        self.McRdz=McRdz
        self.chiLT=chiLT
        self.chiN= chiN
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(UniaxialBendingStrengthControlVars,self).getDict()
        retval.update({'Ncrd':self.Ncrd, 'McRdz':self.McRdz, 'chiLT':self.chiLT, 'chiN':self.chiN})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BiaxialBendingStrengthControlVars,self).setFromDict(dct)
        self.Ncrd= dct['Ncrd']
        self.McRdz= dct['McRdz']
        self.chiLT= dct['chiLT']
        self.chiN= dct['chiN']
        
class BiaxialBendingStrengthControlVars(BiaxialBendingControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification on steel-shape elements.

    :ivar Ncrd:  design strength to axial compression
    :ivar McRdy: design moment strength about Y (weak) axis
    :ivar McRdz: design moment strength about Z (strong) axis
    :ivar chiLT: reduction factor for lateral-torsional buckling (defaults to 1)
    :ivar chiN:  reduction factor for compressive strength (defaults to 1)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0,chiLT=1.0, chiN= 1.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdy:    design moment strength about Y (weak) axis
        :param McRdz:    design moment strength about Z (strong) axis
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(BiaxialBendingStrengthControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
        self.Ncrd=Ncrd
        self.McRdy=McRdy
        self.McRdz=McRdz
        self.chiLT=chiLT
        self.chiN= chiN
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(BiaxialBendingStrengthControlVars,self).getDict()
        retval.update({'Ncrd':self.Ncrd, 'McRdy':self.McRdy, 'McRdz':self.McRdz, 'chiLT':self.chiLT, 'chiN':self.chiN})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BiaxialBendingStrengthControlVars,self).setFromDict(dct)
        self.Ncrd= dct['Ncrd']
        self.McRdy= dct['McRdy']
        self.McRdz= dct['McRdz']
        self.chiLT= dct['chiLT']
        self.chiN= dct['chiN']
        
class SteelShapeUniaxialBendingControlVars(UniaxialBendingStrengthControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification en steel-shape elements.

    :ivar MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
    :ivar MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0, Mz= 0.0, Ncrd=0.0, McRdz=0.0,MvRdz=0.0, MbRdz=0.0, chiLT=1.0, chiN=1.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdz:    design moment strength about Z (strong) axis
        :param MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
        :param MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(SteelShapeUniaxialBendingControlVars,self).__init__(idSection= idSection,combName= combName, CF= CF, N= N, Mz= Mz, Ncrd= Ncrd, McRdz= McRdz, chiLT=chiLT, chiN= chiN)
        self.MvRdz=MvRdz
        self.MbRdz=MbRdz
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SteelShapeUniaxialBendingControlVars,self).getDict()
        retval.update({'MvRdz':self.MvRdz, 'MbRdz':self.MbRdz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(SteelShapeUniaxialBendingControlVars,self).setFromDict(dct)
        self.MvRdz= dct['MvRdz']
        self.MbRdz= dct['MbRdz']
        
class SteelShapeBiaxialBendingControlVars(BiaxialBendingStrengthControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification en steel-shape elements.

    :ivar MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
    :ivar MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0,MvRdz=0.0,MbRdz=0.0, chiLT=1.0, chiN=1.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdy:    design moment strength about Y (weak) axis
        :param McRdz:    design moment strength about Z (strong) axis
        :param MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
        :param MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(SteelShapeBiaxialBendingControlVars,self).__init__(idSection= idSection,combName= combName,CF= CF,N= N, My= My, Mz= Mz, Ncrd= Ncrd, McRdy= McRdy, McRdz= McRdz, chiLT=chiLT, chiN= chiN)
        self.MvRdz=MvRdz
        self.MbRdz=MbRdz
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SteelShapeBiaxialBendingControlVars,self).getDict()
        retval.update({'MvRdz':self.MvRdz, 'MbRdz':self.MbRdz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(SteelShapeBiaxialBendingControlVars,self).setFromDict(dct)
        self.MvRdz= dct['MvRdz']
        self.MbRdz= dct['MbRdz']


#       ___ _                 
#      / __| |_  ___ __ _ _ _ 
#      \__ \ ' \/ -_) _` | '_|
#      |___/_||_\___\__,_|_|  
# Shear control vars.
        
class RCShearControlVars(BiaxialBendingControlVars):
    '''Control variables for shear limit state verification in reinforced 
       concrete elements.

    :ivar Mu:       ultimate bending moment
    :ivar Vy:       shear force parallel to the y axis
    :ivar Vz:       shear force parallel to the z axis
    :ivar theta:    angle between the concrete compression struts and the beam axis
    :ivar Vu:       shear strength

    '''
    def __init__(self,idSection=-1,combName= 'nil',CF= -1.0,N= 0.0, My= 0.0, Mz= 0.0, Mu= 0.0, Vy= 0.0, Vz= 0.0, theta= 0.0, Vu= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Mu:       ultimate bending moment
        :param Vy:       shear force parallel to the y axis
        :param Vz:       shear force parallel to the z axis
        :param theta:    angle between the concrete compression struts and the beam axis
        :param Vu:       shear strength
        '''
        super(RCShearControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
        self.Mu= Mu #Ultimate bending moment.
        self.Vy= Vy #Shear parallel to the y axis.
        self.Vz= Vz #Shear parallel to the z axis.
        self.theta= theta #Strut angle.
        self.Vu= Vu # Shear strength.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(RCShearControlVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mu" +axis+","+str(self.Mu*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vz" +axis+","+str(self.Vz*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vu" +axis+","+str(self.Vu*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCShearControlVars,self).getDict()
        retval.update({'Mu':self.Mu, 'Vy':self.Vy, 'Vz':self.Vz, 'theta':self.theta, 'Vu':self.Vu})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(RCShearControlVars,self).setFromDict(dct)
        self.Mu= dct['Mu']
        self.Vy= dct['Vy']
        self.Vz= dct['Vz']
        self.theta= dct['theta']
        self.Vu= dct['Vu']

class SIATypeRCShearControlVars(RCShearControlVars):
    '''Control variables for shear limit state verification in EHE-08.

    :ivar Vcu: Vcu component of the shear strength.
    :ivar Vsu: Vsu component of the shear strength.
    '''
    def __init__(self,idSection=-1,combName= 'nil',CF= -1.0, N= 0.0, My= 0.0, Mz= 0.0, Mu= 0.0, Vy= 0.0, Vz= 0.0, theta= 0.0, Vcu= 0.0, Vsu= 0.0, Vu= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Mu:       ultimate bending moment
        :param Vy:       shear force parallel to the y axis
        :param Vz:       shear force parallel to the z axis
        :param theta:    angle between the concrete compression struts and the beam axis
        :param Vcu:      Vcu component of the shear strength (defined in the codes)
        :param Vsu:      Vsu component of the shear strength (defined in the codes)
        :param Vu:       shear strength
        '''
        super(SIATypeRCShearControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, My= My, Mz= Mz, Mu= Mu, Vy= Vy, Vz= Vz, theta= theta, Vu= Vu)
        self.Vcu= Vcu #Vcu component of the shear strength.
        self.Vsu= Vsu #Vsu component of the shear strength.
    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)
        '''
        retval= super(SIATypeRCShearControlVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Vcu" +axis+","+str(self.Vcu*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vsu" +axis+","+str(self.Vsu*factor)+"\n")
        return retval
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SIATypeRCShearControlVars,self).getDict()
        retval.update({'Vcu':self.Vcu, 'Vsu':self.Vsu})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(SIATypeRCShearControlVars,self).setFromDict(dct)
        self.Vcu= dct['Vcu']
        self.Vsu= dct['Vsu']

#        ___             _                _           _ 
#       / __|_ _ __ _ __| |__  __ ___ _ _| |_ _ _ ___| |
#      | (__| '_/ _` / _| / / / _/ _ \ ' \  _| '_/ _ \ |
#       \___|_| \__,_\__|_\_\ \__\___/_||_\__|_| \___/_|
# Crack control control vars.     

class CrackControlBaseVars(CFNMyMz):
    '''Biaxial bending. Cracking serviceability limit state variables.

    :ivar steelStress: maximum stress in the reinforcement bars
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0, My= 0.0, Mz= 0.0, steelStress= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency)
        :param N:        axial force
        :param My:       bending moment about Y axis
        :param Mz:       bending moment about Z axis
        :param steelStress: maximum stress in the reinforcement bars
        '''
    #    super(CrackControlBaseVars,self).__init__(combName,CF,N,My) #Jan 26th 2017
        super(CrackControlBaseVars,self).__init__(combName,CF,N,My,Mz)
        self.steelStress= steelStress #Stress in rebars.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CrackControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",steelStress" +axis+","+str(self.steelStress*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(CrackControlBaseVars,self).getDict()
        retval.update({'steelStress':self.steelStress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(CrackControlBaseVars,self).setFromDict(dct)
        self.steelStress= dct['steelStress']


class CrackControlVars(ControlVarsBase):
    '''Cracking serviceability limit state control variables.

    :ivar idSection: section identifier
    :ivar crackControlVarsPos:     Crack control in + face.
    :ivar crackControlVarsNeg:     Crack control in - face.
    '''
    def __init__(self, idSection= 'nil', crackControlBaseVarsPos= None ,crackControlBaseVarsNeg= None):
        '''
        Constructor.

        :param idSection: section identifier
        :param crackControlVarsPos:     Crack control in + face.
        :param crackControlVarsNeg:     Crack control in - face.
        '''
        super(CrackControlVars, self).__init__() # combName?
        self.idSection= idSection # Section identifier.
        if(crackControlBaseVarsPos):
            self.crackControlVarsPos= crackControlBaseVarsPos #Cracks in + face.
        else:
            self.crackControlVarsPos= CrackControlBaseVars()
        if(crackControlBaseVarsNeg):
            self.crackControlVarsNeg= crackControlBaseVarsNeg #Cracks in - face.
        else:
            self.crackControlVarsNeg= CrackControlBaseVars()

    def getCF(self):
        ''' Return the capacity factor.'''
        return max(self.crackControlVarsPos.getCF(),self.crackControlVarsNeg.getCF())

    def getMaxSteelStress(self):
        '''Maximum value for rebar stresses.'''
        return max(self.crackControlVarsPos.steelStress,self.crackControlVarsNeg.steelStress)

    def getMaxN(self):
        '''Maximum internal axial force.'''
        return max(self.crackControlVarsPos.N,self.crackControlVarsNeg.N)

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= 'idSection= "' + self.idSection + '"' 
        retval+= ', crackControlBaseVarsPos= ' + self.crackControlVarsPos.getStrConstructor()
        retval+= ', crackControlBaseVarsNeg= ' + self.crackControlVarsNeg.getStrConstructor()
        return retval
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= dict()
        retval.update({'idSection':self.idSection})
        retval.update({'crackControlBaseVarsPos':self.crackControlVarsPos.getStrConstructor()})
        retval.update({'crackControlBaseVarsNeg':self.crackControlVarsNeg.getStrConstructor()})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.idSection= dct['idSection']
        self.crackControlVarsPos= eval(dct['crackControlBaseVarsPos'])
        self.crackControlVarsNeg= eval(dct['crackControlBaseVarsNeg'])
        

class RCCrackControlVars(CFNMyMz):
    '''Control variables for cracking serviacebility limit state verification.

    :ivar idSection: section identifier.
    :ivar s_rmax: maximum distance between cracks (otherwise a new crack could occur in-between)
    :ivar wk: crack width
    '''
    def __init__(self, idSection=-1,combName= 'nil', CF=-1, N= 0.0, My= 0.0, Mz= 0.0, s_rmax= 0.0, sigma_s= 0.0, sigma_c= 0.0, wk= 0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor.
        :param N:        axial force
        :param My:       bending moment about Y axis
        :param Mz:       bending moment about Z axis
        :param s_rmax:   maximum distance between cracks (otherwise a new crack could occur in-between
        :param sigma_s: maximum tensile stress in the reinforcement.
        :param sigma_c: compressive stress in the most compressed concrete fibre.
        :param wk:       crack width
        '''
        super(RCCrackControlVars,self).__init__(combName= combName, CF= CF, N= N,My= My, Mz= Mz)
        self.idSection=idSection
        self.s_rmax=s_rmax
        self.sigma_s= sigma_s
        self.sigma_c= sigma_c
        self.wk=wk
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCCrackControlVars,self).getDict()
        retval.update({'s_rmax':self.s_rmax, 'sigma_s':self.sigma_s, 'sigma_c':self.sigma_c, 'wk': self.wk})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(RCCrackControlVars,self).setFromDict(dct)
        self.s_rmax= dct['s_rmax']
        self.sigma_s= dct['sigma_s']
        self.sigma_c= dct['sigma_c']
        self.wk= dct['wk']
        
class RCCrackStraightControlVars(RCCrackControlVars):
    '''Control variables for cracking serviacebility limit state verification
    when when considering a concrete stress-strain diagram that takes account
    of the effects of tension stiffening.

    :ivar eps_sm:   mean strain in the reinforcement when taking into account the effects of tension stiffening
    '''
    def __init__(self, idSection=-1, combName= 'nil', CF=-1, N= 0.0, My= 0.0, Mz= 0.0, s_rmax=0.0, eps_sm=0.0, wk=0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName: name of the load combinations to deal with
        :param N:        axial force
        :param My:       bending moment about Y axis
        :param Mz:       bending moment about Z axis
        :param s_rmax:   maximum distance between cracks (otherwise a new crack could occur in-between
        :param eps_sm:   mean strain in the reinforcement when taking into account the effects of tension stiffening
        :param wk:       crack width
        '''
        super(RCCrackStraightControlVars,self).__init__(idSection= idSection, combName= combName, CF= CF, N= N, My= My, Mz= Mz, s_rmax= s_rmax, wk= wk)
        self.eps_sm= eps_sm

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCCrackStraightControlVars,self).getDict()
        retval.update({'eps_sm':self.eps_sm})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(RCCrackStraightControlVars,self).setFromDict(dct)
        self.eps_sm= dct['eps_sm']


#       ___     _   _               
#      | __|_ _| |_(_)__ _ _  _ ___ 
#      | _/ _` |  _| / _` | || / -_)
#      |_|\__,_|\__|_\__, |\_,_\___|
#                    |___/          
# Fatigue control vars
  
class FatigueControlBaseVars(NMyMz):
    '''Biaxial bending. Fatigue limit state variables.

    :ivar Vy:       shear force parallel to Y axis.
    :ivar posSteelStress: traction stress in rebars.
    :ivar negSteelStress: compression stress in rebars.
    :ivar concreteStress: compression stress in concrete.
    '''
    def __init__(self,combName= 'nil',CF=-1.0,N= 0.0, My= 0.0, Mz= 0.0, Vy= 0.0, posSteelStress= 0.0, negSteelStress= 0.0, concreteStress= 0.0):
        '''
        Constructor.

        :param combName: name of the load combination to deal with
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Vy:       shear force parallel to Y axis.
        :param posSteelStress: traction stress in rebars.
        :param negSteelStress: compression stress in rebars.
        :param concreteStress: compression stress in concrete.
        '''
        #Note: Currently, CF attribute  has no sense in fatigue verification.
        # Perhaps, in the future, the maximum value
        #of the CF calculated can be represented by CF. For now, this
        # attribute remains in this class only for a purpose
        #of compatibility with the parent classes
        super(FatigueControlBaseVars,self).__init__(combName,N,My,Mz)
        self.Vy= Vy #Shear.
        self.posSteelStress= posSteelStress #traction stress in rebars.
        self.negSteelStress= negSteelStress #compression stress in rebars.
        self.concreteStress= concreteStress #compression stress in concrete.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(FatigueControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        retval.append("detab,"+str(eTag)+",posSteelStress" +axis+","+str(self.posSteelStress*factor*factor)+"\n")
        retval.append("detab,"+str(eTag)+",negSteelStress" +axis+","+str(self.negSteelStress*factor*factor)+"\n")
        retval.append("detab,"+str(eTag)+",concreteStress" +axis+","+str(self.concreteStress*factor*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(FatigueControlBaseVars,self).getDict()
        retval.update({'Vy':self.Vy, 'negSteelStress':self.negSteelStress, 'posSteelStress':self.posSteelStress, 'concreteStress': self.concreteStress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(FatigueControlBaseVars,self).setFromDict(dct)
        self.Vy= dct['Vy']
        self.negSteelStress= dct['negSteelStress']
        self.posSteelStress= dct['posSteelStress']
        self.concreteStress= dct['concreteStress']


class FatigueControlVars(ControlVarsBase):
    '''Fatigue limit state control variables.

    :ivar idSection: section identifier
    :ivar combName:  name of the load combinations to deal with
    :ivar state0:    Fatigue values (FatigueControlBaseVars) under permanent load.
    :ivar state1:    Fatigue values (FatigueControlBaseVars) under fatigue load.
    :ivar concreteLimitStress: limit for the concrete stress as specified in SIA 262(2013)  4.3.8.3.1
    :ivar concreteBendingCF:   concrete capacity factor under fatigue due to normal stresses.
    :ivar shearLimit:        limit for the shear force as sepecified in SIA 262(2013)  4.3.8.3.2
    :ivar concreteShearCF:   concrete capacity factor under fatigue due to shear forces.
    :ivar Mu:        ultimate bending moment
    :ivar Vu:        ultimate shear force  
    '''
    def __init__(self, idSection= 'nil', controlBaseVars0= None, controlBaseVars1= None, concreteLimitStress= 0.0,concreteBendingCF=-1.0,shearLimit=0.0,concreteShearCF=-1.0,Mu=0.0,Vu=0.0):
        '''
        Constructor.

        :param idSection: section identifier.
        :param combName:  name of the load combinations to deal with
        :param state0:    Fatigue values (FatigueControlBaseVars) under permanent load.
        :param state1:    Fatigue values (FatigueControlBaseVars) under fatigue load.
        :param concreteLimitStress: limit for the concrete stress as specified in SIA 262(2013)  4.3.8.3.1
        :param concreteBendingCF:   concrete capacity factor under fatigue due to normal stresses.
        :param shearLimit:        limit for the shear force as sepecified in SIA 262(2013)  4.3.8.3.2
        :param concreteShearCF:   concrete capacity factor under fatigue due to shear forces.
        :param Mu:        ultimate bending moment
        :param Vu:        ultimate shear force  
        '''
        super(FatigueControlVars, self).__init__() # combName?
        self.idSection= idSection # Section identifier.
        if(controlBaseVars0):
            self.state0=  controlBaseVars0 #Under permanent loads.
        else:
            self.state0= FatigueControlBaseVars()
        if(controlBaseVars1):
            self.state1= controlBaseVars1 #Under fatigue loads.
        else:
            self.state1= FatigueControlBaseVars()
        self.concreteLimitStress= concreteLimitStress #SIA 262(2013)  4.3.8.3.1
        self.concreteBendingCF= concreteBendingCF #Concrete capacity factor.
        self.shearLimit=shearLimit #SIA 262(2013)  4.3.8.3.2
        self.concreteShearCF= concreteShearCF #Concrete shear factor.
        self.Mu= Mu #Ultimate bending moment.
        self.Vu= Vu #Ultimate shear.

    def getSteelPosStressIncrement(self):
        '''Returns positive stress increment in rebars.'''
        return self.state1.posSteelStress-self.state0.posSteelStress

    def getSteelNegStressIncrement(self):
        '''Returns negative stress increment in rebars.'''
        return self.state1.negSteelStress-self.state0.negSteelStress
      
    def getAbsSteelStressIncrement(self):
        '''Returns maximun stress increment in rebars (absolute value).'''
        return max(self.getSteelPosStressIncrement(),-self.getSteelNegStressIncrement())

    def getConcreteStressIncrement(self):
        '''Returns stress increment in concrete.'''
        return self.state1.concreteStress-self.state0.concreteStress

    def getConcreteMaxMinStresses(self):
        '''Used in FatigueController.concreteLimitStress.''' 
        retval= [0.0,0.0]
        retval[0]= min(min(self.state0.concreteStress,self.state1.concreteStress),0.0) # No positive stresses.
        retval[1]= min(max(self.state0.concreteStress,self.state1.concreteStress),0.0) # No positive stresses.
        return retval

    def getConcreteMinStress(self):
        '''Returns minimum (max. compressive) concrete stress between loaded and unloaded states.'''
        sgc0= abs(min(self.state0.concreteStress,0.0))
        sgc1= abs(min(self.state1.concreteStress,0.0))
        return max(sgc0,sgc1)

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= dict()
        retval.update({'idSection':self.idSection})
        retval.update({'controlBaseVars0':self.state0.getStrConstructor()})
        retval.update({'controlBaseVars1':self.state1.getStrConstructor()})
        retval.update({'concreteLimitStress':self.concreteLimitStress, 'concreteBendingCF':self.concreteBendingCF, 'shearLimit':self.shearLimit, 'concreteShearCF':self.concreteShearCF, 'Mu':self.Mu, 'Vu':self.Vu})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        self.idSection= dct['idSection']
        self.controlBaseVars0= eval(dct['controlBaseVars0'])
        self.controlBaseVars1= eval(dct['controlBaseVars1'])
        self.concreteLimitStress= dct['concreteLimitStress']
        self.concreteBendingCF= dct['concreteBendingCF']
        self.shearLimit= dct['shearLimit']
        self.concreteShearCF= dct['concreteShearCF']
        self.Mu= dct['Mu']
        self.Vu= dct['Vu']



#       ___ _                    _                     __
#      | _ \ |__ _ _ _  ___   __| |_ _ _ ___ ______   / /
#      |  _/ / _` | ' \/ -_) (_-<  _| '_/ -_|_-<_-<  / / 
#      |_| |_\__,_|_||_\___| /__/\__|_| \___/__/__/ /_/  
#       _ __| |__ _ _ _  ___   __| |_ _ _ __ _(_)_ _     
#      | '_ \ / _` | ' \/ -_) (_-<  _| '_/ _` | | ' \    
#      | .__/_\__,_|_||_\___| /__/\__|_| \__,_|_|_||_|   
#      |_|                                               
# Plane stress / plane strain control vars.
class PlaneStressControlVars(ControlVarsBase, stresses.Stresses2D):
    '''2D solid mechanics stresses control vars.

    :ivar CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self, combName= 'nil', CF= -1.0, sigma_11= 0.0, sigma_12= 0.0, sigma_22= 0.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param sigma_11:  (1,1) component of the stress tensor.
        :param sigma_12:  (1,2) component of the stress tensor.
        :param sigma_22:  (2,2) component of the stress tensor.
        '''
        ControlVarsBase.__init__(self, combName)
        stresses.Stresses2D.__init__(self, sigma_11= sigma_11, sigma_12= sigma_12, sigma_22= sigma_22)
        self.CF= CF # Capacity factor or efficiency

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= ControlVarsBase.getDict(self)
        retval.update(stresses.Stresses2D.getDict(self))
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        ControlVarsBase.setFromDict(self, dct)
        stresses.Stresses2D.setFromDict(self, dct)

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

#       ___ ___ ___    _     _                       ___ 
#      | __/ __|_  )__/ |   /_\  _ _  _ _  _____ __ | __|
#      | _| (__ / /___| |  / _ \| ' \| ' \/ -_) \ / | _| 
#      |___\___/___|  |_| /_/ \_\_||_|_||_\___/_\_\ |_|  
# EN 1992-1-1:2004. Annex F.
# Tension reinforcement expressions for in-plane stress conditions
class Ec2InPlaneStressControlVars(PlaneStressControlVars):
    '''Control vars to check tension reinforcement expressions for 
       in-plane stress conditions according to Annex F of Eurocode 2
       part 1.

    :ivar ftd1_req: required reinforcement strength along the axis 1.
    :ivar ftd2_req: required reinforcement strength along the axis 2.
    :ivar sigma_c_req: required concrete strength.
    '''
    def __init__(self, combName= 'nil', CF= -1.0, sigma_11= 0.0, sigma_12= 0.0, sigma_22= 0.0, ftd1_req= 0.0, ftd2_req= 0.0, sigma_c_req= 0.0, ftd1_cf= -1.0, ftd2_cf= -1.0, sigma_c_cf= -1.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param sigma_11:  (1,1) component of the stress tensor.
        :param sigma_12:  (1,2) component of the stress tensor.
        :param sigma_22:  (2,2) component of the stress tensor.
        :param ftd1_req: required reinforcement strength along the axis 1.
        :param ftd2_req: required reinforcement strength along the axis 2.
        :param sigma_c_req: required concrete strength.
        :param ftd1_cf: capacity factor of the required reinforcement strength along the axis 1.
        :param ftd2_cf: capacity factor of the required reinforcement strength along the axis 2.
        :param sigma_c_cf: capacity factor of the required concrete strength.
        '''
        super(Ec2InPlaneStressControlVars,self).__init__(combName, CF= CF, sigma_11= sigma_11, sigma_12= sigma_12, sigma_22= sigma_22)
        self.ftd1_req= ftd1_req # required reinforcement strength along the axis 1.
        self.ftd2_req= ftd2_req # required reinforcement strength along the axis 2.
        self.sigma_c_req= sigma_c_req # required concrete strength.
        self.ftd1_cf= ftd1_cf # capacity factor of the required reinforcement strength along the axis 1.
        self.ftd2_cf= ftd2_cf # capacity factor of the required reinforcement strength along the axis 2.
        self.sigma_c_cf= sigma_c_cf # capacity factor of the required concrete strength.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(Ec2InPlaneStressControlVars,self).getDict()
        retval.update({'ftd1_req':self.ftd1_req, 'ftd2_req':self.ftd2_req, 'sigma_c_req':self.sigma_c_req, 'ftd1_cf':self.ftd1_cf, 'ftd2_cf':self.ftd2_cf, 'sigma_c_cf':self.sigma_c_cf})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(VonMisesControlVars, self).setFromDict(dct)
        self.ftd1_req= dct['ftd1_req']
        self.ftd2_req= dct['ftd2_req']
        self.sigma_c_req= dct['sigma_c_req']
        self.ftd1_cf= dct['ftd1_cf']
        self.ftd2_cf= dct['ftd2_cf']
        self.sigma_c_cf= dct['sigma_c_cf']

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF
        
#      __   __          __  __ _            
#      \ \ / /__ _ _   |  \/  (_)___ ___ ___
#       \ V / _ \ ' \  | |\/| | (_-</ -_|_-<
#        \_/\___/_||_| |_|  |_|_/__/\___/__/
# Von mises control vars.                                   
class VonMisesControlVars(ControlVarsBase):
    '''Von Mises stresses control vars.

    :ivar von_mises_stress:  Von Mises stress (defaults to 0.0)
    :ivar CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil', CF= -1.0, vm_stress= 0.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param vm_stress: von mises stress (defaults to 0.0)
        :param CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        '''
        super(VonMisesControlVars,self).__init__(combName)
        self.vm_stress= vm_stress # von Mises stress.
        self.CF= CF # Capacity factor or efficiency

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(VonMisesControlVars,self).getDict()
        retval.update({'vm_stress':self.vm_stress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(VonMisesControlVars,self).setFromDict(dct)
        self.vm_stress= dct['vm_stress']

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF


#       ___         _   _ _           
#      | _ )_  _ __| |_| (_)_ _  __ _ 
#      | _ \ || / _| / / | | ' \/ _` |
#      |___/\_,_\__|_\_\_|_|_||_\__, |
#                               |___/ 
# Buckling control vars.
class BucklingControlVarsBase(NMyMz):
    '''Control variables for buckling ultimate limit state verification.

    :ivar effectiveLengths: buckling effective lengths for the elemement (first index: buckling mode, second index: axis).
    :ivar mechLambda: mechanical slenderness of the element (first index: buckling mode, second index: axis).
    '''
    def __init__(self, combName= 'nil', N= 0.0,My= 0.0,Mz= 0.0, effectiveLengths= None, mechLambda= None):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:  axial force (defaults to 0.0)
        :param My: bending moment about Y axis (defaults to 0.0)
        :param Mz: bending moment about Z axis (defaults to 0.0)
        :param effectiveLengths: buckling effective lengths for the elemement (first index: buckling mode, second index: axis).
        :param mechLambda: mechanical slenderness of the element (first index: buckling mode, second index: axis).
        '''
        super(BucklingControlVarsBase,self).__init__(combName= combName, N= N, My= My, Mz= Mz)
        self.effectiveLengths= list() if effectiveLengths is None else effectiveLengths
        self.mechLambda= list() if mechLambda is None else mechLambda
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(BucklingControlVarsBase,self).getDict()
        retval.update({'effectiveLengths':self.effectiveLengths, 'mechLambda':self.mechLambda})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(BucklingControlVarsBase,self).setFromDict(dct)
        self.effectiveLengths= dct['effectiveLengths']
        self.mechLambda= dct['mechLambda']

class RCBucklingControlVars(BucklingControlVarsBase):
    '''Control variables for buckling ultimate limit state verification on 
       reinforced concrete elements.

    '''
    def __init__(self, combName= 'nil', N= 0.0,My= 0.0,Mz= 0.0, effectiveLengths= None, mechLambda= None, fictitiousEccentricities= None):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:  axial force (defaults to 0.0)
        :param My: bending moment about Y axis (defaults to 0.0)
        :param Mz: bending moment about Z axis (defaults to 0.0)
        :param effectiveLengths: buckling effective lengths for the elemement (first index: buckling mode, second index: axis).
        :param mechLambda: mechanical slenderness of the element (first index: buckling mode, second index: axis).
        :param fictitiousEccenctricities: fictitious eccentricities for the element (first index: buckling mode, second index: axis).
        '''
        super(RCBucklingControlVars,self).__init__(combName= combName, N= N, My= My, Mz= Mz, effectiveLengths= effectiveLengths, mechLambda= mechLambda)
        self.fictitiousEccenctricities= list() if fictitiousEccenctricities is None else fictitiousEccenctricities
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCBucklingControlVars,self).getDict()
        retval.update({'fictitiousEccenctricities':self.fictitiousEccenctricities})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(RCBucklingControlVars,self).setFromDict(dct)
        self.fictitiousEccenctricities= dct['fictitiousEccenctricities']
        
class SteelBucklingControlVars(BucklingControlVarsBase):
    '''Control variables for buckling ultimate limit state verification on
       steel elements.

    '''
    def __init__(self, combName= 'nil', N= 0.0,My= 0.0,Mz= 0.0, effectiveLengths= None, mechLambda= None, strengthReductionFactors= None, bucklingResistance= None):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:  axial force (defaults to 0.0)
        :param My: bending moment about Y axis (defaults to 0.0)
        :param Mz: bending moment about Z axis (defaults to 0.0)
        :param effectiveLengths: buckling effective lengths for the elemement (first index: buckling mode, second index: axis).
        :param mechLambda: mechanical slenderness of the element (first index: buckling mode, second index: axis).
        :param strengthReductionFactors: buckling strength reduction factors of the element (first index: buckling mode, second index: axis).
        :param bucklingResistance: buckling resistance of the element (first index: buckling mode, second index: axis).
        '''
        super(SteelBucklingControlVars,self).__init__(combName= combName, N= N, My= My, Mz= Mz, effectiveLengths= effectiveLengths, mechLambda= mechLambda)
        self.strengthReductionFactors= list() if strengthReductionFactors is None else strengthReductionFactors
        self.bucklingResistance= list() if bucklingResistance is None else bucklingResistance
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SteelBucklingControlVars,self).getDict()
        retval.update({'strengthReductionFactors':self.strengthReductionFactors})
        retval.update({'bucklingResistance':self.bucklingResistance})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(SteelBucklingControlVars,self).setFromDict(dct)
        self.strengthReductionFactors= dct['strengthReductionFactors']
        self.bucklingResistance= dct['bucklingResistance']
        
class StrutAndTieControlVars(CFN):
    '''Control variables for elements of strut-and-tie models.

    :ivar typo: element type ('strut' or 'tie').
    '''
    def __init__(self, CF= -1.0, combName= None, N= 0.0, stress= 0.0, typo= None, inverted= False):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with.
        :param N: axial force (defaults to 0.0).
        :param typo: element type 'strut' or 'tie'.
        :param inverted: if true, the element is working in the opposite way
                         to what was intended: strut in tension or tie in
                         compression.
        '''
        super(StrutAndTieControlVars,self).__init__(CF= CF, combName= combName, N= N)
        self.stress= stress
        self.typo= typo
        self.inverted= inverted
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(StrutAndTieControlVars,self).getDict()
        if(self.stress!=0.0):
            retval.update({'stress':self.stress})
        retval.update({'typo':self.typo, 'inverted':self.inverted})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        super(StrutAndTieControlVars,self).setFromDict(dct)
        if('stress' in dct):
            self.stress= retval['stress']
        self.typo= dct['typo']
        self.inverted= dct['inverted']

# Control vars classes end.
        
def read_control_vars(preprocessor, inputFileName):
    ''' Read control var data from the input file an put them as properties
        of the model elements and/or nodes.

    :param preprocessor: pre-processor for the finite element model.
    :param inputFileName: name of the input file containing the data.
    :returns: number of properties read.
    '''
    retval= 0
    try:
        with open(inputFileName) as f:
            dataDict= json.load(f)
    except IOError:
        lmsg.error("can't read from file: "+str(inputFileName))
        return retval
    if 'elementData' in dataDict: # Control variables on elements.
        elementData= dataDict['elementData']
        elementHandler= preprocessor.getElementHandler
        for eKey in elementData: # iterate on elements.
            elementTag= int(eKey)
            element= elementHandler.getElement(elementTag)
            elementControlVars= elementData[eKey]
            for propKey in elementControlVars: # iterate on element control vars.
                expression= elementControlVars[propKey]
                propValue= eval(expression)
                element.setProp(propKey, propValue)
                retval+= 1
    if 'nodeData' in dataDict: # Control variables on nodes.
        nodeData= dataDict['nodeData']
        nodeHandler= preprocessor.getNodeHandler
        for eKey in nodeData: # iterate on nodes.
            nodeTag= int(eKey)
            node= nodeHandler.getNode(nodeTag)
            nodeControlVars= nodeData[eKey]
            for propKey in nodeControlVars: # iterate on node control vars.
                propValue= eval(nodeControlVars[propKey])
                node.setProp(propKey, propValue)
                retval+= 1
    return retval

def get_control_vars_dict(elements, controlVarName, sections):
    '''Return a dictionary with the values of the control variables for each
     of the given elements.

    :param elements: elements to get the control variables from.
    :param controlVarName: name of the control var to populate the dictionary with.
    :param sections: names of the sections of each element.
    '''
    retval= dict()
    for e in elements:
        eTag= e.tag
        if not eTag in retval:
            retval[eTag]= dict()
        for s in sections:
            propName= controlVarName+s
            controlVar= e.getProp(propName)        
            retval[eTag][propName]= controlVar
    return retval            
    
def get_element_data_dict(controlVarsDict, controlVarName):
    ''' Return a dictionary populated with the control variables for each element.

    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    :param controlVarName: name of the control var to populate the dictionary 
                           with.
    '''
    retval= dict()
    for eTag in controlVarsDict:
        elementControlVars= controlVarsDict[eTag]
        if not eTag in retval:
            retval[eTag]= dict()
        sz= len(elementControlVars)
        if(sz>1): # Many records for each element.
            for index in elementControlVars:
                controlVar= elementControlVars[index]
                sectionName= 'Sect'+str(index)
                propName= controlVarName+sectionName
                retval[eTag][propName]= controlVar.getStrConstructor()
        else: # One record for each element.
            index= next(iter(elementControlVars))
            controlVar= elementControlVars[index]
            propName= controlVarName
            retval[eTag][propName]= controlVar.getStrConstructor()
    return retval

def write_latex_control_vars(outputFile, controlVarsDict):
    ''' Write the values of the control vars in a LaTeX file.

    :param outputFile: output file.
    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    '''
    sectionLines= dict()
    for eTag in controlVarsDict:
        elementControlVars= controlVarsDict[eTag]
        for index in elementControlVars:
            controlVar= elementControlVars[index]
            if not index in sectionLines:
                sectionLines[index]= list()
            outStr= controlVar.getLaTeXString(eTag,1e-3)
            sectionLines[index].append(outStr)
    for index in sectionLines:
        header= 'Section '+str(index)+'\n'
        outputFile.write(header)
        for string in sectionLines[index]:
            outputFile.write(string)
            
def get_capacity_factors_from_control_vars(controlVarsDict):
    ''' Return the capacity factors from the given control vars.

    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    '''
    retval= dict()
    for eTag in controlVarsDict:
        elementControlVars= controlVarsDict[eTag]
        for index in elementControlVars:
            controlVar= elementControlVars[index]
            if not index in retval:
                retval[index]= list()
            retval[index].append(controlVar.getCF())
    return retval
            
def write_control_vars_from_phantom_elements(controlVarsDict, outputCfg):
    '''Writes to file the control-variable values calculated for
     the RC elements in the phantom model.

    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (append or not
           the results to a file, generation or not of lists, ...)
    '''
    outputFileName= outputCfg.outputDataBaseFileName # name for the .json and .tex files.
    controlVarName= outputCfg.controller.limitStateLabel
    dataDict= None
    jsonFileName= outputFileName+'.json'
    if outputCfg.appendToResFile.lower()[0]=='y':
        try:
            with open(jsonFileName) as f:
               dataDict= json.load(f)
        except IOError:
            lmsg.error("can't read from file: "+str(outputFileName))
    else:
        dataDict= dict()
    elementDataDict= get_element_data_dict(controlVarsDict= controlVarsDict, controlVarName= controlVarName)
    dataDict['elementData']= elementDataDict

    # Write the dictionary in a JSON file.
    with open(jsonFileName, 'w') as f:
        json.dump(dataDict, f)       

    if outputCfg.listFile.lower()[0]=='y':
        if outputCfg.appendToResFile.lower()[0]=='y':
            texOutput= open(outputFileName+".tex","a+")
        else:
            texOutput= open(outputFileName+".tex","w+")
        write_latex_control_vars(outputFile= texOutput, controlVarsDict= controlVarsDict)
        texOutput.close()
    retval=None
    if outputCfg.calcMeanCF.lower()[0]=='y':
        capacityFactors= get_capacity_factors_from_control_vars(controlVarsDict= controlVarsDict)
        retval= list()
        for key in capacityFactors:
            retval.append(scipy.mean(capacityFactors[key]))
    return retval

def getControlVarImportModuleStr(preprocessor, outputCfg, sections):
    '''Return the string to import the module where the control var is defined.

    :param preprocessor:    preprocessor from FEA model.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (set of 
           elements to be analyzed [defaults to 'total'], append or not the 
           results to the result file [defatults to 'N'], generation or not of 
           list file [defatults to 'N', ...)
    :param sections: names of the sections to write the output for.
    '''
    retval= None
    elems= outputCfg.getCalcSetElements(preprocessor) # elements in set 'setCalc'
    controlVarName= outputCfg.controller.limitStateLabel
    if(len(elems)>0):
        e0= elems[0]
        s= sections[0]
        propName= controlVarName+s
        controlVar= e0.getProp(propName)
        retval= controlVar.getModuleImportString()
    else:
        lmsg.error('element set is empty.')
    return retval

def write_control_vars_from_elements(preprocessor, controlVarsDict, outputCfg, sections):
    '''Writes to file the control-variable values calculated for elements in set 'setCalc'. 

    :param preprocessor:    preprocessor from FEA model.
    :param controlVarsDict: dictionary containing the values of the control 
                            variables for each element.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (set of 
           elements to be analyzed [defaults to 'total'], append or not the 
           results to the result file [defatults to 'N'], generation or not of 
           list file [defatults to 'N', ...)
    :param sections: names of the sections to write the output for.
    '''
    controlVarName= outputCfg.controller.limitStateLabel
    dataDict= None
    outputFileName= outputCfg.outputDataBaseFileName # name for the .json and .tex files.
    jsonFileName= outputFileName+'.json'
    if(outputCfg.appendToResFile.lower()[0]=='y' and os.path.isfile(jsonFileName)):
        try:
            with open(jsonFileName) as f:
               dataDict= json.load(f)
        except IOError:
            lmsg.error("can't read from file: "+str(jsonFileName)+". Are you sure you need to read previous results?")
            quit()
    else:
        dataDict= dict()
    # Write report in JSON format.
    importString= getControlVarImportModuleStr(preprocessor, outputCfg, sections)
    dataDict['importString']= importString
    elementDataDict= get_element_data_dict(controlVarsDict= controlVarsDict, controlVarName= controlVarName)
    dataDict['elementData']= elementDataDict
    # for e in elems:
    #     elementDataDict[e.tag]= dict()
    #     for s in sections:
    #         propName= controlVarName+s
    #         controlVar= e.getProp(propName)
    #         elementDataDict[e.tag][propName]= controlVar.getStrConstructor()
    with open(jsonFileName, 'w') as f:
        json.dump(dataDict, f)
    
    # Write report in LaTeX format.
    if outputCfg.listFile.lower()[0]=='y':
        if outputCfg.appendToResFile.lower()[0]=='y':
            texOutput= open(outputFileName+".tex","a+")
        else:
            texOutput= open(outputFileName+".tex","w+")
        write_latex_control_vars(outputFile= texOutput, controlVarsDict= controlVarsDict)
        # for s in sections:
        #     texOutput.write("Section: "+s+"\n")
        #     propName= controlVarName+s
        #     for e in elems:
        #         controlVar= e.getProp(propName)
        #         outStr= controlVar.getLaTeXString(e.tag,1e-3)
        #         texOutput.write(outStr)
        texOutput.close()
    retval=None
    if outputCfg.calcMeanCF.lower()[0]=='y':
        capacityFactors= get_capacity_factors_from_control_vars(controlVarsDict= controlVarsDict)
        retval= list()
        for key in capacityFactors:
            retval.append(scipy.mean(capacityFactors[key]))
    return retval


def write_control_vars_from_elements_for_ansys(preprocessor, outputCfg, sectionName1, sectionName2):
    '''
    :param preprocessor: preprocessor for the FE problem.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (set of 
           elements to be analyzed [defaults to 'total'], append or not the 
           results to the result file [defatults to 'N'], generation or not of 
           list file [defatults to 'N', ...)
    :param sectioName1: name of the first section to write the output for.
    :param sectioName2: name of the second section to write the output for.
    '''
    def odd(v):
        if v & 1:
            return True
        else:
            return False
    controlVarName= outputCfg.controller.limitStateLabel
    texOutput1= open("/tmp/texOutput1.tmp","w")
    texOutput1.write("Section 1\n")
    texOutput2= open("/tmp/texOutput2.tmp","w")
    texOutput2.write("Section 2\n")
    outputFileName= outputCfg.outputDataBaseFileName # name for the .json and .tex files.
    ansysOutput1= open(outputFileName+".mac","w")
    ansysOutput2= open(outputFileName+"esf.mac","w")
    #printCabeceraListadoCapacityFactor("texOutput1","1 ("+ sectionName1 +")")
    #printCabeceraListadoCapacityFactor("texOutput2","2 ("+ sectionName2 +")")
    fcs1= [] #Capacity factors at section 1.
    fcs2= [] #Capacity factors at section 2.
    elems= preprocessor.getSets["total"].elements
    for e in elems:
        eTag= e.getProp("idElem")
        controlVar= e.getProp(controlVarName)
        outStr= controlVar.getLaTeXString(eTag,1e-3)
        if(odd(e.tag)):
            fcs1.append(controlVar.getCF())
            texOutput1.write(outStr)
            ansOut= controlVar.getAnsysStrings(eTag,'1',1e-3)
            for s in ansOut:
                ansysOutput1.write(s)
        else:
            fcs2.append(controlVar.getCF())
            texOutput2.write(outStr)
            ansOut= controlVar.getAnsysStrings(eTag,'2',1e-3)
            for s in ansOut:
                ansysOutput2.write(s)
    #printCierreListadoCapacityFactor("texOutput1")
    #printCierreListadoCapacityFactor("texOutput2")
    texOutput1.close()
    texOutput2.close()
    ansysOutput1.close()
    ansysOutput2.close()
    os.system("cat /tmp/texOutput1.tmp /tmp/texOutput2.tmp > "+outputFileName+".tex")
    # os.system("rm -f "+"/tmp/acciones.xci")
    # os.system("rm -f "+"/tmp/cargas.xci")
    # os.system("rm -f "+"/tmp/elems.xci")
    os.system("rm -f "+"/tmp/texOutput1.tmp")
    os.system("rm -f "+"/tmp/texOutput2.tmp")
    retval= [scipy.mean(fcs1),scipy.mean(fcs2)]
    return retval

def get_control_var_values_from_elements(elements, propName, argument):
    ''' Return a dictionary containing the values of the given control var
        in the given element set. The dictiorary key is the element tag.

     :param element: sequence of elements to retrieve the values for.
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
    '''
    retval= dict()
    for e in elements:
        if(e.hasProp(propName)):
            controlVar= e.getProp(propName)
            value= controlVar(argument)
        else:
            value= None
        retval[e.tag]= value
    return retval

def get_maximum_control_var_value_on_surfaces(xcSet, propName, argument):
    ''' Compute the maximum of a control var value for each surface of the
        given set.

     :param xcSet: set of surfaces
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
    '''
    retval= dict()
    sect1CV= propName+'Sect1'
    sect2CV= propName+'Sect2'
    for s in xcSet.surfaces:
        cvDict1= get_control_var_values_from_elements(elements= s.elements, propName= sect1CV, argument= argument)
        cvDict2= get_control_var_values_from_elements(elements= s.elements, propName= sect2CV, argument= argument)
        listCV1= list()
        listCV2= list()
        for key in cvDict1:
            cv1= cvDict1[key]
            if(cv1 is not None):
                listCV1.append(cv1)
            cv2= cvDict2[key]
            if(cv2 is not None):
                listCV2.append(cv2)
        retval[s.tag]= [max(listCV1), max(listCV2)]
    return retval

def get_maximum_control_var_value_on_lines(xcSet, propName, argument):
    ''' Compute the maximum of a control var value for each line of the
        given set.

     :param xcSet: set of lines
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
    '''
    retval= dict()
    sect1CV= propName+'Sect1'
    sect2CV= propName+'Sect2'
    for l in xcSet.lines:
        cvDict1= get_control_var_values_from_elements(elements= l.elements, propName= sect1CV, argument= argument)
        cvDict2= get_control_var_values_from_elements(elements= l.elements, propName= sect2CV, argument= argument)
        listCV1= list()
        listCV2= list()
        for key in cvDict1:
            cv1= cvDict1[key]
            if(cv1 is not None):
                listCV1.append(cv1)
            cv2= cvDict2[key]
            if(cv2 is not None):
                listCV2.append(cv2)
        retval[l.tag]= [max(listCV1), max(listCV2)]
    return retval

def extrapolate_control_var_from_elements(preprocessor, elements, propName, argument, initialValue= 0.0):
    '''Extrapolates element's control var values to the nodes.

     :param preprocessor: preprocessor of the FE problem.
     :param elements: sequence of elements to retrieve the control variables for.
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
     :param initialValue: initial value for the prop defined at the nodes.
    '''
    nodePropName= propName+'_'+argument
    touchedNodes= ext.create_attribute_at_nodes(elements,nodePropName,initialValue)
    #Calculate totals.
    for e in elements:
        elemNodes= e.getNodes
        if(e.hasProp(propName)):
            controlVar= e.getProp(propName)
        else: # Silently ignore this value (LP 26/07/2022).
            controlVar= None
        for n in elemNodes:
            if(controlVar):
                value= controlVar(argument)
                oldValue= n.getProp(nodePropName)
                n.setProp(nodePropName,oldValue+value)
    #Divide by number of elements in the set that touch the node.
    for tag in touchedNodes: # Nodes touched by the elements in the set.
        n= preprocessor.getNodeHandler.getNode(tag)
        denom= float(touchedNodes[tag])
        newValue= n.getProp(nodePropName)/denom
        n.setProp(nodePropName,newValue)
    return nodePropName

def extrapolate_control_var(elemSet, propName, argument, initialValue= 0.0):
    '''Extrapolates element's control var values to the nodes.

     :param elemSet: set of elements.
     :param propName: name of the property that contains the control variables.
     :param argument: name of the control variable to extrapolate.
     :param initialValue: initial value for the prop defined at the nodes.
    '''
    elemSet.fillDownwards()
    eSet= elemSet.elements
    return extrapolate_control_var_from_elements(preprocessor= elemSet.getPreprocessor, elements= eSet, propName= propName, argument= argument, initialValue= initialValue)


