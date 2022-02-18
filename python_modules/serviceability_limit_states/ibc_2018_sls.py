# -*- coding: utf-8 -*-
''' Serviceability Limit States according to 2018 International Building Code (IBC).

https://codes.iccsafe.org/content/IBC2018P5/chapter-16-structural-design
'''

import sys
from misc_utils import log_messages as lmsg

def getDeflectionLimit(span= 1.0, memberType= 'Floor', memberSubType= None, deadLoad= False, liveLoad= True, snowLoad= False, windLoad= False):
    ''' Return the allowable deflection limit accordint to table 1604.3 of
        2018 International Building Code (IBC).

    :param span: span of the member.
    :param memberType: type of the member (Roof, Floor, ExteriorWall, InteriorWall, FarmMember or GreenhouseMember).
    :param memberSubType: secondary type of the member.
    :param deadLoad: if true the dead load is present in the load combination to check.
    :param liveLoad: if true the live load is present in the load combination to check.
    :param snowLoad: if true the snow load is present in the load combination to check.
    :param windLoad: if true the wind load is present in the load combination to check.
    '''
    retval= span/360.0
    if(memberType=='Roof'):
        if(memberSubType=='PlasterCeiling'):
            if(deadLoad and liveLoad):
                retval= span/240.0
            elif(liveLoad or snowLoad or windLoad):
                retval= span/360.0
        elif(memberSubType=='NonPlasterCeiling'):
            if(deadLoad and liveLoad):
                retval= span/180.0
            elif(liveLoad or snowLoad or windLoad):
                retval= span/240.0
        elif(memberSubType=='NoCeiling'):
            if(deadLoad and liveLoad):
                retval= span/120.0
            elif(liveLoad or snowLoad or windLoad):
                retval= span/180.0
    elif(memberType=='Floor'):
            if(deadLoad and liveLoad):
                retval= span/240.0
            elif(liveLoad or snowLoad or windLoad): # normally liveLoad only.
                retval= span/360.0
    elif(memberType=='ExteriorWall'):
        if(memberSubType=='PlasterFinish'):
            retval= span/360.0
        elif(memberSubType=='BrittleFinish'):
            retval= span/240.0
        elif(memberSubType=='FlexibleFinish'):
            retval= span/120.0
    elif(memberType=='InteriorWall'):
        if(memberSubType=='PlasterFinish'):
            retval= span/360.0
        elif(memberSubType=='BrittleFinish'):
            retval= span/240.0
        elif(memberSubType=='FlexibleFinish'):
            retval= span/120.0        
    elif(memberType=='FarmMember'):
        retval= span/180.0
    elif(memberType=='GreenhouseMember'):
        retval= span/120
    else:
        funcName= sys._getframe(0).f_code.co_name
        lmsg.error(funcName+"; memberType '"+str(memberType) + "' unknown.")
    return retval
            
