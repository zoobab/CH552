
/********************************** (C) COPYRIGHT *******************************
* File Name          : MainSlave.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 SPI�豸������ʾ������SPI�������������շ���������ȡ�豸������ȡ��
                       Ȼ���͸��豸
*******************************************************************************/
#include "..\Public\CH554.H"                                                   
#include "..\Public\Debug.H"
#include "SPI.H"
#include "stdio.h"
#include "string.h"

/*Ӳ���ӿڶ���*/
/******************************************************************************
ʹ��CH554 Ӳ��SPI�ӿ� 
         CH554        DIR       
         P1.4        <==>       SCS
         P1.5        <==>       MOSI
         P1.6        <==>       MISO
         P1.7        <==>       SCK
*******************************************************************************/


void main()
{
    UINT8 ret,i=0;
    CfgFsys( ); 
    mDelaymS(5);                                                               //�޸�ϵͳ��Ƶ�������Լ���ʱ�ȴ���Ƶ�ȶ�    
    
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("start ...\n");  
	
    SPISlvModeSet( );                                                          //SPI�ӻ�ģʽ����
// #if SPI0Interrupt
//     CH554SPIInterruptInit();                                                //SPI�жϳ�ʼ��
//     EA  = 1;                                                                //ʹ��ȫ���ж�
// #endif
    while(1)
    {   
        CH554SPISlvWrite(i);                                                   //SPI�ȴ�����������ȡ��                                                  
        mDelayuS(2);                                                          
        ret = CH554SPISlvRead();                                               //��ȡSPI����������
        if(ret != (i^0xff))
        {
		        printf("Err: %02X  %02X  \n",(UINT16)i,(UINT16)ret);               //����ȡ�����أ����ֵ���󣬴�ӡ��������					
        }
        mDelaymS(10);
        i = i+1;
        if((i%40)==0)                                                          //ÿ�ɹ�40�δ�ӡһ��
        {
            printf("success %02x\n",(UINT16)i);										
        }
    }
}