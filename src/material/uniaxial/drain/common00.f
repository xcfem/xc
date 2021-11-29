        subroutine fill00 (param, hstv, stateP)
        
c .....................................................................
c     fill00 - a fortran subroutine to fill the named common block
c              used by Drain subroutine RESP00 for the OpenSees
c              class DrainHardeningMaterial
c
c     written:  MHS
c     created:  June 2001
c .....................................................................
        
        implicit none
        integer, parameter :: dp = selected_real_kind(15, 50)
        real(dp) ::    param(*),hstv(*),stateP(*)

        real(dp)         E,sigY,Hiso,Hkin
        real(dp)         ep,alpha,kappa
        real(dp)         epsP,sigP,tangP
        real(dp)         tang
        
        include 'infel00.h'
        
c        Fill in the model input parameters
        E    = param(1)
        sigY = param(2)
        Hiso = param(3)
        Hkin = param(4)
        
c        Fill in the history variables
        ep    = hstv(1)
        alpha = hstv(2)
        kappa = hstv(3)
        
c        Fill in previous state information
        epsP  = stateP(1)
        sigP  = stateP(2)
        tangP = stateP(3)
        
        end
        
        
        
        subroutine get00 (hstv)
        
c .....................................................................
c     get00 - a fortran subroutine to get the named common block
c             data used by Drain subroutine RESP00 for the OpenSees
c             class DrainHardeningMaterial
c
c     written:  MHS
c     created:  June 2001
c .....................................................................

        implicit none
        integer, parameter :: dp = selected_real_kind(15, 50)
        real(dp)         hstv(*)
        
        real(dp)         E,sigY,Hiso,Hkin
        real(dp)         ep,alpha,kappa
        real(dp)         epsP,sigP,tangP
        real(dp)         tang
        
        include 'infel00.h'
        
c        Get the history variables
        hstv(1) = ep
        hstv(2) = alpha
        hstv(3) = kappa

        end
