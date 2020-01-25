/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : fr_vad.c
  �� �� ��   : ����
  ��    ��   : л���� 58441
  ��������   : 2011��8��15��
  ����޸�   :
  ��������   : FR VADģ�麯��ʵ��
  �����б�   :
              FR_VadAdaptByPvadWithFac
              FR_VadAdaptByPvadWithMargin
              FR_VadAdaptThreshold
              FR_VadAverAcf
              FR_VadCompareFloat
              FR_VadCompareSpectral
              FR_VadComputeEnergy
              FR_VadComputeRav1
              FR_VadDecideAdapt
              FR_VadDeciVad
              FR_VadDetect
              FR_VadDetectPeriod
              FR_VadHangover
              FR_VadInit
              FR_VadPredictValues
              FR_VadSchurRecursion
              FR_VadUpdatePeriod
              FR_VadUpdateStep

  �޸���ʷ   :
  1.��    ��   : 2011��8��15��
    ��    ��   : л���� 58441
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "fr_codec.h"
#include "fr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : FR_VadInit
 ��������  : FR VADģ���ʼ��
             ��ʼ��ֵ�μ�Э��3GPP TS 46.032��Talbe3.1
 �������  : FR_VAD_STATE_STRU *pstVadState  --VAD�ṹ��ָ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��6��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadInit(FR_VAD_STATE_STRU *pstVadState)
{
    Word32                              swCnt = 0;

    /*��ʼ��rvad״̬����*/
    pstVadState->ashwRvad[0]            = FR_VAD_INIT_VALUE_RVAD0;
    pstVadState->ashwRvad[1]            = FR_VAD_INIT_VALUE_RVAD1;
    pstVadState->ashwRvad[2]            = FR_VAD_INIT_VALUE_RVAD2;

    CODEC_OpVecSet(&(pstVadState->ashwRvad[3]), 6, 0);

    /*��ʼ��rvad��������*/
    pstVadState->shwNormRvad            = FR_VAD_INIT_VALUE_NORM_RVAD;

    /*��ʼ�������ϵ��*/
    for (swCnt = 0; swCnt <= 26; swCnt++)
    {
        pstVadState->aswAcf[swCnt]     = 0;
    }

    /*��ʼ�������ϵ����ֵ*/
    for (swCnt = 0; swCnt <= 35; swCnt++)
    {
        pstVadState->aswSav0[swCnt]    = 0;
    }

    /*��ʼ��sacf�ӳٵ���*/
    pstVadState->shwPtSacf              = 0;

    /*��ʼ��sav0�ӳٵ���*/
    pstVadState->shwPtSav0              = 0;

    /*��ʼ���������ֵ*/
    pstVadState->swLastDm               = 0;

    /*��ʼ�������ۼ�ֵ*/
    pstVadState->shwOldLagCnt           = 0;
    pstVadState->shwVeryOldLagCnt       = 0;

    /*��ʼ��LTP����*/
    pstVadState->shwOldLag              = FR_VAD_INIT_VALUE_OLD_LAG;

    /*��ʼ��VAD�ж�����Ӧ��ֵ*/
    pstVadState->stThvad.shwExp         = FR_VAD_INIT_VALUE_THVAD_E;
    pstVadState->stThvad.shwMnts        = FR_VAD_INIT_VALUE_THVAD_M;

    /*��ʼ������Ӧ֡���ۼ�ֵ*/
    pstVadState->shwAdaptCnt            = 0;

    /*��ʼ������Burst֡���ۼ�*/
    pstVadState->shwBurstCnt            = 0;

    /*��ʼ��hangover֡���ۼ�*/
    pstVadState->shwHangCnt             = FR_VAD_INIT_VALUE_HANG_COUNT;

    /* ���ڼ���־��ʼ��Ϊ1����ZSP500��һ�� */
    pstVadState->shwPtch                = 1;
}

/*****************************************************************************
 �� �� ��  : FR_VadDetect
 ��������  : FR VAD�����������
 �������  : Word32                *pswAcf      --�����ϵ��ָ��
             Word16                shwScalauto  --�����ϵ��������ϵ��
             Word16                *pshwNc      --4���ӿ��LTP�ӳ�ָ��
             FR_VAD_STATE_STRU *pstVadState --VAD�ṹ��״̬
 �������  : FR_VAD_STATE_STRU *pstVadState --VAD�ṹ��״̬
 �� �� ֵ  : FR_VAD_STATUS_ENUM_UINT16          --VAD����־
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
FR_VAD_STATUS_ENUM_UINT16 FR_VadDetect(
                Word32                 *pswAcf,
                Word16                  shwScalauto,
                Word16                 *pshwNc,
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word16                              shwScalVad, shwNormRav1;
    Word16                              ashwRav1[FR_AR_LEN];
    Word16                              shwState;
    FR_VAD_STATUS_ENUM_UINT16           enVvad, enVadStatus;
    Word32                              aswAv0[FR_AR_LEN], aswAv1[FR_AR_LEN];
    FR_FLOAT_STRU                       stVad, stAcf0;

    /* Э��3GPP TS 46.032-- section 6.1 --������Ӧ�˲����������� */
    shwScalVad = FR_VadComputeEnergy(pswAcf, shwScalauto, &stAcf0, &stVad, pstVadState);

    /* Э��3GPP TS 46.032-- section 6.2 --������ACF��ֵ */
    FR_VadAverAcf(pswAcf, shwScalVad, aswAv0, aswAv1, pstVadState);

    /* Э��3GPP TS 46.032 -- section 6.3 -- ���������Ԥ��ֵ */
    FR_VadPredictValues(aswAv1, ashwRav1, &shwNormRav1);

    /* Э��3GPP TS 46.032 -- section 6.4 -- �ױȽϣ����������Ա�־ */
    shwState = FR_VadCompareSpectral(ashwRav1,
                                     shwNormRav1,
                                     aswAv0,
                                     pstVadState);

    /* Э��3GPP TS 46.032 -- 6.5 -- */
    /* FR_VadDetectPeriod(pVadState); �ú����ͺ���zsp500����һ�£�����Э�鲻һ�� */

    /* Э��3GPP TS 46.032 -- section 6.6 -- ��VAD�ж���ֵ���е��� */
    FR_VadAdaptThreshold(shwState,
                         0,
                         shwNormRav1,
                         stAcf0,
                         stVad,
                         ashwRav1,
                         pstVadState);

    /* Э��3GPP TS 46.032 -- section 6.7 -- ����VAD�о� */
    enVvad = FR_VadDeciVad(stVad, pstVadState->stThvad);

    /* Э��3GPP TS 46.032 -- section 6.8 -- ����hangover���� */
    enVadStatus = FR_VadHangover(enVvad,
                                 &(pstVadState->shwBurstCnt),
                                 &(pstVadState->shwHangCnt));

    /* Э��3GPP TS 46.032 -- section 6.9 -- ���������Ը��� */
    FR_VadUpdatePeriod(pshwNc, pstVadState);

    /* Э��3GPP TS 46.032 -- section 6.5 -- �������ڼ���־ */
    FR_VadDetectPeriod(pstVadState);

    return enVadStatus;
}

/*****************************************************************************
 �� �� ��  : FR_VadComputeEnergy
 ��������  : Э��3GPP TS 46.032-- section 6.1 --������Ӧ�˲�����������
 �������  : const Word32          aswAcf[]      --�����ϵ��
             Word16                shwScalauto   --�����ϵ��������ϵ��
             FR_FLOAT_STRU         *pstAcf0      --�ź�֡����
             FR_FLOAT_STRU         *pstVadState  --�˲�������
             FR_VAD_STATE_STRU *pstVadState  --VAD״̬�ṹ��
 �������  : ��
 �� �� ֵ  : Word16                              --����ϵ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 FR_VadComputeEnergy(
                const Word32            aswAcf[],
                Word16                  shwScalauto,
                FR_FLOAT_STRU          *pstAcf0,
                FR_FLOAT_STRU          *pstPvad,
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word32                              swCnt;
    Word16                              ashwSacf[FR_AR_LEN];
    Word16                              shwScalVad, shwNormAcf, shwNormProd;
    Word32                              swTemp = 0;

    /* �б�aswAcf[0]�Ƿ�Ϊ�㣻�������ϵ������aswAcf��ֵ���� */
    if (shwScalauto < 0)
    {
        shwScalVad       = 0;
    }
    else
    {
        shwScalVad       = shwScalauto;
    }

    /* ��ACF[0..8]���й�һ������ */
    shwNormAcf           = norm_l(aswAcf[0]);

    for(swCnt = 0; swCnt <= 8; swCnt++)
    {
        ashwSacf[swCnt] =(Word16)(L_shl(aswAcf[swCnt], shwNormAcf) >> 19);
    }

    /* �����ź�����acf0��ָ����β������ */
    pstAcf0->shwExp  = add(32, shl(shwScalVad,1));
    pstAcf0->shwExp  = sub(pstAcf0->shwExp, shwNormAcf);
    pstAcf0->shwMnts = shl(ashwSacf[0],3);

    /* �����˲�����pvad��ָ����β������ */
    pstPvad->shwExp  = add(pstAcf0->shwExp, 14);
    pstPvad->shwExp  = sub(pstPvad->shwExp, pstVadState->shwNormRvad);

    for (swCnt = 1; swCnt <= 8; swCnt++)
    {
        swTemp = L_add(swTemp, L_mult(ashwSacf[swCnt], pstVadState->ashwRvad[swCnt]));
    }

    swTemp     = L_add(swTemp, L_shr(L_mult(ashwSacf[0],pstVadState->ashwRvad[0]),1));

    if (swTemp <= 0)
    {
        swTemp = 1;
    }

    shwNormProd      = norm_l(swTemp);

    /* �����˲�����pvad��ֵ */
    pstPvad->shwExp  = sub(pstPvad->shwExp, shwNormProd);
    pstPvad->shwMnts = (Word16)(L_shl(swTemp, shwNormProd) >> 16);

    return shwScalVad;
}


/*****************************************************************************
 �� �� ��  : FR_VadAverAcf
 ��������  : Э��3GPP TS 46.032-- section 6.2 --������ACF��ֵ
 �������  : const Word32          aswAcf        --�����ϵ��
             const Word16          shwScalvad    --��������
             FR_VAD_STATE_STRU *pstVadState  --vad�ṹ��״̬
 �������  : Word32                aswAv0[]      --�����ϵ����ֵ0
             Word32                aswAv1[]      --�����ϵ����ֵ1
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadAverAcf(
                const Word32            aswAcf[],
                const Word16            shwScalvad,
                Word32                  aswAv0[],
                Word32                  aswAv1[],
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word32                              swCnt;
    Word16                              shwScal;
    Word32                              swTemp;

    /* ������������shwScal */
    shwScal            = sub(10, shl(shwScalvad,1));

    /* ����aswAv0[0..8] �� aswAv1[0..8] */
    for (swCnt = 0; swCnt < FR_AR_LEN; swCnt++)
    {
        swTemp         = L_shr(aswAcf[swCnt], shwScal);

        aswAv0[swCnt] = L_add(pstVadState->aswAcf[swCnt], swTemp);
        aswAv0[swCnt] = L_add(pstVadState->aswAcf[swCnt+9], aswAv0[swCnt]);
        aswAv0[swCnt] = L_add(pstVadState->aswAcf[swCnt+18], aswAv0[swCnt]);

        /* ��������±��Ƿ�Խ�� */
        if ((pstVadState->shwPtSacf + swCnt) >= 27)
        {
            return;
        }

        pstVadState->aswAcf[pstVadState->shwPtSacf + swCnt] = swTemp;

        aswAv1[swCnt] = pstVadState->aswSav0[pstVadState->shwPtSav0 + swCnt];

        pstVadState->aswSav0[pstVadState->shwPtSav0 + swCnt] = aswAv0[swCnt];
    }

    /* ����shwPtSacf */
    if (18 == pstVadState->shwPtSacf)
    {
        pstVadState->shwPtSacf = 0;
    }
    else
    {
        pstVadState->shwPtSacf = add(pstVadState->shwPtSacf, 9);
    }

    /* ����shwPtSav0 */
    if (27 == pstVadState->shwPtSav0)
    {
        pstVadState->shwPtSav0 = 0;
    }
    else
    {
        pstVadState->shwPtSav0 = add(pstVadState->shwPtSav0, 9);
    }
}


/*****************************************************************************
 �� �� ��  : FR_VadSchurRecursion
 ��������  : Э��3GPP TS 46.032 -- section 6.3.1 -- ���㷴��ϵ��
 �������  : Word32 aswAv1[]    --�����ϵ����ֵ1
 �������  : Word16 ashwVpar[]  --����ϵ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadSchurRecursion(Word32 aswAv1[], Word16 ashwVpar[])
{
    Word32                              i, j;
    Word16                              shwTemp;
    Word16                              ashwValueP[FR_AR_LEN], ashwValueK[FR_AR_LEN];
    Word16                              ashwSacf[FR_AR_LEN];

    /* ��������ϵ��Ϊ0��������ϵ����0�󷵻� */
    if (0 == aswAv1[0])
    {
        CODEC_OpVecSet(&ashwVpar[0], 9, 0);

        return;
    }

    /* ��aswAvl[0]�������ϵ�����й�һ������λ�����µ������ϵ��ashwSacf[] */
    shwTemp                 = norm_l(aswAv1[0]);

    for (i = 0; i <= 8; i++)
    {
        ashwSacf[i]         = (Word16)(L_shl(aswAv1[i], shwTemp) >> 16);
    }

    /* ��ʼ������ashwValueK[]��ashwValueP[]���ڵ����㷨 */
    for (i = 1; i <= 7; i++)
    {
        ashwValueK[9-i]     = ashwSacf[i];
    }

    CODEC_OpVecCpy(&ashwValueP[0], &ashwSacf[0], 9);

    /* ���㷴��ϵ�� */
    for (j = 1; j <= 8; j++)
    {
        if ((ashwValueP[0] < abs_s(ashwValueP[1])) || (0 == ashwValueP[0]))
        {
            CODEC_OpVecSet(&ashwVpar[j], (9 - j), 0);

            return;
        }

        ashwVpar[j]         = div_s(abs_s(ashwValueP[1]), ashwValueP[0]);

        if (ashwValueP[1] > 0)
        {
            ashwVpar[j]     = sub(0, ashwVpar[j]);
        }

        if (8 == j)
        {
            break;
        }

        /* Schur�����㷨 */
        ashwValueP[0]       = add(ashwValueP[0], mult_r(ashwValueP[1], ashwVpar[j]));

        for (i = 1; i <= (8-j); i++)
        {
            ashwValueP[i]   = add(ashwValueP[i+1], mult_r(ashwValueK[9-i], ashwVpar[j]));
            ashwValueK[9-i] = add(ashwValueK[9-i], mult_r(ashwValueP[i+1], ashwVpar[j]));
        }
    }
}

/*****************************************************************************
 �� �� ��  : FR_VadUpdateStep
 ��������  : Э��3GPP TS 46.032 -- section 6.3.2 -- �����˲���ϵ��ashwAav1[]
 �������  : Word16 ashwVpar[]   --����ϵ��
 �������  : Word16 ashwAav1[]   --�˲���ϵ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadUpdateStep(Word16 ashwVpar[], Word16 ashwAav1[])
{
    Word32                              i, m;
    Word16                              shwTemp;
    Word32                              aswCoef[9] = {0};
    Word32                              aswWork[9] = {0};

    /* ��ʼ��step-up �ݹ� */
    aswCoef[0]         = L_shl(16384, 15);

    aswCoef[1]         = L_shl(ashwVpar[1], 14);

    /* LPC�����˲� */
    for (m = 2; m <= 8; m++)
    {
        for (i = 1; i <= (m-1); i++)
        {
            shwTemp    = (Word16)(aswCoef[m-i] >> 16);
            aswWork[i] = L_add(aswCoef[i], L_mult(ashwVpar[m], shwTemp));
        }

        for (i = 1; i <= (m-1); i++)
        {
            aswCoef[i] = aswWork[i];
        }

        aswCoef[m]     = L_shl(ashwVpar[m],14);
    }

    /* ����aav1[0..8] */
    for (i = 0; i <= 8; i++)
    {
        ashwAav1[i]    = (Word16)L_shr(aswCoef[i],19);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : FR_VadComputeRav1
 ��������  : Э��3GPP TS 46.032 -- section 6.3.3 -- ���������Ԥ��ֵ
 �������  : Word16 ashwAav1[]     --�˲���ϵ��
 �������  : Word16 ashwRav1[]     --����ϵ��
             Word16 *pshwNormRav1  --����ϵ����������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadComputeRav1(Word16 ashwAav1[], Word16 ashwRav1[], Word16 *pshwNormRav1)
{
    Word32                              i, k;
    Word32                              aswWork[9];
    Word16                              shwNormRav1;

    /* ���㷴��ϵ���������� */
    for (i = 0; i <= 8; i++)
    {
        aswWork[i]     = 0;

        for (k = 0; k <= (8-i); k++)
        {
            aswWork[i] = L_add(aswWork[i], L_mult(ashwAav1[k], ashwAav1[k+i]));
        }
    }

    if (0 == aswWork[0])
    {
        /* ����ϵ������������0 */
        shwNormRav1    = 0;
    }
    else
    {
        /* ����ϵ������������aswWork[0]��һ�� */
        shwNormRav1    = norm_l(aswWork[0]);
    }

    /* ���㷴��ϵ�� */
    for (i = 0; i <= 8; i++)
    {
        ashwRav1[i]    =(Word16)(L_shl(aswWork[i], shwNormRav1) >> 16);
    }

    *pshwNormRav1      = shwNormRav1;
}

/*****************************************************************************
 �� �� ��  : FR_VadPredictValues
 ��������  : Э��3GPP TS 46.032 -- section 6.3 -- ���������Ԥ��ֵ
 �������  : Word32 aswAv1[]       --�����ϵ����ֵ1
 �������  : Word32 aswAv1[]       --�����ϵ����ֵ1
             Word16 ashwRav1[]     --�����Ԥ��ֵ
             Word16 *pshwNormrav1  --��������
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadPredictValues(Word32 aswAv1[], Word16 ashwRav1[], Word16 *pshwNormrav1)
{
    Word16                              ashwVpar[9], ashwAav1[9];

    /* Э��3GPP TS 46.032 -- section 6.3.1 -- ���㷴��ϵ�� */
    FR_VadSchurRecursion(aswAv1, ashwVpar);

    /* Э��3GPP TS 46.032 -- section 6.3.2 -- �����˲���ϵ��ashwAav1[] */
    FR_VadUpdateStep(ashwVpar, ashwAav1);

    /* Э��3GPP TS 46.032 -- section 6.3.3 -- ���������Ԥ��ֵ */
    FR_VadComputeRav1(ashwAav1, ashwRav1, pshwNormrav1);
}

/*****************************************************************************
 �� �� ��  : FR_VadCompareSpectral
 ��������  : Э��3GPP TS 46.032 -- section 6.4 -- �ױȽϣ����������Ա�־
 �������  : Word16                ashwRav1[]     --�����Ԥ��ֵ
             Word16                shwNormRav1    --�����Ԥ��ֵ��������
             Word32                aswAv0[]       --�����ϵ����ֵ0
             FR_VAD_STATE_STRU *pstVadState   --VAD״̬
 �������  : FR_VAD_STATE_STRU *pstVadState   --VAD״̬
 �� �� ֵ  : Word16    --�����Ա�־
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 FR_VadCompareSpectral(
                Word16                  ashwRav1[],
                Word16                  shwNormRav1,
                Word32                  aswAv0[],
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word32                              i;
    Word16                              shwStatus;
    Word16                              ashwSav0[9], shwShift, shwDivShift, shwTemp;
    Word32                              swSump, swTemp, swDm;

    /* ��һ��aswAv0[0..8] */
    if (0 == aswAv0[0])
    {
        CODEC_OpVecSet(&ashwSav0[0], 9, FR_VAD_MAX_VALUE_13BITS);
    }
    else
    {
        shwShift              = norm_l(aswAv0[0]);
        for (i = 0; i <= 8; i++)
        {
            ashwSav0[i]       = (Word16)(L_shl(aswAv0[i],shwShift-3) >> 16);
        }
    }

    /* ���㹫ʽ��������Ͳ���swSump */

    swSump                    = 0;

    for (i = 1; i <= 8; i++)
    {
        swSump                = L_add(swSump, L_mult(ashwRav1[i], ashwSav0[i]));
    }

    /* ȡswSump����ֵ����swTemp */
    swTemp                    = L_abs(swSump);

    /* �����������Ͳ������ĸ�ı�ֵswDm */
    if (0 == swTemp)
    {
        swDm                  = 0;
        shwShift              = 0;
    }
    else
    {
        ashwSav0[0]           = shl(ashwSav0[0],3);

        shwShift              = norm_l(swTemp);

        shwTemp               = (Word16)(L_shl(swTemp, shwShift) >> 16);

        if (ashwSav0[0] >= shwTemp)
        {
            shwDivShift       = 0;
            shwTemp           = div_s(shwTemp, ashwSav0[0]);
        }
        else
        {
            shwDivShift       = 1;
            shwTemp           = sub(shwTemp, ashwSav0[0]);
            shwTemp           = div_s(shwTemp, ashwSav0[0]);
        }

        if (1 == shwDivShift)
        {
            swDm              = FR_VAD_SET_VALUE_DM;
        }
        else
        {
            swDm              = 0;
        }

        swDm                  = L_shl(L_add(swDm, shwTemp), 1);

        if (swSump < 0)
        {
            swDm              = L_sub(0, swDm);
        }
    }

    /* ��һ��������swDm */
    swDm                      = L_shl(swDm, 14);
    swDm                      = L_shr(swDm, shwShift);
    swDm                      = L_add(swDm, L_shl(ashwRav1[0],11));
    swDm                      = L_shr(swDm, shwNormRav1);

    /* ���㵱ǰ֡swDm����֡swLastDm��ֵ�����汾֡swDm */
    swTemp                    = L_sub(swDm, pstVadState->swLastDm);
    pstVadState->swLastDm     = swDm;

    if (swTemp < 0)
    {
        swTemp                = L_sub(0, swTemp);
    }

    swTemp                    = L_sub(swTemp, 3277);

    /* ���������Ա�־ */
    shwStatus                 = (swTemp < 0) ? 1 : 0;

    return shwStatus;
}

/*****************************************************************************
 �� �� ��  : FR_VadDetectPeriod
 ��������  : Э��3GPP TS 46.032 -- section 6.5 -- �������ڼ���־
 �������  : FR_VAD_STATE_STRU *pstVadState   --VAD�ṹ��״̬
 �������  : FR_VAD_STATE_STRU *pstVadState   --VAD�ṹ��״̬
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadDetectPeriod(FR_VAD_STATE_STRU *pstVadState)
{
    Word16                              shwTemp;
    Word16                              shwPtch;

    /* ���������ۼ�ֵ */
    shwTemp = add(pstVadState->shwOldLagCnt, pstVadState->shwVeryOldLagCnt);

    /* ���������ۼ�ֵ��С�������ڼ���־ */
    if (shwTemp >= 4)
    {
        shwPtch               = 1;
    }
    else
    {
        shwPtch               = 0;
    }

    pstVadState->shwPtch      = shwPtch;
}


/*****************************************************************************
 �� �� ��  : FR_VadAdaptThreshold
 ��������  : Э��3GPP TS 46.032 -- section 6.6 -- ��VAD�ж���ֵ���е���
 �������  : Word16                shwSpecStat   --Ƶ���ȶ��Ա�־
             Word16                shwTone       --tone����־
             Word16                shwNormRav1   --����ϵ����������
             FR_FLOAT_STRU         stAcf0        --�ź�����
             FR_FLOAT_STRU         stPvad        --�˲�������
             Word16                ashwRav1[]    --����ϵ��
             FR_VAD_STATE_STRU *pstVadState  --VAD״̬
 �������  : FR_VAD_STATE_STRU *pstVadState  --VAD״̬
 �� �� ֵ  : void
 ���ú���  : FR_VadCompareFloat
             FR_VadAdaptByPvadWithFac
             FR_VadAdaptByPvadWithMargin
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadAdaptThreshold(
                Word16                  shwSpecStat,
                Word16                  shwTone,
                Word16                  shwNormRav1,
                FR_FLOAT_STRU           stAcf0,
                FR_FLOAT_STRU           stPvad,
                Word16                  ashwRav1[],
                FR_VAD_STATE_STRU      *pstVadState)
{
    Word16                              shwComp;
    FR_COMPARE_ENUM_UINT16              uhwComp;
    FR_FLOAT_STRU                       stThesh;

    stThesh.shwExp                      = FR_VAD_THRESH_E_PTH;
    stThesh.shwMnts                     = FR_VAD_THRESH_M_PTH;

    /* �Ƚ��ź�������pth����ֵ��С */
    uhwComp = FR_VadCompareFloat(stAcf0, stThesh);
    if (FR_COMPARE_SMALL == uhwComp)
    {
        pstVadState->stThvad.shwExp    = FR_VAD_THRESH_E_PLEV;
        pstVadState->stThvad.shwMnts   = FR_VAD_THRESH_M_PLEV;
        return;
    }

    /* �ж��Ƿ���Ҫ��ֵ���� */
    shwComp = FR_VadDecideAdapt(pstVadState->shwPtch, shwSpecStat, shwTone);
    if (1 == shwComp)
    {
        pstVadState->shwAdaptCnt        = 0;  /* ����Ӧ֡���ۼ���0 */
        return;
    }

    pstVadState->shwAdaptCnt            = add(pstVadState->shwAdaptCnt, 1);
    if (pstVadState->shwAdaptCnt <= 8)
    {
        return;
    }

    /* ���㵱ǰVAD�ж���ֵ */
    pstVadState->stThvad.shwMnts        = sub(pstVadState->stThvad.shwMnts,
                                              shr(pstVadState->stThvad.shwMnts, 5));
    if ( pstVadState->stThvad.shwMnts < 16384)
    {
        pstVadState->stThvad.shwMnts    = shl(pstVadState->stThvad.shwMnts,1);
        pstVadState->stThvad.shwExp     = sub(pstVadState->stThvad.shwExp, 1);
    }

    /* �����˲���������Ȩ���е��� */
    FR_VadAdaptByPvadWithFac(stPvad, &pstVadState->stThvad);

    /* �����˲����������ӱ�Ե����ֵ���е��� */
    FR_VadAdaptByPvadWithMargin(stPvad, &pstVadState->stThvad);

    /* ���������ϵ�� */
    pstVadState->shwNormRvad            = shwNormRav1;

    CODEC_OpVecCpy(&(pstVadState->ashwRvad[0]), &ashwRav1[0], 9);

    /* ��������Ӧ֡���ۼ�Ϊ9 */
    pstVadState->shwAdaptCnt            = 9;
}

/*****************************************************************************
 �� �� ��  : FR_VadCompareFloat
 ��������  : �Ƚϸ���x�븡��y��С����x<y�򷵻�1
 �������  : FR_FLOAT_STRU stFloatx  --����x
             FR_FLOAT_STRU stFloaty  --����y
 �������  : ��
 �� �� ֵ  : Word16    --�Ƚϱ�־
 ���ú���  :
 ��������  : FR_VadAdaptThreshold

 �޸���ʷ      :
  1.��    ��   : 2011��5��6��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
FR_COMPARE_ENUM_UINT16 FR_VadCompareFloat(FR_FLOAT_STRU stFloatx, FR_FLOAT_STRU stFloaty)
{
    FR_COMPARE_ENUM_UINT16                              enComp = FR_COMPARE_BIG;

    /* ���stPloatx��ָ��С��stPloaty��ָ�����Ƚϱ�־��1 */
    if (stFloatx.shwExp < stFloaty.shwExp)
    {
        enComp     = FR_COMPARE_SMALL;
    }

    /* ���stPloatx��ָ������stPloaty��ָ����stPloatx��β��С��stPloaty��β�����Ƚϱ�־��1 */
    if (stFloatx.shwExp == stFloaty.shwExp)
    {
        if (stFloatx.shwMnts < stFloaty.shwMnts)
        {
            enComp = FR_COMPARE_SMALL;
        }
    }

    return enComp;
}

/*****************************************************************************
 �� �� ��  : FR_VadDecideAdapt
 ��������  : �ж��Ƿ���Ҫ����VAD��ֵ����
 �������  : Word16 shwPtch      --���ڱ�־
             Word16 shwSpecStat  --���ȶ����Ա�־
             Word16 shwTone      --tone����־
 �������  : ��
 �� �� ֵ  : Word16      --�Ƿ���ҪVAD��ֵ������־
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��6��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
Word16 FR_VadDecideAdapt(Word16 shwPtch, Word16 shwSpecStat, Word16 shwTone)
{
    Word16                              shwComp = 0;

    if (1 == shwPtch)
    {
        shwComp = 1;
    }

    if (0 == shwSpecStat)
    {
        shwComp = 1;
    }

    if (1 == shwTone)
    {
        shwComp = 1;
    }

    return shwComp;
}

/*****************************************************************************
 �� �� ��  : FR_VadAdaptByPvadWithFac
 ��������  : �����˲���������Ȩ��VAD�ж���ֵ���е���
 �������  : FR_FLOAT_STRU stPvad       --�˲�������ֵ
             FR_FLOAT_STRU *pstThvad    --VAD�ж���ֵָ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��6��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadAdaptByPvadWithFac(FR_FLOAT_STRU stPvad, FR_FLOAT_STRU *pstThvad)
{
    Word32                              swTemp;
    FR_FLOAT_STRU                       stTemp;
    FR_COMPARE_ENUM_UINT16              uhwComp, uhwComp2;

    /* �����˲���������Ȩֵ */
    swTemp                    = L_add(stPvad.shwMnts, stPvad.shwMnts);
    swTemp                    = L_add(swTemp, stPvad.shwMnts);
    swTemp                    = L_shr(swTemp,1);
    stTemp.shwExp             = add(stPvad.shwExp, 1);

    if (swTemp > FR_VAD_MAX_VALUE_16BITS)
    {
        swTemp                = L_shr(swTemp,1);
        stTemp.shwExp         = add(stTemp.shwExp, 1);
    }

    stTemp.shwMnts            = (Word16)swTemp;

    /* �Ƚ���ֵ���˲���������Ȩֵ */
    uhwComp = FR_VadCompareFloat(*pstThvad, stTemp);

    /* ��shwCompΪ1ʱ ����pstThvad +( pstThvad /inc)��stPvad*fac����Сֵ */
    if (FR_COMPARE_SMALL == uhwComp)
    {
        swTemp                = L_add(pstThvad->shwMnts, shr(pstThvad->shwMnts, 4));

        if (swTemp > FR_VAD_MAX_VALUE_16BITS)
        {
            pstThvad->shwMnts = (Word16)L_shr(swTemp,1);
            pstThvad->shwExp  = add(pstThvad->shwExp,1);
        }
        else
        {
            pstThvad->shwMnts = (Word16)swTemp;
        }

        uhwComp2 = FR_VadCompareFloat(stTemp, *pstThvad);
        if (FR_COMPARE_SMALL == uhwComp2)
        {
            pstThvad->shwExp  = stTemp.shwExp;
            pstThvad->shwMnts = stTemp.shwMnts;
        }
    }
}

/*****************************************************************************
 �� �� ��  : FR_VadAdaptByPvadWithMargin
 ��������  : �����˲����������ӱ�Ե����ֵ��VAD�о���ֵ���е���
 �������  : FR_FLOAT_STRU stPvad      --�˲�������ֵ
             FR_FLOAT_STRU *pstThvad   --VAD�ж���ֵָ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��7��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadAdaptByPvadWithMargin(
                FR_FLOAT_STRU           stPvad,
                FR_FLOAT_STRU          *pstThvad)
{
    Word16                              shwTemp;
    Word32                              swTempL;
    FR_FLOAT_STRU                       stTemp;
    Word16                              shwComp = 0;

    /* ���� stPvad + margin */
    if (FR_VAD_THRESH_E_MARGIN == stPvad.shwExp)
    {
        swTempL                = L_add(stPvad.shwMnts, FR_VAD_THRESH_M_MARGIN);
        stTemp.shwMnts         = (Word16)L_shr(swTempL,1);
        stTemp.shwExp          = add(stPvad.shwExp, 1);
    }
    else
    {
        if (stPvad.shwExp > FR_VAD_THRESH_E_MARGIN)
        {
            shwTemp            = sub(stPvad.shwExp, FR_VAD_THRESH_E_MARGIN);
            shwTemp            = shr(FR_VAD_THRESH_M_MARGIN, shwTemp);
            swTempL            = L_add(stPvad.shwMnts, shwTemp);

            if (swTempL > FR_VAD_MAX_VALUE_16BITS)
            {
                stTemp.shwExp  = add(stPvad.shwExp, 1);
                stTemp.shwMnts = (Word16)L_shr(swTempL, 1);
            }
            else
            {
                stTemp.shwExp  = stPvad.shwExp;
                stTemp.shwMnts = (Word16)swTempL;
            }
        }
        else
        {
            shwTemp            = sub(FR_VAD_THRESH_E_MARGIN, stPvad.shwExp);
            shwTemp            = shr(stPvad.shwMnts, shwTemp);
            swTempL            = L_add(FR_VAD_THRESH_M_MARGIN, shwTemp);

            if (swTempL > FR_VAD_MAX_VALUE_16BITS)
            {
                stTemp.shwExp  = add(FR_VAD_THRESH_E_MARGIN, 1);
                stTemp.shwMnts = (Word16)L_shr(swTempL,1);
            }
            else
            {
                stTemp.shwExp  = FR_VAD_THRESH_E_MARGIN;
                stTemp.shwMnts = (Word16)swTempL;
            }
        }
    }

    /* �ж� pstThvad �Ƿ���� stPvad + margin */
    if (pstThvad->shwExp > stTemp.shwExp)
    {
        shwComp = 1;
    }

    if (pstThvad->shwExp == stTemp.shwExp)
    {
        if (pstThvad->shwMnts > stTemp.shwMnts)
        {
            shwComp            = 1;
        }
    }

    if (1 == shwComp)
    {
        pstThvad->shwExp       = stTemp.shwExp;
        pstThvad->shwMnts      = stTemp.shwMnts;
    }
}

/*****************************************************************************
 �� �� ��  : FR_VadDeciVad
 ��������  : Э��3GPP TS 46.032 -- section 6.7 -- ����VAD�о�
 �������  : FR_FLOAT_STRU stPvad       --�˲�������
             FR_FLOAT_STRU stThvad      --VAD�ж���ֵ
 �������  : ��
 �� �� ֵ  : FR_VAD_STATUS_ENUM_UINT16  --VAD�����ж���־
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
FR_VAD_STATUS_ENUM_UINT16 FR_VadDeciVad(
                FR_FLOAT_STRU           stPvad,
                FR_FLOAT_STRU           stThvad)
{

    FR_VAD_STATUS_ENUM_UINT16           enVadStatus = FR_VAD_STATUS_NOT;

    /* �˲������������ж���ֵʱ�ж���־��1*/
    if (stPvad.shwExp >  stThvad.shwExp)
    {
        enVadStatus                     = FR_VAD_STATUS_YES;
    }

    if (stPvad.shwExp == stThvad.shwExp)
    {
        if (stPvad.shwMnts > stThvad.shwMnts)
        {
            enVadStatus                 = FR_VAD_STATUS_YES;
        }
    }

    return enVadStatus;
}

/*****************************************************************************
 �� �� ��  : FR_VadHangover
 ��������  : Э��3GPP TS 46.032 -- section 6.8 -- ����hangover����
 �������  : FR_VAD_STATUS_ENUM_UINT16 enVvad   --VAD�����ж����
             Word16 *pshwBurstCount             --����burst֡����
             Word16 *pshwHangCount              --hangover֡����
 �������  : Word16 *pshwBurstCount             --����burst֡����
             Word16 *pshwHangCount              --hangover֡����
 �� �� ֵ  : FR_VAD_STATUS_ENUM_UINT16          --VAD�ж���־
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
FR_VAD_STATUS_ENUM_UINT16 FR_VadHangover(
                FR_VAD_STATUS_ENUM_UINT16                   enVvad,
                Word16                                     *pshwBurstCount,
                Word16                                     *pshwHangCount)
{
    FR_VAD_STATUS_ENUM_UINT16           enVadStatus;
    Word16                              shwBurstCount, shwHangCount;

    shwBurstCount                       = *pshwBurstCount;
    shwHangCount                        = *pshwHangCount;

    if (FR_VAD_STATUS_YES == enVvad)
    {
        shwBurstCount                   = add(shwBurstCount, 1);                /* ����burst֡������1 */
    }
    else
    {
        shwBurstCount                   = 0;
    }

    if (shwBurstCount >= 3)
    {
        shwHangCount                    = 5;                                   /* hangover֡������5 */
        shwBurstCount                   = 3;
    }

    enVadStatus                         = enVvad;                               /* VAD�ж���־��ΪVAD�����ж���־ */
    if (shwHangCount >= 0)
    {
        enVadStatus                     = FR_VAD_STATUS_YES;                    /* VAD�ж���־��1 */
        shwHangCount                    = sub(shwHangCount, 1);
    }

    *pshwBurstCount                     = shwBurstCount;
    *pshwHangCount                      = shwHangCount;

    return enVadStatus;
}

/*****************************************************************************
 �� �� ��  : FR_VadUpdatePeriod
 ��������  : Э��3GPP TS 46.032 -- section 6.9 -- ���������Ը���
 �������  : const Word16 *pshwLags               --RPE-LTP�����õ�4���ӿ���ӳ�
             FR_VAD_STATE_STRU *pstVadState   --VAD�ṹ��״̬
 �������  : FR_VAD_STATE_STRU *pstVadState   --VAD�ṹ��״̬
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��5��3��
    ��    ��   : л���� 58441
    �޸�����   : �����ɺ���

*****************************************************************************/
void FR_VadUpdatePeriod(Word16 *pshwLags, FR_VAD_STATE_STRU *pstVadState)
{
    Word32                              i, j;
    Word16                              shwMinLag, shwMaxLag, shwSmalLag;
    Word16                              shwLagCount, shwTemp;

    shwLagCount                         = 0;

    /* 4���ӿ�ѭ�� */
    for (i = 0; i <= 3; i++)
    {
        /* ���������ӳ������ֵshwMaxLag����СֵshwMinLag */
        if (pstVadState->shwOldLag > pshwLags[i])
        {
            shwMinLag                   = pshwLags[i];
            shwMaxLag                   = pstVadState->shwOldLag;
        }
        else
        {
            shwMinLag                   = pstVadState->shwOldLag;
            shwMaxLag = pshwLags[i] ;
        }

        /* ����shwSmalLag*/
        shwSmalLag = shwMaxLag;

        for (j = 0; j <= 2; j++)
        {
            if (shwSmalLag >= shwMinLag)
            {
                shwSmalLag              = sub(shwSmalLag, shwMinLag);
            }
        }

        /* ����smallag��minlag��ֵ����smallag */
        shwTemp = sub(shwMinLag, shwSmalLag);
        if (shwTemp < shwSmalLag)
        {
            shwSmalLag                  = shwTemp;
        }

        if (shwSmalLag < 2)
        {
            shwLagCount                 = add(shwLagCount, 1);
        }

        /* ���浱ǰLTP�ӳ� */
        pstVadState->shwOldLag          = pshwLags[i];
    }

    /* ���½ṹ��״̬���������ۼ�ֵshwOldLagCnt��shwVeryOldLagCnt������֡vad��� */
    pstVadState->shwVeryOldLagCnt       = pstVadState->shwOldLagCnt;
    pstVadState->shwOldLagCnt           = shwLagCount;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

