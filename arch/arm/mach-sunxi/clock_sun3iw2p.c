/*
 * f1c800 specific clock code
 *
 *
 * (C) Copyright 2024 Sergey Goncharov <khorght@mail.ru>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/prcm.h>
#include <asm/arch/sys_proto.h>

#include <asm/gpio.h>
#include <asm/arch/gpio.h>

#ifdef CONFIG_SPL_BUILD
void clock_init_safe(void)
{
	unsigned int reg_val;
	
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	
	/* Open Gate for PIO */
	setbits_le32(&ccm->apb1_gate, (1<<5));
	/* Set GPIOs PG6/7 as Outputs and put them HIGH to lightup leds */
	//sunxi_gpio_set_cfgpin(SUNXI_GPG(6), 1);
	//sunxi_gpio_set_cfgpin(SUNXI_GPG(7), 1);
	//setbits_le32(0x01C20800+0x00E8, (1<<6)|(1<<7));	
	
	/* Set CPU clock to default 800MHz */
	clock_set_pll1(800000000); /* Argument here meaningless, we set to mandatory value */
	/* Set Periph0(2x) to default 1.2MHz */
	clock_set_pll6(1200000000);
	
	clock_set_pll10(297000000); // Default value for DE
	
	/* Set Periph1(2x) to default 1.2MHz */
	clrbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_PERIPH1);
	writel(0x00001811, &ccm->pll9_cfg);
	setbits_le32(&ccm->pll9_cfg, CCM_PLL9_CTRL_EN);
	setbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_PERIPH1);
	while (!(readl(&ccm->pll9_cfg) & CCM_PLL9_CTRL_LOCK))
		;
	
	/* Set AHB, APB1 */
	/* Note: First set PRE_DIV, the switch source */
	/* Note: For AHB1 - first sshould be changed source (Peripn0(1x)), then prediv */
	/* AHB1 - Periph0(1x)/4, APB1=AHB1/2, HCLK=CPUCLK */
	writel(0x000031A0, &ccm->ahb1_apb1_div);
	//writel(0x00012190, &ccm->ahb1_apb1_div); // value from original boot0
	// at startup it's 13180 -> AHB1/3, APB1=AHB1/2, AHB1 src - Periph0(1x), HCLK=CPUCLK/2 
	
	/* Set APB2 (used by UART/TWI/SCR */
	writel(0x01000000, &ccm->apb2_div); // Default is source OSC24M, div=1, prediv=1
	//writel(0x02030002, &ccm->apb2_div); // value from original boot0 -> periph0(2x)/8/3 = 50MHz
	
	/* Deassert reset, configure MBUS */
	setbits_le32(&ccm->mbus_reset, CCM_MBUS_RESET_RESET);
	writel(0x01000003, &ccm->mbus0_clk_cfg);	// Set Src to PLL_PERIPH0(2X), div 3+1 -> Peiprh0(2X)/4 -> 300MHz
	//setbits_le32(&ccm->mbus0_clk_cfg, 0x80000000);
	do {
		/* ARM926EJ-S code do not have sdelay */
		volatile int i = 10000;

		while (i > 0) i--;
	} while(0);
	
	/* Unknown registers and unknown purpose, but this part of code is present in original boot0 */
	reg_val = readl(&ccm->bus_reset2_cfg);
	reg_val |= 1;
	writel(reg_val, &ccm->bus_reset2_cfg);
	/* Disable unknown pll lock */
	reg_val = readl(0x01c20320);
	reg_val &= ~0x10;
	writel(reg_val, 0x01c20320);
	/* Unknown register */
	reg_val = readl(0x01c20020);
	reg_val |= 0x80000000;
	writel(reg_val, 0x01c20020);
	/* Enable unknown pll lock */
	reg_val = readl(0x01c20320);
	reg_val |= 0x10;
	writel(reg_val, 0x01c20320);
	while ((readl(0x01c20020) & 0x10000000) == 0)
		;
	/* Unknown register */
	reg_val = 0x10090000;
	writel(reg_val, 0x01c20230);
	do {
		/* ARM926EJ-S code do not have sdelay */
		volatile int i = 10000;

		while (i > 0) i--;
	} while(0);
	/* Unknown register */
	reg_val = readl(0x01c2005c);
	reg_val |= 0x80000000;
	writel(reg_val, 0x01c2005c);
	reg_val = readl(0x01c2005c);
	reg_val &= ~0x10000;
	reg_val |= 0x20000;
	writel(reg_val, 0x01c2005c);
	reg_val = readl(0x01c2005c);
	reg_val |= 3;
	writel(reg_val, 0x01c2005c);
	/* In original boot0 after all clocks are set i-cache enable made */
	//MRC             p15, 0, R0,c1,c0, 0
	//ORR             R0, R0, #0x1000											set bit 12 (I) I-cache
	//MCR             p15, 0, R0,c1,c0, 0	
	// U-Boot states that i_cache is enabled in start.S, additionally we may 
	// enable d-cache, in mach-sunxi/board.c, at the end there is a function for it
	// U-boot says it called immidiately after relocation https://github.com/ARM-software/u-boot/blob/master/doc/README.arm-caches
	//dcache_enable();

	
	
	/* After UART one more init happens - AVS, but try to place it here */
	reg_val = readl(&ccm->avs_clk_cfg);
	reg_val |= 0x80000000;
	writel(reg_val, &ccm->avs_clk_cfg);
	reg_val = 0x17;
	writel(reg_val, 0x01c20c8c);
	reg_val = 0;
	writel(reg_val, 0x01c20c84);
	reg_val = readl(0x01c20c80);
	reg_val |= 1;
	writel(reg_val, 0x01c20c80);
}
#endif

void clock_init_sec(void)
{

}

void clock_init_uart(void)
{
#if CONFIG_CONS_INDEX < 4
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* UART Clock Source is APB2 */
	writel(APB2_CLK_SRC_OSC24M | APB2_CLK_RATE_N_1 | APB2_CLK_RATE_M(1), &ccm->apb2_div);
	/* Deassert UART Reset */
	setbits_le32(&ccm->bus_reset3_cfg,
		     (1 << (APB2_RESET_UART_SHIFT + CONFIG_CONS_INDEX - 1)));	
	/* Open the Clock Gate for UART */
	setbits_le32(&ccm->apb2_gate,
		     (APB2_CLK_GATE_OPEN << (APB2_GATE_UART_SHIFT + CONFIG_CONS_INDEX - 1)));
#endif
}

#ifdef CONFIG_SPL_BUILD
// CPU
void clock_set_pll1(unsigned int clk)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	
	unsigned int k, m, n, value, diff;
	unsigned best_k = 0, best_m = 0, best_n = 0, best_diff = 0xffffffff;

	// Clk may be 200MHz ~ 2.1GHz
	if (clk > 2100000000) clk = 2100000000;
	if (clk < 200000000) clk = 200000000; 
	
	// All calculations are in KHz to avoid overflows
	/* PLL_CPU = (24000000*N*K)/(M*P) */
	clk /= 1000;
	// Pick the closest lower clock
	for (k = 1; k <= 4; k++) {
		for (m = 1; m <= 4; m++) {
			for (n = 1; n <= 32; n++) {
				value = 24000 * n * k / m;
				if (value > clk)
					continue;

				diff = clk - value;
				if (diff < best_diff) {
					best_diff = diff;
					best_k = k;
					best_m = m;
					best_n = n;
				}
				if (diff == 0)
					goto done;
			}
		}
	}

done:
	// 768MHz -> n = 16, k = 2, m = 1, p = 0(/1)
	
	/* PLL CPU Lock disable */
	clrbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_CPU);
	/* Switch to 24MHz clock while changing PLL1 */
	writel((CPU_CLK_SRC_OSC24M << CPU_CLK_SRC_SHIFT), &ccm->cpu_axi_cfg);
	/* Delay at least 10uS */
	do {
		/* ARM926EJ-S code do not have sdelay */
		volatile int i = 100;

		while (i > 0) i--;
	} while(0);
	
	/* Set PLL */
/*	writel(CCM_PLL1_CTRL_N(best_n) |
		   CCM_PLL1_CTRL_K(best_k) |
		   CCM_PLL1_CTRL_M(best_m) |
		   CCM_PLL1_CTRL_P(0x0),
		   &ccm->pll1_cfg);*/
	writel(CCM_PLL1_CTRL_N(17) |	//16
		   CCM_PLL1_CTRL_K(1) |		//2
		   CCM_PLL1_CTRL_M(1) |
		   CCM_PLL1_CTRL_P(0x0),
		   &ccm->pll1_cfg);
	
	/* Enable PLL */
	setbits_le32(&ccm->pll1_cfg, CCM_PLL1_CTRL_EN);
	/* CPU PLL Lock enable */
	setbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_CPU);
	/* Poll lock bit */
	while ((readl(&ccm->pll1_cfg) & CCM_PLL1_CTRL_LOCK) == 0)
		;
	/* Delay at least 20uS */
	do {
		/* ARM926EJ-S code do not have sdelay */
		volatile int i = 200;

		while (i > 0) i--;
	} while(0);
	/* Switch CPU to PLL1 */
	writel(CPU_CLK_SRC_PLL1 << CPU_CLK_SRC_SHIFT, &ccm->cpu_axi_cfg);
}
#endif

// Video0
void clock_set_pll3(unsigned int clk)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	
	const int m = 8;
	
	if (clk == 0) {
		clrbits_le32(&ccm->pll3_cfg, CCM_PLL3_CTRL_EN);
		return;
	}	
	/* PLL3 Lock disable */
	clrbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_VIDEO0);
	if (clk > 600000000) clk = 600000000;
	if (clk < 192000000) clk = 192000000;
	
	// Default is 297 -> n = 99, m = 8

	/* PLL3 rate = 24000000 * n / m */
	writel(CCM_PLL3_CTRL_INTEGER_MODE |
	       CCM_PLL3_CTRL_N(clk / (24000000 / m)) | 
	       CCM_PLL3_CTRL_M(m),
	       &ccm->pll3_cfg);
	setbits_le32(&ccm->pll3_cfg, CCM_PLL3_CTRL_EN);
	/* PLL3 Lock enable */
	setbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_VIDEO0);
	
	while ((readl(&ccm->pll3_cfg) & CCM_PLL3_CTRL_LOCK) == 0)
		;
}

#ifdef CONFIG_SUNXI_DE2
/* Video0 */
void clock_set_pll3_factors(int m, int n)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* PLL3 Lock disable */
	clrbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_VIDEO0);
	/* PLL3 rate = 24000000 * n / m */
	writel(CCM_PLL3_CTRL_INTEGER_MODE |
	       CCM_PLL3_CTRL_N(n) | 
	       CCM_PLL3_CTRL_M(m),
	       &ccm->pll3_cfg);
	setbits_le32(&ccm->pll3_cfg, CCM_PLL3_CTRL_EN);
	/* PLL3 Lock enable */
	setbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_VIDEO0);
	
	while ((readl(&ccm->pll3_cfg) & CCM_PLL3_CTRL_LOCK) == 0)
		;
}
#endif

// DDR1
/* DDR1 PLL moved from PLL5 to PLL11, use this as a wrapper */
void clock_set_pll5(unsigned int clk, bool sigma_delta_enable)
{
	clock_set_pll11(clk, sigma_delta_enable);
}

// Periph0
void clock_set_pll6(unsigned int clk)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	
	/* Periph0(2x) = 24M*N*K should be in range 504MHz .. 1.4GHz, default safe 1.2MHz */
	/* K >=2 in priority, 21=<N=<58 */	
	
	/* PLL6 Lock disable */
	clrbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_PERIPH0);
	/* Set Periph0(2x) to default 1.2MHz */
	writel(0x90001811, &ccm->pll6_cfg);
	/* PLL6 Lock enable */
	setbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_PERIPH0);
	
	while ((readl(&ccm->pll6_cfg) & CCM_PLL6_CTRL_LOCK) == 0)
		;
	
}

#ifdef CONFIG_MACH_SUN6I
void clock_set_mipi_pll(unsigned int clk)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	unsigned int k, m, n, value, diff;
	unsigned best_k = 0, best_m = 0, best_n = 0, best_diff = 0xffffffff;
	unsigned int src = clock_get_pll3();

	/* All calculations are in KHz to avoid overflows */
	clk /= 1000;
	src /= 1000;

	/* Pick the closest lower clock */
	for (k = 1; k <= 4; k++) {
		for (m = 1; m <= 16; m++) {
			for (n = 1; n <= 16; n++) {
				value = src * n * k / m;
				if (value > clk)
					continue;

				diff = clk - value;
				if (diff < best_diff) {
					best_diff = diff;
					best_k = k;
					best_m = m;
					best_n = n;
				}
				if (diff == 0)
					goto done;
			}
		}
	}

done:
	/* MIPI Lock disable */
	clrbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_MIPI);
	writel(CCM_MIPI_PLL_CTRL_EN | 
		   CCM_MIPI_PLL_CTRL_LDO1_EN | 
		   CCM_MIPI_PLL_SRC_VIDEO0 |
	       CCM_MIPI_PLL_CTRL_N(best_n) | 
		   CCM_MIPI_PLL_CTRL_K(best_k) |
	       CCM_MIPI_PLL_CTRL_M(best_m), 
		   &ccm->mipi_pll_cfg);
	/* MIPI Lock enable */
	setbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_MIPI);
	
	while ((readl(&ccm->mipi_pll_cfg) & CCM_MIPI_PLL_CTRL_LOCK) == 0)
		;
}
#endif

//#ifdef CONFIG_SUNXI_DE2
// DE
void clock_set_pll10(unsigned int clk)
{
	unsigned int reg_val;
	
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	const int m = 2; /* 12 MHz steps */

	if (clk == 0) {
		clrbits_le32(&ccm->pll10_cfg, CCM_PLL10_CTRL_EN);
		return;
	}
	/* DE Lock disable */
	clrbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_DE);
	/* PLL10 rate = 24000000 * n / m */
	//writel(CCM_PLL10_CTRL_EN | CCM_PLL10_CTRL_INTEGER_MODE |
	//      CCM_PLL10_CTRL_N(clk / (24000000 / m)) | CCM_PLL10_CTRL_M(m),
	//       &ccm->pll10_cfg);
	//boot0 value:
	reg_val = readl(&ccm->pll10_cfg);
	reg_val &= ~ ((0xf) | (0x7f00));
	writel(reg_val, &ccm->pll10_cfg);
	reg_val |= 0x1000000;
	writel(reg_val, &ccm->pll10_cfg);
	reg_val |= 0x2000000;
	writel(reg_val, &ccm->pll10_cfg);
	reg_val |= 0x80000000;
	writel(reg_val, &ccm->pll10_cfg);
	
	/* Set Lock enable */
	setbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_DE);
	
	while ((readl(&ccm->pll10_cfg) & CCM_PLL10_CTRL_LOCK) == 0)
		;
}
//#endif

// DDR1
void clock_set_pll11(unsigned int clk, bool sigma_delta_enable)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	if (clk > 1600000000) clk = 1600000000;
	if (clk < 192000000) clk = 192000000;
	

	if (sigma_delta_enable)
		writel(CCM_PLL11_PATTERN, &ccm->pll11_pattern_cfg0);

	/* PLL11 DDR1 Lock disable */
	clrbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_DDR1);
	
	/* PLL_DDR1 = (24MHz*N)/M, 192MHz..1.6GHz, default 600MHz */
	/* 16=< N =< 75 */
	
#ifdef CONFIG_DRAM_CLK
	writel(CCM_PLL11_CTRL_EN | CCM_PLL11_CTRL_UPD |
	       (sigma_delta_enable ? CCM_PLL11_CTRL_SIGMA_DELTA_EN : 0) |
	       CCM_PLL11_CTRL_N(clk / 24000000), &ccm->pll11_cfg);	
#else
	writel(0x00001800 | 
		   (sigma_delta_enable ? CCM_PLL11_CTRL_SIGMA_DELTA_EN : 0), &ccm->pll11_cfg); // set to default 600MHz
#endif	
	/* Validate PLL */
	setbits_le32(&ccm->pll11_cfg, CCM_PLL11_CTRL_UPD);
	/* Enable PLL */
	setbits_le32(&ccm->pll11_cfg, CCM_PLL11_CTRL_EN);
	/* PLL11 DDR1 Lock enable */
	setbits_le32(&ccm->pll_lock_ctrl, CCM_PLL_LOCK_DDR1);
	/* Poll Pll is valid, bit cleared - ok */
	while (readl(&ccm->pll11_cfg) & CCM_PLL11_CTRL_UPD)
		;
	while ((readl(&ccm->pll11_cfg) & CCM_PLL11_CTRL_LOCK) ==0)
		;
	
	/* Configure DRAM */
	writel(0x00000011, &ccm->dram_pll_cfg); /* 24M_PLL_DDR1, deassert reset */
	writel(CCM_DRAMCLK_CFG_SRC_PERIPH02X | CCM_DRAMCLK_CFG_RST, &ccm->dram_clk_cfg); 
	/* Validate DRAM */
	setbits_le32(&ccm->dram_clk_cfg, CCM_DRAMCLK_CFG_UPD);
	/* Poll configuration valid */
	while (readl(&ccm->dram_clk_cfg) & CCM_DRAMCLK_CFG_UPD)
		;
	/* Release reset for SDRAM */
	setbits_le32(&ccm->bus_reset0_cfg, (1<<14)); 
	/* Open clock gate for SDRAM */
	setbits_le32(&ccm->ahb_gate0, (1<<14)); 
}

//Video0
unsigned int clock_get_pll3(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll3_cfg);
	int n = ((rval & CCM_PLL3_CTRL_N_MASK) >> CCM_PLL3_CTRL_N_SHIFT) + 1;
	int m = ((rval & CCM_PLL3_CTRL_M_MASK) >> CCM_PLL3_CTRL_M_SHIFT) + 1;

	/* Multiply by 1000 after dividing by m to avoid integer overflows */
	return (24000 * n / m) * 1000;
}

//Periph0
unsigned int clock_get_pll6(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll6_cfg);
	int n = ((rval & CCM_PLL6_CTRL_N_MASK) >> CCM_PLL6_CTRL_N_SHIFT) + 1;
	int k = ((rval & CCM_PLL6_CTRL_K_MASK) >> CCM_PLL6_CTRL_K_SHIFT) + 1;
	
	return 24000000 * n * k / 2;
}

unsigned int clock_get_mipi_pll(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->mipi_pll_cfg);
	unsigned int n = ((rval & CCM_MIPI_PLL_CTRL_N_MASK) >> CCM_MIPI_PLL_CTRL_N_SHIFT) + 1;
	unsigned int k = ((rval & CCM_MIPI_PLL_CTRL_K_MASK) >> CCM_MIPI_PLL_CTRL_K_SHIFT) + 1;
	unsigned int m = ((rval & CCM_MIPI_PLL_CTRL_M_MASK) >> CCM_MIPI_PLL_CTRL_M_SHIFT) + 1;
	unsigned int src = clock_get_pll3();

	/* Multiply by 1000 after dividing by m to avoid integer overflows */
	return ((src / 1000) * n * k / m) * 1000;
}

void clock_set_de_mod_clock(u32 *clk_cfg, unsigned int hz)
{
	int pll = clock_get_pll6() * 2;
	int div = 1;

	while ((pll / div) > hz)
		div++;

	writel(CCM_DE_CTRL_GATE | CCM_DE_CTRL_PLL6_2X | CCM_DE_CTRL_M(div),
	       clk_cfg);
}
