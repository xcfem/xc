      SUBROUTINE SKYSDC (INRTIA,N,D)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYSDC evaluates the inertia of (A)                              *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    INRTIA (aio) : inertia of (A)                                    *
C*    N      (sii) : dimension of the matrix                           *
C*    D      (ari) : matrix (D) in (A)=(L)*(D)*(L')                    *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          INRTIA(3),N
      DOUBLE PRECISION D(*)
C
C==== local variables ==================================================
C
      INTEGER          I,NNEG,NPLUS,NULL
C
C**** executable statements ********************************************
C
      NNEG  = 0
      NPLUS = 0
      NULL  = 0
C
      DO 010 I = 1,N 
         IF      ( D(I) .LT. 0.0D0 ) THEN
	         NNEG  = NNEG  + 1
         ELSE IF ( D(I) .EQ. 0.0D0 ) THEN
		 NULL  = NULL  + 1
	 ELSE
		 NPLUS = NPLUS + 1
	 ENDIF
  010 CONTINUE
C
      INRTIA(1) = NNEG
      INRTIA(2) = NULL
      INRTIA(3) = NPLUS
C
      RETURN
C
C**** end of SKYSDC ****************************************************
C
      END
