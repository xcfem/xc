# -*- coding: utf-8 -*-
'''Base classes to store limit state control variables (internal forces, 
   strains, stresses,...) calculated in the analysis.
'''

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2026,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"

import sys
from misc_utils import log_messages as lmsg
from materials.stresses import Stresses2D
from materials.sections.internal_forces import ShellMaterialInternalForces
from postprocess.reports import common_formats as fmt

#        ___         _           _        
#       / __|___ _ _| |_ _ _ ___| |       
#      | (__/ _ \ ' \  _| '_/ _ \ |       
#       \___\___/_||_\__|_| \___/_|       
#      __ ____ _ _ _(_)__ _| |__| |___ ___
#      \ V / _` | '_| / _` | '_ \ / -_|_-<
#       \_/\__,_|_| |_\__,_|_.__/_\___/__/
# Classes defining basic control variables.

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
    def __init__(self, idSection= 'nil',combName= 'nil', N= 0.0):
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


#       ___ _                    _                     __
#      | _ \ |__ _ _ _  ___   __| |_ _ _ ___ ______   / /
#      |  _/ / _` | ' \/ -_) (_-<  _| '_/ -_|_-<_-<  / / 
#      |_| |_\__,_|_||_\___| /__/\__|_| \___/__/__/ /_/  
#       _ __| |__ _ _ _  ___   __| |_ _ _ __ _(_)_ _     
#      | '_ \ / _` | ' \/ -_) (_-<  _| '_/ _` | | ' \    
#      | .__/_\__,_|_||_\___| /__/\__|_| \__,_|_|_||_|   
#      |_|                                               
# Plane stress / plane strain control vars.
class PlaneStressControlVars(ControlVarsBase, Stresses2D):
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
        Stresses2D.__init__(self, sigma_11= sigma_11, sigma_12= sigma_12, sigma_22= sigma_22)
        self.CF= CF # Capacity factor or efficiency

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= ControlVarsBase.getDict(self)
        retval.update(Stresses2D.getDict(self))
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        ControlVarsBase.setFromDict(self, dct)
        Stresses2D.setFromDict(self, dct)

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

#       ___ _        _ _   _     _                     _ 
#      / __| |_  ___| | | (_)_ _| |_ ___ _ _ _ _  __ _| |
#      \__ \ ' \/ -_) | | | | ' \  _/ -_) '_| ' \/ _` | |
#      |___/_||_\___|_|_| |_|_||_\__\___|_| |_||_\__,_|_|
#       / _|___ _ _ __ ___ ___                           
#      |  _/ _ \ '_/ _/ -_|_-<                           
#      |_| \___/_| \__\___/__/                           
#                                                       
# Internal forceson a material point of a shell element.      
class ShellControlVars(ControlVarsBase, ShellMaterialInternalForces):
    '''3D structural mechanics internal forces control vars.

    :ivar CF: capacity factor (efficiency) (defaults to -1.0; 
              CF<1.0 -> Ok; CF>1.0 -> KO).
    '''
    def __init__(self, combName= 'nil', CF= -1.0, n1= 0.0, n2= 0.0, n12= 0.0, m1= 0.0, m2= 0.0, m12= 0.0, q13= 0.0, q23= 0.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param n1: axial force parallel to axis 1.
        :param n2: axial force parallel to axis 2.
        :param n12: in plane shear force.
        :param m1: bending moment parallel to axis 1.
        :param m2: bending moment parallel to axis 2.
        :param m12: twisting stress resultant.
        :param q13: out of plane shear force normal to axis 1 and parallel to axis 3.
        :param q23: out of plane shear force normal to axis 1 and parallel to axis 3.
        '''
        ControlVarsBase.__init__(self, combName)
        ShellMaterialInternalForces.__init__(self, n1= n1, n2= n2, n12= n12, m1= m1, m2= m2, m12= m12, q13= q13, q23= q23)
        self.CF= CF # Capacity factor or efficiency

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= ControlVarsBase.getDict(self)
        retval.update(ShellMaterialInternalForces.getDict(self))
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.

        :param dct: dictionary containing the values of the object members.
        '''
        ControlVarsBase.setFromDict(self, dct)
        ShellMaterialInternalForces.setFromDict(self, dct)

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
