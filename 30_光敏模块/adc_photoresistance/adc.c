#include "adc.h"
#include "timer.h"
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
 * 函数名称： adc_init
 * 功能描述： 初始化ADC1_CH4
 * 输入参数： 无
 * 返 回 值： 是否成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  jackson	      创建
 ***********************************************************************/
int adc_init(void)
{
    /* 1、先清零CFG寄存器 */
    ADC1->CFG = 0;

    /* 2、采用VREFH/VREFL的参考电压，12位ADC，ADICLK的时钟源 */
    ADC1->CFG |= (2 << 2) | (3 << 0);

    /* 3、关闭DMA，使能ADACK */
    ADC1->GC = 0;
    ADC1->GC |= 1 << 0;

    /* 4、校准ADC */
    ADC1->GS |= (1 << 2);                               // 清除CALF位，写1清零
    ADC1->GC |= (1 << 7);                               // 使能校准功能

    /* 5、校准完成之前GC寄存器的CAL位会一直为1，直到校准完成此位自动清零 */
    while((ADC1->GC & (1 << 7)) != 0) {                 // 如果GS寄存器的CALF位为1的话表示校准失败
        if((ADC1->GS & (1 << 2)) != 0) {
            return 2;
        }
    }

    /* 6、校准成功以后HS寄存器的COCO0位会置1 */
    if((ADC1->HS  & (1 << 0)) == 0) 
        return 1;
    if(ADC1->GS & (1 << 2)) return 1;

    return 0;
}
		  			 		  						  					  				 	   		  	  	 	  
/**********************************************************************
 * 函数名称： getadc_value
 * 功能描述： 获取ADC原始值
 * 输入参数： 无
 * 返 回 值： 是否成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  jackson	      创建
 ***********************************************************************/
unsigned int adc_value(void)
{
    ADC1->HC[0] = 0;					                // 清除
    ADC1->HC[0] |= (3 << 0);					        // 配置ADC通道3

    while((ADC1->HS & (1 << 0)) == 0);                  // 等待转换完成
    return ADC1->R[0];                                  // 返回ADC值
}

/**********************************************************************
 * 函数名称： adc_avr
 * 功能描述： 获取ADC平均值
 * 输入参数： 平均次数
 * 返 回 值： 平均值
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2020/03/16	     V1.0	  jackson	      创建
 ***********************************************************************/
unsigned short adc_avr(unsigned char temp)
{
	unsigned int temp_val = 0, i = temp;
	while(i--) {
		temp_val += adc_value();
		gpt2_chan1_delay_us(5000);
	}
	return temp_val / temp;
}
		  			 		  						  					  				 	   		  	  	 	  
