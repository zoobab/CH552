
/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 Time ��ʼ������ʱ������������ֵ��T2��׽���ܿ���������
                       ��ʱ���жϺ���          		 		   
*******************************************************************************/
#include "CH554.h"                                                  
#include "common.h"
#include "Timer.h" 
#include "stdio.h"
#include "USB.h"

UINT8 	Tim_3ms;

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : mTimer_x_ModInit(UINT8 x ,UINT8 mode)
* Description    : CH554��ʱ������xģʽ����
* Input          : UINT8 mode,Timerģʽѡ��
                   0��ģʽ0��13λ��ʱ����TLn�ĸ�3λ��Ч
                   1��ģʽ1��16λ��ʱ��
                   2��ģʽ2��8λ�Զ���װ��ʱ��
                   3��ģʽ3������8λ��ʱ��  Timer0
                   3��ģʽ3��Timer1ֹͣ									 
* Output         : None
* Return         : �ɹ�  SUCCESS
                   ʧ��  FAIL
*******************************************************************************/
UINT8 mTimer_x_ModInit(UINT8 x ,UINT8 mode)
{
    if(x == 0){TMOD = TMOD & 0xf0 | mode;}
    else if(x == 1){TMOD = TMOD & 0x0f | (mode<<4);}
    else if(x == 2){RCLK = 0;TCLK = 0;CP_RL2 = 0;}                               //16λ�Զ����ض�ʱ��
    else return FAIL;
    return SUCCESS;
}

/*******************************************************************************
* Function Name  : mTimer_x_SetData(UINT8 x,UINT16 dat)
* Description    : CH554Timer0 TH0��TL0��ֵ
* Input          : UINT16 dat;��ʱ����ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer_x_SetData(UINT8 x,UINT16 dat)
{
    UINT16 tmp;
    tmp = 65536 - dat;	
	if(x == 0){TL0 = tmp & 0xff;TH0 = (tmp>>8) & 0xff;}
	else if(x == 1){TL1 = tmp & 0xff;TH1 = (tmp>>8) & 0xff;}
	else if(x == 2)
	{
		RCAP2L = TL2 = tmp & 0xff;                                               //16λ�Զ����ض�ʱ��
		RCAP2H = TH2 = (tmp>>8) & 0xff;
    }                                                 
}

/*******************************************************************************
* Function Name  : CAP2Init(UINT8 mode)
* Description    : CH554��ʱ������2 T2EX���Ų�׽���ܳ�ʼ��
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP2Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;	
    C_T2  = 0;
    EXEN2 = 1; 
    CP_RL2 = 1;                                                                //����T2ex�Ĳ�׽����
    T2MOD |= mode << 2;                                                        //���ز�׽ģʽѡ��
}

/*******************************************************************************
* Function Name  : CAP1Init(UINT8 mode)
* Description    : CH554��ʱ������2 T2���Ų�׽���ܳ�ʼ��T2
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP1Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 1;
    C_T2 = 0;
    T2MOD = T2MOD & ~T2OE | (mode << 2) | bT2_CAP1_EN;                         //ʹ��T2���Ų�׽����,���ز�׽ģʽѡ��
}

#ifdef T0_INT
/*******************************************************************************
* Function Name  : mTimer0Interrupt()
* Description    : CH554��ʱ������0��ʱ�������жϴ�������
*******************************************************************************/
void	mTimer0Interrupt( void ) interrupt INT_NO_TMR0 using 1                //timer0�жϷ������,ʹ�üĴ�����1
{                                                                           //��ʽ3ʱ��TH0ʹ��Timer1���ж���Դ
    SCK = ~SCK;
//     mTimer_x_SetData(0,0x0000);                                          //���Զ����ط�ʽ�����¸�TH0��TL0��ֵ      
}
#endif

#ifdef T1_INT
/*******************************************************************************
* Function Name  : mTimer1Interrupt()
* Description    : CH554��ʱ������0��ʱ�������жϴ�������
*******************************************************************************/
void	mTimer1Interrupt( void ) interrupt INT_NO_TMR1 using 2                //timer1�жϷ������,ʹ�üĴ�����2
{                                                                           //��ʽ3ʱ��Timer1ֹͣ
    SCK = ~SCK;
//     mTimer_x_SetData(1,0x0000);                                          //���Զ����ط�ʽ�����¸�TH1��TL1��ֵ      
}
#endif



/*******************************************************************************
* Function Name  : mTimer2Interrupt()
* Description    : CH554��ʱ������0��ʱ�������жϴ�������
*******************************************************************************/
#ifdef T2_INT
void	mTimer2Interrupt(void) interrupt INT_NO_TMR2 using 3                //timer2�жϷ������,ʹ�üĴ�����3
{                                                                             
    mTimer2RunCTL( 0 );                                                     //�ض�ʱ��
#ifdef  T2_CAP   
    if(EXF2)                                                                //T2ex��ƽ�仯�ж��жϱ�־
    {
        SCK = ~SCK;                                                         //P17��ƽָʾ���
        Cap[FLAG++] = RCAP2;                                                //T2EX
        printf("RCAP2 %04x  \n",RCAP2);                                  			
        EXF2 = 0;                                                           //���T2ex��׽�жϱ�־		
    }
    if(CAP1F)                                                               //T2��ƽ��׽�жϱ�־
    {
        Cap[FLAG++] = T2CAP1;                                               //T2;	  	
        printf("T2CAP1 %04x  \n",T2CAP1);				
        CAP1F = 0;                                                          //���T2��׽�жϱ�־
    }
#endif	
    if(TF2)
    {     
		Tim_3ms++;
		TF2 = 0;                                                             //��ն�ʱ��2����ж�	
    }
    mTimer2RunCTL(1);                                                      //����ʱ��   
}
#endif