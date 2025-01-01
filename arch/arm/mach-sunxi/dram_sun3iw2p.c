

#include <common.h>
#include <errno.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/dram.h>
#include <asm/io.h>

#include <spl.h>
#include <asm/arch/spl.h>
#include <asm/arch/sys_proto.h>
#include <linux/compiler.h>

void dram_enable_all_master(void);
void dram_disable_all_master(void);
void set_master_priority(void);
//void bit_delay_compensation_standby(struct dram_para_t *para);
unsigned int mctl_channel_init(unsigned char channel, __dram_para_t *para);
void ccm_set_pll_ddr1_sscg(__dram_para_t *para);
unsigned int ccm_set_pll_ddr1_clk(__dram_para_t *para);
unsigned int dram_get_size(void);
void mctl_clk_init(__dram_para_t *para);
void mctl_com_init(__dram_para_t *para);
void auto_set_timing_para(__dram_para_t *para);
unsigned int dram_autodetect_size(__dram_para_t *para);
unsigned int mctl_core_init(__dram_para_t *para);
unsigned int dram_simple_test(unsigned int dram_size, unsigned int len);
unsigned int do_dram_init(__dram_para_t *para);



static inline int ns_to_t(int ns, unsigned int freq)
{
	return ((ns*freq)/1000 + (((ns*freq)%1000 != 0)?1:0));
}

//from mctl_standby.c
void dram_enable_all_master(void)
{
    /* enable all master */
    writel(0xffffffff, MC_MAER);
    sunxi_delay(10);
}

//from mctl_standby.c
void dram_disable_all_master(void)
{
    /* disable all master except cpu/cpus */
    writel(0x1, MC_MAER);
    sunxi_delay(10);
}

void set_master_priority(void)
{
    // enable bandwidth limit windows and set windows size 1us
    writel(0x12b, MC_TMR);
    //dram_dbg_8("DRAM master priority setting ok.\n");
}



/*
	// FROM MCTL_STANDBY.C CALLED IN MCTL_CHANNEL_INIT
	maybe we don't need it as stated here for h616 https://lists.denx.de/pipermail/u-boot/2022-December/502166.html
	// code for h616 is https://github.com/iuncuim/u-boot/blob/t507-lpddr4/arch/arm/mach-sunxi/dram_sun50i_h616.c
void bit_delay_compensation_standby(struct dram_para_t *para)
{
    unsigned int reg_val, i, j;
    reg_val = readl(PGCR0);
    reg_val &= (~(0x1 << 26));
    writel(PGCR0, reg_val);

    for (i = 0; i < MAX_BYTE_NUM; i++)
    {
        for (j = 0; j < 11; j++)
        {
            writel((DATX0IOCR(j) + i * 0x80), reg[15 + j + (i * 11)]);	// DATX0IOCR - 0x01c63000 + 0x310
        }
    }

    for (i = 0; i < 31; i++)
    {
        writel(ACIOCR1(i), reg[60 + i]);								// ACIOCR1 - 0x01c63000 + 0x210
    }

    for (i = 0; i < MAX_BYTE_NUM; i++)
    {
        writel(DXnSDLR6(i), reg[95 + i]);								// DXnSDLR6 - 0x01c63000 + 0x33c 
    }

    reg_val = readl(PGCR0);
    reg_val |= (0x1 << 26);
    writel(PGCR0, reg_val);
}
*/

// ret: 0 - fail, 1 - ok
unsigned int mctl_channel_init(unsigned char channel, __dram_para_t *para) 
{
	unsigned int odt;
	unsigned int reg_val;
	unsigned int ret;

    /***********************************
    Function : Set Phase
     **********************************/	
	clrsetbits_le32(PGCR2, 0xf00, 0x300);
	
	reg_val=readl(PGCR2);
//	printf("PGCR2 is %x\n", reg_val);
	
	/***********************************
    Function : AC/DX IO Configuration
     **********************************/
//	printf("DRAMC read ODT type : %d (0: off  1: dynamic)\n", para->dram_odt_en);

	odt = ((~(para->dram_odt_en)) << 5) & 0x20;
	reg_val = readl(DXnGCR0(0)) & ~0x30;
	reg_val |= odt;
	if (para->dram_clk > 672) {
		reg_val &= ~0xf600;
		reg_val &= ~0x1e;
		reg_val |= 0x400;
	} else {				
    	reg_val &= ~0xf000;
		reg_val &= ~0x1e;
	}
	writel(reg_val, DXnGCR0(0));
	reg_val = readl(DXnGCR0(1)) & ~0x30;
	reg_val |= odt;
	if (para->dram_clk > 672) {
		reg_val &= ~0xf600;
		reg_val &= ~0x1e;
		reg_val |= 0x400;
	} else {				
    	reg_val &= ~0xf000;
		reg_val &= ~0x1e;
	}
	writel(reg_val, DXnGCR0(1));
//	printf("DXnGCR0 = %x\n", reg_val);
	clrsetbits_le32(ACIOCR0, 0x800, 2);
	
	/***********************************
    Function : AC/DX IO Bit Delay
     **********************************/
	// in mctl_standby here bit_delay_compensation_standby(para);
	
    /***********************************
    Function : DQS Gate Mode Choose
     **********************************/	
	if ( ((para->dram_tpr13 >> 2) & 3) == 1 ) { // dqs gating mode
		clrbits_le32(PGCR2, 0xc0);
		reg_val = readl(DQSGMR);
		reg_val &= ~0x107;
		writel(reg_val, DQSGMR);
//		printf("DRAM DQS gate is open.\n");
	} else {
		clrbits_le32(PGCR2, 0x40);
		setbits_le32(PGCR2, 0xc0);
//		printf("DRAM DQS gate is PD mode.\n");
	}
//	printf("DQGGMR:0x%08x\n", readl(DQSGMR));
	/***********************************
    Function : Training
     **********************************/
	reg_val = readl(DTCR);
	reg_val &= 0xf0000000;
	reg_val |= 0x01003087;
	writel(reg_val, DTCR);			// one rank
//	printf("DTCR:0x%08x\n", readl(DTCR));
	/* ZQ pad release */
	reg_val = readl(0x01c215f4);	//VDD_SYS_PWROFF_GATING
	reg_val = 0;					// in mctl_standy it's &= ~(0x1<<1))
	writel(reg_val, 0x01c215f4);	//VDD_SYS_PWROFF_GATING
	sunxi_delay(10);
	/* ZQ calibration */
	reg_val = readl(ZQCR);
	reg_val &= ~0xffff;
	reg_val |= ((para->dram_zq) & 0xffff);
	writel(reg_val, ZQCR);
	if ( ((para->dram_tpr13 >> 2) & 3) == 1) { // (((para->dram_tpr13 >> 2) & 3) - 1) is dqs_gating_mode
		reg_val = 0x52;
		writel(reg_val, PIR);
		reg_val = 0x53;
		writel(reg_val, PIR);
		//"GATE MODE PIR value is %x\n", reg_val
		do {
			reg_val = readl(PGSR0);
		} while ((reg_val & 1) != 1);
		sunxi_delay(10);
		if (para->dram_type	== 3) reg_val = 0x5a0;
		else reg_val = 0x520;	
	} else {
		if (para->dram_type == 3) reg_val = 0x1f2;
		else reg_val = 0x172;
	}
	writel(reg_val, PIR);
	reg_val |= 1;
	writel(reg_val, PIR);
//	printf("DRAM initial PIR value is %x\n", reg_val);
	sunxi_delay(10);
	do {
		reg_val = readl(PGSR0);
	} while ((reg_val & 1) != 1);

//	printf("ZQCR:0x%08x\n", readl(ZQCR));	
	//"[DEBUG_4]PGSR0 = 0x%x\n"
	/////////////////////////////////////////////////////////////////
	// START OF part from mctl_stanby
	/////////////////////////////////////////////////////////////////
//    reg_val = readl(PGCR3);
//    reg_val &= (~(0x3 << 25));
//    reg_val |= (0x2 << 25);
//    writel(reg_val, PGCR3);
//    sunxi_delay(10);
    /* entry self-refresh */
//    reg_val = readl(PWRCTL);
//    reg_val |= 0x1 << 0;
//    writel(reg_val, PWRCTL);

//    while (((readl(STATR) & 0x7) != 0x3))
        ;
    /* pad release */
//    reg_val = readl(VDD_SYS_PWROFF_GATING);	// VDD_SYS_PWROFF_GATING        (0xf1c21400 + 0x1f4)
//    reg_val &= ~(0x1 << 0);
//    writel(reg_val, VDD_SYS_PWROFF_GATING);	// VDD_SYS_PWROFF_GATING        (0xf1c21400 + 0x1f4)
//    sunxi_delay(10);
    /* exit self-refresh but no enable all master access */
//    reg_val = readl(PWRCTL);
//    reg_val &= ~(0x1 << 0);
//    writel(reg_val, PWRCTL);

//    while (((readl(STATR) & 0x7) != 0x1))
//        ;
//    sunxi_delay(15);
    /* training :DQS gate training */
//    if (((para->dram_tpr13 >> 2) & 3) == 1) //dqs_gating_mode
//    {
//        reg_val = readl(PGCR2);
//        reg_val &= ~(0x3 << 6);
//        writel(reg_val, PGCR2);
//        reg_val = readl(PGCR3);
//        reg_val &= (~(0x3 << 25));
//        reg_val |= (0x1 << 25);
//        writel(reg_val, PGCR3);
//        sunxi_delay(1);
//        reg_val = 0x401;
//        writel(reg_val, PIR);

//        while ((readl(PGSR0) & 0x1) != 0x1)
//            ;
//    }
	/////////////////////////////////////////////////////////////////
	// END OF part from mctl_stanby abscent here
	/////////////////////////////////////////////////////////////////
    /***********************************
    Function : Training Information
     **********************************/
    reg_val = readl(PGSR0);
	reg_val >>= 20;
	ret = 1;
	
	if (reg_val & 0xff) {
		ret = 0;
//		printf("Training Error - PGSR0:0x%x\n", reg_val);
		if (reg_val & 1) {
			printf("ZQ calibration error,check external 240 ohm resistor\n");
		}
	}
	
    /***********************************
    Function : Controller Setting
    **********************************/		
	do {
		reg_val = readl(STATR);
	} while ((reg_val & 1) != 1);
	setbits_le32(RFSHCTL0, 0x800000000);
	sunxi_delay(10);	
	clrbits_le32(RFSHCTL0, 0x800000000);
	sunxi_delay(10);
	setbits_le32(MC_CCCR, 0x800000000);
	sunxi_delay(10);
	clrbits_le32(PGCR3, 0x6000000);
	/***********************************
    DQS GATE debug
    **********************************/
	if (para->dram_type == 2) {
		if ( ((para->dram_tpr13 >> 2) & 3) == 1) { //dqs_gating_mode
			clrsetbits_le32(DXCCR, 0xc0, 0x40);
		}
	}
	return (ret);
}

// DRAM CLK Spread Spectrum Function
void ccm_set_pll_ddr1_sscg(__dram_para_t *para)
{
    unsigned int pll_ssc, pll_step;
    unsigned int ret, reg_val;
    struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
    /* Calculate PLL_STEP */
    pll_step = (para->dram_tpr13 >> 16) & 0xf; // r4
    if (pll_step >= 9) pll_step = 9;
    /* Calculate PLL_SSC */
    ret = (para->dram_tpr13 >> 20) & 0x7; // r0

    if (ret > 5) {	
    	pll_ssc = 0x19999 - (1 << pll_step); // 0x19999 <==> (8 << 17) / 10
    }
    else
    {
        if (ret == 0)
        {
            if ((para->dram_tpr13 & 0x400) != 0)
            {
                pll_ssc = 1 << pll_step;
            }
            else 
            {
                return;
            }
        }
        else
        {
        	pll_ssc = (ret << 18) / 10 - (1 << pll_step);
			pll_ssc = (pll_ssc / (1 << pll_step));
            pll_ssc = (pll_ssc * (1 << pll_step));
        }
    }
	pll_ssc = pll_ssc >> pll_step;
	/*Configure PLL_SSC and PLL_STEP */
	reg_val = readl(&ccm->dram_pll1_cfg);
	reg_val &= ~0x1ffff00f;
	reg_val |= (pll_ssc << 12) | pll_step;
	writel(reg_val, &ccm->dram_pll1_cfg);
	/*Update PLL Configuration*/
	reg_val = readl(&ccm->pll11_cfg);
	reg_val |= 0x40000000;
	writel(reg_val, &ccm->pll11_cfg);
	sunxi_delay(20);
	/*Enable Linear Frequency Modulation*/
	reg_val = readl(&ccm->dram_pll1_cfg);
	reg_val |= 0x80000000;
	writel(reg_val, &ccm->dram_pll1_cfg);
	/*Update PLL Configuration */
	reg_val = readl(&ccm->pll11_cfg);
	reg_val |= 0x40000000;
	writel(reg_val, &ccm->pll11_cfg);
	do {
		reg_val = readl(&ccm->pll11_cfg);
	} while ((reg_val & 0x40000000) != 0);
	sunxi_delay(20);
}	

// ret: pll clock in MHz
unsigned int ccm_set_pll_ddr1_clk(__dram_para_t *para)
{
    unsigned int reg_val;
    unsigned int div;
    struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	/* Disable PLL_DDR1 LOCK Function */
    clrbits_le32(&ccm->pll_lock_ctrl, 0x400);
    /*Calculate Divider */
    div = para->dram_clk / 12;
    if (div < 12)
    {
        div = 12;
    }
    /*Configure Divider */
    reg_val = readl(&ccm->pll11_cfg);
    reg_val &= ~0x7f00;
	reg_val &= ~0x3;
    reg_val |= ((div - 1) << 8);
	writel(reg_val, &ccm->pll11_cfg);
	/* Enable DDR1 PLL clock */
	setbits_le32(&ccm->pll11_cfg, 0x80000000);
    /* Validate PLL Configuration */
	setbits_le32(&ccm->pll11_cfg, 0x40000000);
	while ((readl(&ccm->pll11_cfg) & 0x40000000) != 0) //bit30 cleared - valid
		;
	/* Enable PLL_DDR1 LOCK Function */
    setbits_le32(&ccm->pll_lock_ctrl, 0x400);
	/* Poll PLL valid and locked */
    while ((readl(&ccm->pll11_cfg) & 0x10000000) != 0) //bit28 set - locked
		;
    /*Disable PLL_DDR1 LOCK Function */
    clrbits_le32(&ccm->pll_lock_ctrl, 0x400);
	sunxi_delay(10);
    /*Configure CLK Spread Spectrum */
    ccm_set_pll_ddr1_sscg(para);
    return 24 * div;
}

// ret: dram size
unsigned int dram_get_size(void) // return r0
{
	unsigned int dram_size = 0;
	unsigned int reg_val;
  
	reg_val = readl(MC_WORK_MODE);
	dram_size += (reg_val >> 4) & 0xf;
	dram_size += ((reg_val << 20) >> 28);
	dram_size -= 0xe;
	dram_size += ((reg_val << 28) >> 30);
	return (1 << dram_size);
}


void mctl_clk_init(__dram_para_t *para)
{
	unsigned int reg_val;
	unsigned int ret;
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	clrbits_le32(&ccm->mbus0_clk_cfg, 0x80000000);	// Turn Off MBUS CLK
	clrbits_le32(&ccm->mbus_reset, 0x80000000);		// MBUS Domain Reset
	clrbits_le32(&ccm->ahb_gate0, 0x4000);			// Turn Off AHB Domain CLK
	clrbits_le32(&ccm->bus_reset0_cfg, 0x4000);		// AHB Domain Reset
	clrbits_le32(&ccm->pll11_cfg, 0x80000000);		// Turn-off DDR1 PLL
	setbits_le32(&ccm->pll11_cfg, 0x40000000);		// Validate - What For ??????????????????
	clrbits_le32(&ccm->dram_clk_cfg, 0x80000000);	// DRAM Controller Reset
	sunxi_delay(10);

	ret = ccm_set_pll_ddr1_clk(para);				// returns ddr pll clock
	//clock_set_pll11(para->dram_clk, false);		// external function from clock_sun3iw2p
	
	reg_val = readl(&ccm->dram_clk_cfg);
	reg_val &= ~0x300000;							// DRAM Controller clk becomes DDR1_PLL
	reg_val |= 0x100000;							// Use Periph0(2x) 
	para->dram_clk = ret >> 1;						// ddr_pll / 2
	//printf("pll_ddr1 = %d MHz\n", ret);
	sunxi_delay(10);
	writel(reg_val, &ccm->dram_clk_cfg);			//
	sunxi_delay(10);
	reg_val |= 0x110000;							// Validate configuration
	writel(reg_val, &ccm->dram_clk_cfg);
	sunxi_delay(10);
	setbits_le32(&ccm->bus_reset0_cfg, 0x4000);		// Release AHB Domain Reset
	writel(0x4000, &ccm->bus_reset0_cfg);
	setbits_le32(&ccm->ahb_gate0, 0x4000);			// Turn On AHB Domain CLK
	
	dram_disable_all_master();						// Disable Master Access Right
	
	setbits_le32(&ccm->mbus_reset, 0x80000000);		// Release MBUS Domain Reset
	setbits_le32(&ccm->mbus0_clk_cfg, 0x80000000);	// Turn On MBUS Domain CLK
	setbits_le32(&ccm->dram_clk_cfg, 0x80000000);	// Release DRAM Controller Reset
	sunxi_delay(10);
	reg_val = 0x8000;
	writel(reg_val, CLKEN);							// Enable DRAM Controller CLK
	sunxi_delay(10);
}

void mctl_com_init(__dram_para_t *para)
{
	unsigned int reg_val;
	
	reg_val = readl(MC_WORK_MODE);
	reg_val &= ~0xff0000;
	reg_val &= ~0xf000;
	reg_val |= (para->dram_tpr13 << 14) & 0x80000;
	reg_val |= (para->dram_type << 16) & 0x70000;
	reg_val |= 0x400000;		
	reg_val |= 0x1000;
	writel(reg_val, MC_WORK_MODE);
	sunxi_delay(10);
	
	reg_val = readl(MC_WORK_MODE) & ~0xfff;
	reg_val |= (para->dram_para1 >> 10) & 4;
	reg_val |= ((para->dram_para1 & ~0xf) - 16) & 0xff;

	switch (para->dram_para1 & 0xf) {
		case 1:
			reg_val |= 0x700;
		break;	
		case 2:
			reg_val |= 0x800;
		break;
		case 4:
			reg_val |= 0x900;
		break;
		case 8:
			reg_val |= 0xA00;
		break;
		default:
			reg_val |= 0x600;
		break;
	}
	writel(reg_val, MC_WORK_MODE);
	//printf("MC_WORK_MODE is %x\n", reg_val);
	sunxi_delay(10);
	reg_val = 0x201;
	writel(reg_val, ODTMAP);
}



void auto_set_timing_para(__dram_para_t *para)
{
	unsigned int tccd;
	unsigned int trd2wr;
	unsigned int twr2rd;
	unsigned int trasmax;
	unsigned int tmod;
	unsigned int t_rdata_en;
	unsigned int mr0;
	unsigned int mr1;
	unsigned int mr2;
	unsigned int mr3;
	unsigned int tdinit3;
	unsigned int tdinit2;
	unsigned int tdinit1;
	unsigned int trtp;
	unsigned int tdinit0;
	unsigned int trfc;
	unsigned int txp;
	unsigned int twtr;
	unsigned int tras;
	unsigned int tfaw;
	unsigned char trc;
	unsigned int trrd;
	unsigned int trefi;
	unsigned int tcke;
	unsigned int tckesr;
	unsigned int twtp;
	unsigned int twr;
	unsigned int trp;
	unsigned int trcd;
	unsigned int tcksre;
	unsigned int tcksrx;
	unsigned int tcwl;
	unsigned int tcl;
	unsigned int wr_latency;
	unsigned int tmrd;
	
	unsigned int reg_val;

	
	if ((para->dram_tpr13 & 2) == 0) {
//		printf("Auto calculate timing parameter!\n");
		if (para->dram_type == 3) {
			tfaw = ns_to_t(50, para->dram_clk/2);				//var_70
			trrd = ns_to_t(10, para->dram_clk/2);
			if (trrd < 2) trrd = 2;				  				//var_78		
			trcd = ns_to_t(15, para->dram_clk/2);				//r11
			if (trcd >= 2) twr = trcd; else twr = 2;			//r9
			trp = trcd;											//r5
			trc = ns_to_t(53, para->dram_clk/2);				//var_74
			trtp = ns_to_t(8, para->dram_clk/2);				//r8->var_5c
			twtr = trtp;										//var_68
			txp = twtr;											//var_68
			tras = ns_to_t(38, para->dram_clk/2);				//var_6c
			trefi = ns_to_t(7800, para->dram_clk/2) >> 5; 		//var_7c
			trfc = ns_to_t(350, para->dram_clk/2);				//var_64
		} else if (para->dram_type == 2) {
			tfaw = ns_to_t(50, para->dram_clk / 2);				//var 70
			trrd = ns_to_t(10, para->dram_clk / 2);				//var_78
			trcd = ns_to_t(20, para->dram_clk / 2);				//r11
			trp = ns_to_t(15, para->dram_clk / 2);				//r5
			twr = trp;											//r9
			trc = ns_to_t(65, para->dram_clk / 2);				//var_74
			trtp = ns_to_t(8, para->dram_clk / 2);				//r8->var_5c
			twtr = trtp;										//var_5c
			txp = para->dram_type; 								//var_68
			tras = ns_to_t(45, para->dram_clk / 2);				//var_6c
			trefi = ns_to_t(7800, para->dram_clk / 2) >> 5; 	//var_7c changed from 7500
			trfc = ns_to_t(328, para->dram_clk / 2);			//var_64
		} else {
			tfaw = 0x10;		//var_70
			trrd = 3;			//var_78
			trcd = 6;			//r11
			trp = 6;			//r5
			twr = 8;			//r9
			trc = 0x14;			//var_74
			trtp = 3;			//r8->var_5c
			twtr = trtp;		//var_5c
			txp = 0xa;			//var_68
			tras = 0xe;			//var_6c
			trefi = 0x62;		//var_7c
			trfc = 0x80;		//var_64
		}
		//				 var_74   r11          var_78		   var_70         var_2c aka tccd
		para->dram_tpr0 = trc | (trcd << 6) | (trrd << 11) | (tfaw << 15) | 0x400000;
		//				var_6c	  r5			r9			var_5c			var_5c			var_68	
		para->dram_tpr1 = tras | (trp << 6) | (twr << 11) | (trtp << 15) | (twtr << 20) | (txp << 23);
		//				  var_7c   var_64
		para->dram_tpr2 = trefi | (trfc << 12);
		tccd = 2;
	} else {
//		printf("User defined timing parameters!\n");
		tfaw = (para->dram_tpr0 >> 15) & 0x3f;				 		//var_70
		trrd = (para->dram_tpr0 >> 11) & 0xf; 						//var_78
		trcd = (para->dram_tpr0 >> 6) & 0x1f; 						//r11
		trp = (para->dram_tpr1 >> 6) & 0x1f;						//r5
		trc = para->dram_tpr0 & 0x3f; 								//var_74
		trtp = (para->dram_tpr1 >> 20) & 7; 						//r8 aka var_5c
		txp = (para->dram_tpr1 >> 23) & 0x1f; 						//var_68
		twtr = trtp;												//var_5c
		tras = para->dram_tpr1 & 0x3f;								//var_6c
		trefi = para->dram_tpr2 & 0xfff;							//var_7c
		trfc = (para->dram_tpr2 >> 12) & 0x1ff;						//var_64
		trtp = (para->dram_tpr1 >> 15) & 0x1f;						//var_5c
		tccd = (para->dram_tpr0 >> 21) & 7;							//var_2c
		twr = (para->dram_tpr1 >> 11) & 0xf;						//r9
	}
	if (para->dram_type == 2) {
		if ((para->dram_clk/2) > 204) t_rdata_en = 2; else t_rdata_en = 1;		//var_44
		if ((para->dram_clk/2) > 204) mr0 = 0xe73; else mr0 = 0xa63;			//var_4c
		tdinit3 = para->dram_clk + 1;											//var_50
		twr2rd = twtr + 5;	 													//var_38
		tdinit0 = para->dram_clk * 200 + 1;										//var_60
		mr1 = para->dram_mr1;													//var_48
		tdinit2 = para->dram_clk * 200 + 1;										//var_54
		tcke = 3;																//LR
		trasmax = para->dram_clk / 30;											//var_3c
		tdinit1 = para->dram_clk * 400 / 1000 + 1;								//var_58
		if ((para->dram_clk/2) > 204) tcl = 4; else tcl = 3; 					//r3
		tmod = 0xc;																//var_40
		twtp = twr + 5; 														//r9
		tcksrx = 5;																//r12
		tcksre = 5;																//r12
		tckesr = 4; 															//r0
		tcwl = 3; 																//r8
		wr_latency = 1; 														//r2
		mr2 = 0;																//r1
		mr3 = 0;																//r1
		trd2wr = tcl;															//var_34
		tmrd = 2;																//r6
	} else if (para->dram_type == 3) {
		trasmax = para->dram_clk / 30;											//var_3c
		tdinit0 = para->dram_clk * 500 + 1;										//var_60
		tdinit1 = para->dram_clk * 360/1000 + 1;								//var_58
		tdinit2 = para->dram_clk * 200 + 1;										//var_54
		tckesr = 4;																//r0
		tdinit3 = para->dram_clk * 200 + 1;										//var_50
		twr2rd = twtr + 6;														//var_38
		mr1 = para->dram_mr1;													//var_48
		tcke = para->dram_type; 												//LR
		tmod = 0xc;																//var_40
		twtp = twr + 6; 														//r9
		trd2wr = 5;																//var_34
		tcksrx = 5;																//r12
		tcksre = 5;																//r12
		tmrd = 4;																//r6
		tcwl = 4;																//r8
		tcl = 6;																//r3
		wr_latency = 2;															//r2
		t_rdata_en = 4;															//var_44
		mr2 = 0x18;																//r1
		mr3 = 0x18;																//r1
		mr0 = 0x1c70;															//var_4c
	} else {
		twr2rd = 8;			//var_38
		trasmax = 0x1b;		//var_3c
		tmod = 6;			//var_40
		trd2wr = 4;			//var_34
		t_rdata_en = 1;		//var_44
		mr1 = 0;			//var_48
		mr0 = 0;			//var_4c
		tdinit3 = 0;		//var_50
		tdinit2 = 0;		//var_54
		tdinit1 = 0;		//var_58
		tdinit0 = 0;		//var_60
		tcke = 2;			//LR
		tckesr = 3;			//r0
		mr2 = 0;			//r1
		mr3 = 0;			//r1
		wr_latency = 1;		//r2
		tcl = 3;			//r3
		tmrd = 2;			//r6
		tcwl = 3;			//r8
		twtp = 0xc;			//r9
		tcksrx = 4;			//r12
		tcksre = 4;			//r12
	}
	
	if ((trtp + trp) < (tcl + 2)) trtp = (2 - trp) + tcl; 	//
	if ((para->dram_mr0 >> 16) == 0) para->dram_mr0 = mr0;	//var_4c
	if ((para->dram_mr1 >> 16) == 0) para->dram_mr1 = mr1;	//var_48 or 30
	if ((para->dram_mr2 >> 16) == 0) para->dram_mr2 = mr2;	//var_
	if ((para->dram_mr3 >> 16) == 0) para->dram_mr3 = mr3;	//var_
	writel(para->dram_mr0 & 0xffff, DRAM_MR0);
	writel(para->dram_mr1 & 0xffff, DRAM_MR1);
	writel(para->dram_mr2 & 0xffff, DRAM_MR2);
	writel(para->dram_mr3 & 0xffff, DRAM_MR3);

	writel( (para->dram_odt_en >> 4) & 3, LP3MR11);
	//		  var_6c  var_3c           var_70			r9
	reg_val = tras | (trasmax << 8) | (tfaw << 16) | (twtp << 24);
	writel(reg_val, DRAMTMG0);
	//		 var_74		  var_5c		var_68
	reg_val = trc | 10 | (trtp << 8) | (txp << 16);
	writel(reg_val, DRAMTMG1);
	//	     var_38 		var_34		    r3			r8
	reg_val = twr2rd | (trd2wr << 8) | (tcl << 16) | (tcwl << 24);
	writel(reg_val, DRAMTMG2);
	//		var_40		r6
	reg_val = tmod | (tmrd << 12);		// (tmrw<<16) = 0 => ignored here
	writel(reg_val, DRAMTMG3);
	//		  r5	var_78			var_2c			r11
	reg_val = trp | (trrd << 8) | (tccd << 16) | (trcd << 24);
	writel(reg_val, DRAMTMG4);
	//			LR		r0				r12				r12
	reg_val = tcke | (tckesr << 8) | (tcksre <<16) | (tcksrx << 24);
	writel(reg_val, DRAMTMG5);
	//
	reg_val = (readl(DRAMTMG8) & ~0xffff) | 0x6600 | 0x10;
	writel(reg_val, DRAMTMG8);
	//			r2							var_44
	reg_val = wr_latency | (0x1 << 8) | (t_rdata_en << 16) | (0x2 << 24);
	writel(reg_val, PITMG0);
	//			var_60		var_58
	reg_val = tdinit0 | (tdinit1 << 20);
	writel(reg_val, PTR3);
	//			var_54		var_50
	reg_val = tdinit2 | (tdinit3 << 20);
	writel(reg_val, PTR4);
	//		  var_64	var_7c
	reg_val = trfc | (trefi << 16);
	writel(reg_val, RFSHTMG);
}

//ret: 0 - ok, 1 - fail
unsigned int dram_autodetect_size(__dram_para_t *para)
{
	unsigned int i=0, j=0;
	unsigned int ret=1;
	mctl_clk_init(para);
	mctl_com_init(para);
	auto_set_timing_para(para);
	ret = mctl_channel_init(0, para);
	//struct sunxi_ccm_reg *const ccm =
	//	(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	/*
	printf("\n");
	printf("CPU PLL (0x%08x):0x%08x\n", &ccm->pll1_cfg, readl(&ccm->pll1_cfg));
	printf("CPU AXI:0x%x\n", readl(&ccm->cpu_axi_cfg));
	printf("Periph0 PLL (0x%08x):0x%08x\n", &ccm->pll6_cfg, readl(&ccm->pll6_cfg));
	printf("DDR1 Pll (0x%08x):0x%08x\n", &ccm->pll11_cfg, readl(&ccm->pll11_cfg));
	printf("PLL DDR AUX (0x%08x):0x%08x\n", &ccm->dram_pll_cfg, readl(&ccm->dram_pll_cfg));
	printf("DRAM Cfg (0x%08x):0x%08x\n", &ccm->dram_clk_cfg, readl(&ccm->dram_clk_cfg));
	printf("PLL DDR1 Cfg (0x%08x):0x%08x\n", &ccm->dram_pll1_cfg, readl(&ccm->dram_pll1_cfg));
	printf("MBUS rst (0x%08x):0x%08x\n", &ccm->mbus_reset, readl(&ccm->mbus_reset));
	printf("MBUS clk (0x%08x):0x%08x\n", &ccm->mbus0_clk_cfg, readl(&ccm->mbus0_clk_cfg));
	printf("AHB1/APB1 (0x%08x):0x%08x\n", &ccm->ahb1_apb1_div, readl(&ccm->ahb1_apb1_div));
	printf("ABP2 (0x%08x):0x%08x\n", &ccm->apb2_div, readl(&ccm->apb2_div));
	printf("Gating 0(0x%08x):0x%08x\n", &ccm->ahb_gate0, readl(&ccm->ahb_gate0));
	printf("Bus Reset 0(0x%08x):0x%08x\n", &ccm->bus_reset0_cfg, readl(&ccm->bus_reset0_cfg));
	printf("PLL Lock reg(0x%08x):...\n", &ccm->pll_lock_ctrl);

	printf("------\n");
	printf("MR0:0x%08x\n", readl(DRAM_MR0));
	printf("MR1:0x%08x\n", readl(DRAM_MR1));
	printf("MR2:0x%08x\n", readl(DRAM_MR2));
	printf("MR3:0x%08x\n", readl(DRAM_MR3));
	printf("------\n");
	
	printf("DRAMTMG0:0x%08x\n", readl(DRAMTMG0));
	printf("DRAMTMG1:0x%08x\n", readl(DRAMTMG1));
	printf("DRAMTMG2:0x%08x\n", readl(DRAMTMG2));
	printf("DRAMTMG3:0x%08x\n", readl(DRAMTMG3));
	printf("DRAMTMG4:0x%08x\n", readl(DRAMTMG4));
	printf("DRAMTMG5:0x%08x\n", readl(DRAMTMG5));
	printf("DRAMTMG6:0x%08x\n", readl(DRAMTMG6));
	printf("DRAMTMG7:0x%08x\n", readl(DRAMTMG7));
	printf("DRAMTMG8:0x%08x\n", readl(DRAMTMG8));
	printf("------\n");
	printf("PITMG0:0x%08x\n", readl(PITMG0));
	printf("PITMG1:0x%08x\n", readl(PITMG1));
	printf("PTR0:0x%08x\n", readl(PTR0));
	printf("PTR2:0x%08x\n", readl(PTR2));
	printf("PTR3:0x%08x\n", readl(PTR3));
	printf("PTR4:0x%08x\n", readl(PTR4));
	printf("RFSHTMG:0x%08x\n", readl(RFSHTMG));
	printf("ODTMAP:0x%08x\n", readl(ODTMAP));
	printf("------\n");
	printf("RFSHCTL0:0x%08x\n", readl(RFSHCTL0));
	printf("PGCR0:0x%08x\n", readl(PGCR0));
	printf("PGCR1:0x%08x\n", readl(PGCR1));
	printf("PGCR2:0x%08x\n", readl(PGCR2));
	printf("PGCR3:0x%08x\n", readl(PGCR3));
	printf("------\n");
	printf("MC_WORK_MODE is %x\n", readl(MC_WORK_MODE));
	printf("MC_R1_WORK_MODE is %x\n", readl(MC_R1_WORK_MODE));
	printf("MC_CCCR:0x%08x\n", readl(MC_CCCR));
	printf("MC_MAER:0x%08x\n", readl(MC_MAER));
	printf("MC_TMR:0x%08x\n", readl(MC_TMR));
	printf("VDD_SYS_PWROFF_GATING:0x%08x\n", readl(0x01c215f4));
	printf("ACIOCR0:0x%08x\n", readl(ACIOCR0));
	printf("VTFCR:0x%08x\n", readl(VTFCR));
	printf("ODTCFG:0x%08x\n", readl(ODTCFG));
	printf("CLKEN:0x%08x\n", readl(CLKEN));
	printf("DXCCR:0x%08x\n", readl(DXCCR));
	printf("MC_MCGCR:0x%08x\n", readl(MC_MCGCR));
	printf("MC_SWONF:0x%08x\n", readl(MC_SWONR));
	printf("MC_SWOFFR:0x%08x\n", readl(MC_SWOFFR));
	
	
	printf("ccm +20:0x%08x\n", readl(0x01c20020));
	printf("ccm +230:0x%08x\n", readl(0x01c20230));
	printf("ccm +5c:0x%08x\n", readl(0x01c2005c));
	printf("ccm +144:0x%08x\n", readl(0x01c20144));
	*/
	/* Timer AVS */
	/*
	printf("ccm +c8c:0x%08x\n", readl(0x01c20c8c));
	printf("ccm +c84:0x%08x\n", readl(0x01c20c84));
	printf("ccm +c80:0x%08x\n", readl(0x01c20c80));
	*/

	if (ret == 0) return 1; // Fail
	printf("\nDRAM autodetect size\n");

	printf("0\n");
	for (i=0;i<64;i++) {
		writel( (i%2)?(DRAM_BASE_ADDR + 4*i):(~(DRAM_BASE_ADDR + 4*i)), DRAM_BASE_ADDR + 4*i );
	}
	printf("1\n");
	clrsetbits_le32(MC_WORK_MODE, 0xf0c, 0x6f0);
	/*Row detect */
	for (i=11;i<16;i++) {
		for (j=0;j<64;j++) {
			if (readl(DRAM_BASE_ADDR + (1<<(i+11))) != readl((j%2)?~(DRAM_BASE_ADDR + j*4):(DRAM_BASE_ADDR + j*4))) {
				break;
			}
		}
		if (j == 63) break;
	}
	printf("2 %d\n", j);
	if (j != 63) return 1;
	para->dram_para1 = (para->dram_para1 & ~0xff0) | (i << 4); 
	sunxi_delay(10);
	clrsetbits_le32(MC_WORK_MODE, 0xffc, 0x6a4);
	/*Row size ? */
	for (i=0;i<64;i++) {
		if ( readl(0x80000000+0x800) != readl((i%2)?~(DRAM_BASE_ADDR + i*4):(DRAM_BASE_ADDR + i*4)) ) {
			i = 1;
			break;
		}
	}
	printf("2 %d\n", i);
	if (i == 63) i = 0; else return 1;
	para->dram_para1 = (para->dram_para1 & ~ 0xf000) | (i << 12); 
	sunxi_delay(10);
	clrsetbits_le32(MC_WORK_MODE, 0xffc, 0xaa0);
	/*Page/column size ? */
	for (i = 9; i < 15; i ++) {
		for (j = 0; j < 64; j++) {
			if ( readl(0x80000000 + (1<<i)) != readl((j%2)?~(DRAM_BASE_ADDR + j*4):(DRAM_BASE_ADDR + j*4)) ) {
				break;
			}
		}
		if ((i == 9) && (j == 63)) {i = 0; break;}
		else if ((i == 13) && (j == 63)) break;
		else if (i == 14) {i = 13; break;};
	}
	if (j != 63) return 1;
	if (i != 0) i = 1 << (i - 10);
	
	para->dram_para1 = (para->dram_para1 & ~0xf ) | (1 << (i - 10));
	return 0;
}

// ret: 0 - fail, 1 - ok
unsigned int mctl_core_init(__dram_para_t *para)
{
	unsigned int ret;
	
	mctl_clk_init(para);
	mctl_com_init(para);
	auto_set_timing_para(para);
	ret = mctl_channel_init(0, para);
	return ret;
}


//0 - ok, 1 - fail
unsigned int dram_simple_test(unsigned int dram_size, unsigned int len) 
{
	unsigned int i;
	
	dram_size >>= 1;
	dram_size <<= 20;

	for (i = 0; i < len; i ++) {
		writel(0x1234567 + i, DRAM_BASE_ADDR + i*4);
		writel(0xFEDCBA98 + i, DRAM_BASE_ADDR + i*4 + dram_size);	
	}
	
	for (i = 0; i < len; i ++) {	
		if (readl(DRAM_BASE_ADDR + i*4 + dram_size) != 0xFEDCBA98 + i) {
			printf("DRAM simple test FAIL.\n");
			return 1;
		}
		if (readl(DRAM_BASE_ADDR + i*4) != (i + 0x1234567)) {
			printf("DRAM simple test FAIL.\n");
			return 1;
		}
	}
//	printf("DRAM simple test OK.\n");
	return 0;
}


// 0 - fail, else - dram size in MB
unsigned int do_dram_init(__dram_para_t *para)
{
	unsigned int reg_val;
	unsigned int ret = 1;
	unsigned int dram_size = 0;

	//printf("Dram boot drive info v.0.2\n");
	if ((para->dram_tpr13 & 1) == 0) {
		ret = dram_autodetect_size(para);
		if (ret == 0) return 0;
	} else if ((para->dram_tpr13 & 0x8000) == 0) {
		para->dram_tpr13 |= 0x6000;
		para->dram_tpr13 |= 0x1;
	}	
//	printf("DRAM CLK =%d MHZ\n", para->dram_clk);
//	printf("DRAM Type =%d (2:DDR2,3:DDR3)\n", para->dram_type);
//	printf("DRAM zq value: 0x%x\n", para->dram_zq);
	
	ret = mctl_core_init(para);
	
	if (ret == 0) {
		printf("DRAM initial error : 1 !\n");
		return 0;	
	}
	//loc_1d14
	if ((para->dram_para2 & 0x7fff) != 0) {
		dram_size = 0x7fff & (para->dram_para2 >>16);
	} else {
		dram_size = dram_get_size();
		if (dram_size > 0xc00) dram_size = 0xc00;
		para->dram_para2 = (para->dram_para2 >> 16) | (dram_size << 16);		
	}
//	printf("DRAM SIZE =%d M\n",dram_size);

	reg_val = readl(PGCR0) & ~0xf000;
	if (para->dram_tpr13 & 0x200) {
		writel(reg_val, PGCR0);
//		printf("HDR/DDR dynamic mode!\n");
	} else {
		reg_val |= 0x5000;
		writel(reg_val, PGCR0);
//		printf("HDR/DDR always on mode!\n");
	}
	reg_val = readl(ZQCR);
	reg_val |= (0x1U<<31);
	writel(reg_val, ZQCR);
	setbits_le32(ZQCR, 0x80000000);
	if ((para->dram_tpr13 & 0x100) != 0) {
		setbits_le32(VTFCR, 0x300);
//		printf("VTF enable\n");		
	}

	if ((para->dram_tpr13 & 0x4000000) == 0) {
		setbits_le32(PGCR2, 0x2000);
//		printf("DQ hold enable!\n");
	} else {
		clrbits_le32(PGCR2, 0x2000);
//		printf("DQ hold disable!\n");	
	}
	if ((para->dram_tpr13 & 0x8000000) == 0) {
		set_master_priority();	
	}
	dram_enable_all_master();
	ret = dram_simple_test(dram_size, 0x100);
	if (ret != 0) return 0;
	return dram_size;
}

unsigned long sunxi_dram_init(void)
{
	unsigned long dram_size;
	
	__dram_para_t dram_para;

#ifdef CONFIG_SUNXI_DRAM_CLK
	dram_para.dram_clk		= CONFIG_SUNXI_DRAM_CLK;
#else		
	dram_para.dram_clk		= 408;		// Keep in mind that UserManual says maximum SDRAM freq is 400MHz
#endif
	dram_para.dram_type		= 2;
#ifdef CONFIG_SUNXI_DRAM_ZQ		
	dram_para.dram_zq 		= CONFIG_SUNXI_DRAM_ZQ;
#else
	dram_para.dram_zq		= 0x3b3bfb; 
#endif
	dram_para.dram_odt_en	= 0;
	dram_para.dram_para1	= 0x000000D2;
#ifdef CONFIG_SUNXI_DRAM_SIZE			
	dram_para.dram_para2	= CONFIG_SUNXI_DRAM_SIZE << 16;
#else
	dram_para.dram_para2	= 0x400000;
#endif
	dram_para.dram_mr0		= 0xE73;
	dram_para.dram_mr1		= 0x0;
	dram_para.dram_mr2		= 0x0; 
	dram_para.dram_mr3		= 0x0; 
	dram_para.dram_tpr0		= 0x00461950;
	dram_para.dram_tpr1		= 0x0121210B; 
	dram_para.dram_tpr2		= 0x0004F03A; 
	dram_para.dram_tpr3		= 0x0AD3A981; 
	dram_para.dram_tpr4		= 0x0;
	dram_para.dram_tpr5		= 0x0;
	dram_para.dram_tpr6		= 0x0;
	dram_para.dram_tpr7		= 0x1e380000;
	dram_para.dram_tpr8		= 0x0;
	dram_para.dram_tpr9		= 0x0;
	dram_para.dram_tpr10	= 0x0;
	dram_para.dram_tpr11	= 0x0;
	dram_para.dram_tpr12	= 0x0;
	dram_para.dram_tpr13	= 0x0;
	
	dram_size = do_dram_init(&dram_para);

	return dram_size*1024*1024;
	//return (dram_para.para1 >> 16)*1024*1024;	
}









	
	
	
	