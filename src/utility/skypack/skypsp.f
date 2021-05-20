      SUBROUTINE SKYPSP (DGPNT,IFIRST,ILAST,INDEX,JMIN,MCOLS,
     &                   MROWS,NCOLS,A,WORK,SHAPE)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYPSP copies a two dimensional array                            *
C*    into the skyline stored array                                    *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    DGPNT  (aii) : addresses of the diagonal coefficients            *
C*    IFIRST (sii) : index of the first row in the submatrix           *
C*    ILAST  (sii) : index of the last  row in the submatrix           *
C*    INDEX  (aio) : indexes of the columns copied into (WORK)         *
C*    JMIN   (sii) : index of the first column of (A)                  *
C*    MCOLS  (sii) : maximum number of columns stored in (WORK)        *
C*    MROWS  (sii) : maximum number of rows    stored in (WORK)        *
C*    NCOLS  (sio) : number of columns copied  to (WORK)               *
C*    A      (ari) : block of the skyline matrix                       *
C*    WORK   (arw) : two dimensional work array                        *
C*    SHAPE  (sci) : type of the submatrix to be copied                *
C*                   'R' = rectangular                                 *
C*                   'T' = triangular                                  *
C*                                                                     *
C*  - SUBPROGRAM :                                                     *
C*    SKYPRI                                                           *
C*                                                                     *
C*  - INTRINSIC FUNCTION :                                             *
C*    MAX0                                                             *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          DGPNT(*),IFIRST,ILAST,INDEX(*),
     &                 JMIN,MCOLS,MROWS,NCOLS
      DOUBLE PRECISION A(*),WORK(MROWS,MCOLS)
      CHARACTER        SHAPE*1
C
C==== local variables ==================================================
C
      INTEGER          I,IBELOW,IEND,J,JA,JFROW,K
C
C==== subprogram =======================================================
C
      INTEGER          SKYPRI
C
C==== intrinsic function ===============================================
C
      INTEGER          MAX0
C
C**** executable statements ********************************************
C
      DO 010 K = 1,NCOLS
C
C....... copy a column of (WORK) associated with INDEX(K) to (A) .......
C
         J      = INDEX(K)
         JA     = DGPNT(J) - J
         JFROW  = SKYPRI(DGPNT,J,JMIN)
         IBELOW = MAX0(IFIRST,JFROW)
C
         IF      ( SHAPE .EQ. 'R' ) THEN
                 IEND = ILAST
         ELSE IF ( SHAPE .EQ. 'T' ) THEN
                 IEND = J
         END IF
C
         DO 020 I = IBELOW,IEND 
            A(JA+I) = WORK(I-IFIRST+1,K)
  020    CONTINUE
C
  010 CONTINUE
C
      RETURN
C
C**** end of SKYPSP ****************************************************
C
      END
