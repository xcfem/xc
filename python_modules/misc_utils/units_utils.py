# -*- coding: utf-8 -*-
from math import floor
from fractions import Fraction

inchToMeter= 25.4e-3
footToMeter= 0.3048 # feet to meters.
poundToN= 4.4482216282509 # pound-force to Newton.
kipToN= 1000*poundToN # kip to Newton.
kipTokN= poundToN
psiToPa= 6894.7572932 # pounds by square inch to Pascals.
ksiToPa= 1000*psiToPa # a thousand pounds by square inch to Pascals.
psfToPa= 0.047880258888889e3 # pounds by square foot to Pascals.

 
def convert_inches(n):
    '''Return (feet,inches,inches' fraction) equivalent to number n of inches 
    given as parameter, where n is a real number.
    '''
    feet, r = int(floor(n) / 12), n % 12
    inches  = int(floor(r))
    numer   = int(round(32 * (r - inches)))
     
    return feet, inches, Fraction(numer, 32)
 
# the rest is just pretty-printing
 
def show_inches(inches, fractions):
    '''Pretty printing of (inches,fractions) given as parameter'''
    def pluralize_inches(i):
        if i == 1:
            return str(i) + " inch"
        else:
            return str(i) + " inches"
     
    if inches and fractions:
        return str(inches) + ' and ' + str(fractions) + ' inches'
    elif inches or fractions: 
        return pluralize_inches(inches or fractions)
    else: return ''
 
def feet_and_inches(n):
 
    def pluralize_feet(ft):
        if not ft:
            return ''
        elif ft == 1:
            return str(ft) + ' foot'
        else:
            return str(ft) + ' feet'
     
    if not n:
        return "0 feet 0 inches"
    else:
        feet, inches, fractions = convert_inches(n)
        ft = pluralize_feet(feet)
        i  = show_inches(inches, fractions)
        return ft + (ft and i and ' ') + i

def roundLst(lst,decpl):
    '''return a list with the values of 'lst' rounded to 'decpl' 
    decimal places'''
    retval=[]
    for i in lst:
        retval.append(round(i,decpl))
    return retval
