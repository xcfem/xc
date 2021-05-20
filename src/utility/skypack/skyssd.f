      SUBROUTINE SKYSSD (LN,N,NX,D,X)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYSSD performs a division by the pivots in (L)*(D)*(L')*(X)=(Y) *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    LN (sii) : leading dimension of (X)                              *
C*    N  (sii) : dimension of the matrix                               *
C*    NX (sii) : number of right-hand sides                            *
C*    D  (ari) : reciprocal of the entries in (D)                      *
C*    X  (arb) : right-hand sides, LNxNX array                         *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          LN,N,NX
      DOUBLE PRECISION D(*),X(LN,NX)
C
C==== local variables ==================================================
C
      INTEGER          I,J
C
C**** executable statements ********************************************
C
      DO 010 I = 1,NX
         DO 020 J = 1,N  
            X(J,I) = X(J,I)*D(J)
  020    CONTINUE
  010 CONTINUE
C
      RETURN
C
C**** end of SKYSSD ****************************************************
C
      END
