

#ifndef _SUNXI_CLOCK_SUN3IW2P_H
#define _SUNXI_CLOCK_SUN3IW2P_H

struct sunxi_ccm_reg {
	u32 pll1_cfg;		/* 0x00 pll1 (cpu) control */
	u32 reserved0;
	u32 pll2_cfg;		/* 0x08 pll2 (audio) control */
	u32 reserved1;
	u32 pll3_cfg;		/* 0x10 pll3 (video0) control */
	u32 reserved2;
	u32 pll4_cfg;		/* 0x18 pll4 (ve) control */
	u32 reserved3;
	u32 reserved4;		//UNKNOWN REG
	u32 reserved5;
	u32 pll6_cfg;		/* 0x28 pll6 (periph0) control */
	u32 reserved6;
	u32 pll7_cfg;		/* 0x30 pll7 (video1) control */
	u32 reserved7;
	u32 reserved8;
	u32 reserved9;
	u32 mipi_pll_cfg;	/* 0x40 MIPI pll control */
	u32 pll9_cfg;		/* 0x44 pll9 (periph1) control */
	u32 pll10_cfg;		/* 0x48 pll10 (de) control */
	u32 pll11_cfg;		/* 0x4c pll11 (ddr1) control */
	u32 cpu_axi_cfg;	/* 0x50 CPU/AXI divide ratio */
	u32 ahb1_apb1_div;	/* 0x54 AHB1/APB1 divide ratio */
	u32 apb2_div;		/* 0x58 APB2 divide ratio */
	u32 reserved10;		//UNKNOWN REG
	u32 ahb_gate0;		/* 0x60 ahb module clock gating 0 */
	u32 ahb_gate1;		/* 0x64 ahb module clock gating 1 */
	u32 apb1_gate;		/* 0x68 apb1 module clock gating */
	u32 apb2_gate;		/* 0x6c apb2 module clock gating */
	u32 bus_gate4;      /* 0x70 gate 4 module clock gating */
	u32 reserved11;
	u32 tp_cfg;         /* 0x78 tp module clock gating */
	u32 reserved12[3];
	u32 sd0_clk_cfg;	/* 0x88 sd0 clock control */
	u32 sd1_clk_cfg;	/* 0x8c sd1 clock control */
	u32 reserved13;
	u32 reserved14;
	u32 ts_clk_cfg;		/* 0x98 transport stream clock (tsc) control */
	u32 ss_clk_cfg;		/* 0x9c security system clock (ce) control */
	u32 spi0_clk_cfg;	/* 0xa0 spi0 clock control */
	u32 spi1_clk_cfg;	/* 0xa4 spi1 clock control */
	u32 reserved15;
	u32 reserved16;
	u32 i2s0_clk_cfg;	/* 0xb0 I2S0 clock control*/
	u32 reserved17;
	u32 cir_cgf;		/* 0xb8 cir config */
	u32 reserved18[4];
	u32 usb_clk_cfg;	/* 0xcc USB clock (usbphy)control */
	u32 reserver19[8];
	u32 dram_pll_cfg;	/* 0xf0 DRAM Aux configuration clock control */
	u32 dram_clk_cfg;	/* 0xf4 DRAM configuration clock control */
	u32 dram_pll1_cfg;	/* 0xf8 PLL_DDR1 cfg register */
	u32 mbus_reset;		/* 0xfc MBUS reset control */
	u32 dram_clk_gate;	/* 0x100 DRAM module gating */
	u32 de_clk_cfg;		/* 0x104 DE module clock */
	u32 reserved20;
	u32 reserved21;
	u32 tcon_cfg;		/* 0x110 TCON module clock */
	u32 reserver22[4];
	u32 deinterlacer_cfg;	/* 0x124 DEINTERLACER module clock */
	u32 reserved23[5];
	u32 ve_clk_cfg;		/* 0x13c VE module clock */
	u32 adda_clk_cfg;	/* 0x140 ADDA (ac) module clock */
	u32 avs_clk_cfg;	/* 0x144 AVS module clock */
	u32 reserved24[5];
	u32 mbus0_clk_cfg;	/* 0x15c MBUS0 (mbus) module clock */
	u32 reserved25;
	u32 reserved26;
	u32 mipi_dsi_clk_cfg;	/* 0x168 MIPI DSI clock control */
	u32 reserved27[5];
	u32 tve_clk_cfg;	/* 0x180 TVE module clock */
	u32 reserved28;
	u32 tvdclk_cfg;		/* 0x188 TVD module clock */
	u32 reserved29[37];
	u32 pll1_bias_cfg;	/* 0x220 PLL1 CPU Bias config */ 
	u32 pll2_bias_cfg;	/* 0x224 PLL2 Audio Bias config */
	u32 pll3_bias_cfg;	/* 0x228 PLL3 Video0 Bias config */
	u32 pll4_bias_cfg;	/* 0x22c PLL4 VE Bias config */
	u32 reserved30a;
	u32 pll6_bias_cfg;	/* 0x234 PLL6 Periph0 Bias config */
	u32 pll7_bias_cfg;	/* 0x238 PLL7 Video1 Bias config */
	u32 mipi_bias_cfg;	/* 0x23c mipi Bias config */
	u32 reserved30;
	u32 pll9_bias_cfg;	/* 0x244 PLL9 Periph1 Bias config */
	u32 pll10_bias_cfg;	/* 0x248 PLL10 DE Bias config */
	u32 pll11_bias_cfg;	/* 0x24c PLL11 DDR1 Bias config */
	u32 cpu_tuning_cfg;	/* 0x250 CPU Tuning config */
	u32 reserved31[7];
	u32 mipi_tuning_cfg;	/* 0x270 MIPI Tuning config */
	u32 reserved32[3];
	u32 pll1_pattern_cfg;	/* 0x280 PLL1 (cpu) Pattern config */
	u32 pll2_pattern_cfg;	/* 0x284 PLL2 (audio) Pattern config */
	u32 pll3_pattern_cfg;	/* 0x288 PLL3 (video0) Pattern config */
	u32 pll4_pattern_cfg;	/* 0x28c PLL4 (ve) Pattern config */
	u32 reserved33;
	u32 reserved34;
	u32 pll7_pattern_cfg;	/* 0x298 PLL7 (video1) Pattern config */
	u32 reserved35;
	u32 mipi_pattern_cfg;	/* 0x2a0 MIPI Pattern config */
	u32 pll9_pattern_cfg;	/* 0x2a4 PLL9 (periph1) Pattern config */
	u32 pll10_pattern_cfg;	/* 0x2a8 PLL10 (de) Pattern config */
	u32 reserved36;
	u32 reserved37;
	u32 pll11_pattern_cfg0;	/* 0x2b4 PLL11 Pattern config0 (PLL_DDR1 Pattern) */
	u32 pll11_pattern_cfg1; /* 0x2b8 PLL11 Pattern config1 (PLL_DDR1 Pattern) */
	u32 reserved38;
	u32 bus_reset0_cfg;	/* 0x2c0 Bus Reset 0 config */
	u32 bus_reset1_cfg;	/* 0x2c4 Bus Reset 1 config */
	u32 reserved39;
	u32 reserved40;
	u32 bus_reset2_cfg;	/* 0x2d0 Bus Reset 2 config */
	u32 reserved41;
	u32 bus_reset3_cfg;	/* 0x2d8 Bus Reset 3 config */		
	u32 bus_reset4_cfg;	/* 0x2dc Bus Reset 4 config */
	u32 reserved42[8];
	u32 ps_ctrl_cfg;	/* 0x300 PS Control register */
	u32 ps_cnt_cfg;		/* 0x304 PS Counter register */
	u32 reserved43[6];
	u32 pll_lock_ctrl;	/* 0x320 PLL lock control */
};


/* apb2 bit field */
#define APB2_CLK_SRC_LOSC					(0x0 << 24)
#define APB2_CLK_SRC_OSC24M					(0x1 << 24)
#define APB2_CLK_SRC_PLL6					(0x2 << 24)		/* PLL_PERIPH0(2X) */
#define APB2_CLK_SRC_MASK					(0x3 << 24)
#define APB2_CLK_RATE_N_1					(0x0 << 16)
#define APB2_CLK_RATE_N_2					(0x1 << 16)
#define APB2_CLK_RATE_N_4					(0x2 << 16)
#define APB2_CLK_RATE_N_8					(0x3 << 16)
#define APB2_CLK_RATE_N_MASK				(3 << 16)
#define APB2_CLK_RATE_M(m)					(((m)-1) << 0)
#define APB2_CLK_RATE_M_MASK    			(0x1f << 0)
/* apb2 gate field */
#define APB2_GATE_UART_SHIFT				(16)
#define APB2_GATE_UART_MASK					(0xff << APB2_GATE_UART_SHIFT)
#define APB2_GATE_TWI_SHIFT					(0)
#define APB2_GATE_TWI_MASK					(0xf << APB2_GATE_TWI_SHIFT)
#define APB2_CLK_GATE_OPEN					1
#define APB2_CLK_GATE_CLOSE					0
/* apb2 reset */
#define APB2_RESET_UART_SHIFT				(16)
#define APB2_RESET_UART_MASK				(0xff << APB2_RESET_UART_SHIFT)
#define APB2_RESET_TWI_SHIFT				(0)
#define APB2_RESET_TWI_MASK					(0xf << APB2_RESET_TWI_SHIFT)
/* CPU axi/aux */
#define CPU_CLK_SRC_SHIFT					16
#define CPU_CLK_SRC_LOSC					0
#define CPU_CLK_SRC_OSC24M					1
#define CPU_CLK_SRC_PLL1					2
/* pll1 CPU */
#define CCM_PLL1_CTRL_M(n)					((((n) - 1) & 0x3) << 0)
#define CCM_PLL1_CTRL_K(n)					((((n) - 1) & 0x3) << 4)
#define CCM_PLL1_CTRL_N(n)					((((n) - 1) & 0x1f) << 8)
#define CCM_PLL1_CTRL_P(n)					(((n) & 0x3) << 16)
#define CCM_PLL1_CTRL_EN					(1 << 31)
#define CCM_PLL1_CTRL_LOCK					(1 << 28)
/* pll3 Video0 */
#define CCM_PLL3_CTRL_M_SHIFT				0
#define CCM_PLL3_CTRL_M_MASK				(0xf << CCM_PLL3_CTRL_M_SHIFT)
#define CCM_PLL3_CTRL_M(n)					((((n) - 1) & 0xf) << 0)
#define CCM_PLL3_CTRL_N_SHIFT				8
#define CCM_PLL3_CTRL_N_MASK				(0x7f << CCM_PLL3_CTRL_N_SHIFT)
#define CCM_PLL3_CTRL_N(n)					((((n) - 1) & 0x7f) << 8)
#define CCM_PLL3_CTRL_INTEGER_MODE			(1 << 24)
#define CCM_PLL3_CTRL_LOCK					(1 << 28)
#define CCM_PLL3_CTRL_EN					(1 << 31)
/* pll6 Periph0 */
#define CCM_PLL6_CTRL_N_SHIFT				8
#define CCM_PLL6_CTRL_N_MASK				(0x1f << CCM_PLL6_CTRL_N_SHIFT)
#define CCM_PLL6_CTRL_K_SHIFT				4
#define CCM_PLL6_CTRL_K_MASK				(0x3 << CCM_PLL6_CTRL_K_SHIFT)
#define CCM_PLL6_CTRL_LOCK					(1 << 28)
#define CCM_PLL6_CTRL_EN					(1 << 31)
/* mipi pll */
#define CCM_MIPI_PLL_CTRL_M_SHIFT			0
#define CCM_MIPI_PLL_CTRL_M_MASK			(0xf << CCM_MIPI_PLL_CTRL_M_SHIFT)
#define CCM_MIPI_PLL_CTRL_M(n)				((((n) - 1) & 0xf) << 0)
#define CCM_MIPI_PLL_CTRL_K_SHIFT			4
#define CCM_MIPI_PLL_CTRL_K_MASK			(0x3 << CCM_MIPI_PLL_CTRL_K_SHIFT)
#define CCM_MIPI_PLL_CTRL_K(n)				((((n) - 1) & 0x3) << 4)
#define CCM_MIPI_PLL_CTRL_N_SHIFT			8
#define CCM_MIPI_PLL_CTRL_N_MASK			(0xf << CCM_MIPI_PLL_CTRL_N_SHIFT)
#define CCM_MIPI_PLL_CTRL_N(n)				((((n) - 1) & 0xf) << 8)
#define CCM_MIPI_PLL_CTRL_LDO2_EN			(0x3 << 22)
#define CCM_MIPI_PLL_CTRL_LDO1_EN			(0x3 << 23)
#define CCM_MIPI_PLL_CTRL_EN				(0x1 << 31)
#define CCM_MIPI_PLL_SRC_VIDEO0				(0x0 << 21)
#define CCM_MIPI_PLL_SRC_VIDEO1				(0x1 << 21)
#define CCM_MIPI_PLL_CTRL_LOCK				(1 << 28)
/* pll9 Periph1 */
#define CCM_PLL9_CTRL_N_SHIFT				8
#define CCM_PLL9_CTRL_N_MASK				(0x1f << CCM_PLL9_CTRL_N_SHIFT)
#define CCM_PLL9_CTRL_K_SHIFT				4
#define CCM_PLL9_CTRL_K_MASK				(0x3 << CCM_PLL9_CTRL_K_SHIFT)
#define CCM_PLL9_CTRL_LOCK					(1 << 28)
#define CCM_PLL9_CTRL_EN					(1 << 31)
/* pll10 de */
#define CCM_PLL10_CTRL_M_SHIFT				0
#define CCM_PLL10_CTRL_M_MASK				(0xf << CCM_PLL10_CTRL_M_SHIFT)
#define CCM_PLL10_CTRL_M(n)					((((n) - 1) & 0xf) << 0)
#define CCM_PLL10_CTRL_N_SHIFT				8
#define CCM_PLL10_CTRL_N_MASK				(0x7f << CCM_PLL10_CTRL_N_SHIFT)
#define CCM_PLL10_CTRL_N(n)					((((n) - 1) & 0x7f) << 8)
#define CCM_PLL10_CTRL_INTEGER_MODE			(0x1 << 24)
#define CCM_PLL10_CTRL_LOCK					(0x1 << 28)
#define CCM_PLL10_CTRL_EN					(0x1 << 31)
/* pll11 DDR */
#define CCM_PLL11_CTRL_M(n)					((((n) - 1) & 0x3) << 0)
#define CCM_PLL11_CTRL_N(n)					((((n) - 1) & 0x7f) << 8)
#define CCM_PLL11_CTRL_NEW_MODE				(0x0 << 15)
#define CCM_PLL11_CTRL_ORIGINAL_MODE		(0x1 << 15)
#define CCM_PLL11_CTRL_SIGMA_DELTA_EN		(0x1 << 24)
#define CCM_PLL11_CTRL_LOCK					(0x1 << 28)
#define CCM_PLL11_CTRL_UPD					(0x1 << 30)
#define CCM_PLL11_CTRL_EN					(0x1 << 31)
/* pll DDR AUX */
#define CCM_PLLDDR_AUX_24M_SYS				(0x0 << 4)
#define CCM_PLLDDR_AUX_24M_PLL_DDR1			(0x1 << 4)
#define CCM_PLLDDR_AUX_RST					(0x1 << 0)	
/* pll lock regsiter */
#define CCM_PLL_LOCK_CPU					(0x1 << 0)
#define CCM_PLL_LOCK_AUDIO					(0x1 << 1)
#define CCM_PLL_LOCK_VIDEO0					(0x1 << 2)
#define CCM_PLL_LOCK_VE						(0x1 << 3)
#define CCM_PLL_LOCK_PERIPH0				(0x1 << 5)
#define CCM_PLL_LOCK_VIDEO1					(0x1 << 6)
#define CCM_PLL_LOCK_MIPI					(0x1 << 7)
#define CCM_PLL_LOCK_PERIPH1				(0x1 << 8)
#define CCM_PLL_LOCK_DE						(0x1 << 9)
#define CCM_PLL_LOCK_DDR1					(0x1 << 10)
/* dram cfg */
#define CCM_DRAMCLK_CFG_DIV(x)				((x - 1) << 0)
#define CCM_DRAMCLK_CFG_DIV_MASK			(0x3 << 0)
#define CCM_DRAMCLK_CFG_SRC_PERIPH02X		(0x1 << 20)
#define CCM_DRAMCLK_CFG_SRC_PLL11			(0x0 << 20)
#define CCM_DRAMCLK_CFG_SRC_MASK			(0x3 << 20)
#define CCM_DRAMCLK_CFG_UPD					(0x1 << 16)
#define CCM_DRAMCLK_CFG_RST					(0x1 << 31)
/* mbus reset */
#define CCM_MBUS_RESET_RESET				(0x1 << 31)
/* mbus clk */
#define CCM_MBUS_DIV_MASK					(0x3 << 0)
#define CCM_MBUS_DIV_M(x)					((x-1) << 0)
#define CCM_MBUS_SRC_MASK					(0x3 << 24)
#define CCM_MBUS_SRC_24M					(0x0 << 24)
#define CCM_MBUS_SRC_PERIPH0				(0x1 << 24)
#define CCM_MBUS_SRC_DDR1					(0x3 << 24)
#define CCM_MBUS_CLK_RESET					(0x1 << 31)
/* mmc clk */
#define CCM_MMC_CTRL_M(x)					((x) - 1)
#define CCM_MMC_CTRL_OCLK_DLY(x)			((x) << 8)
#define CCM_MMC_CTRL_N(x)					((x) << 16)
#define CCM_MMC_CTRL_SCLK_DLY(x)			((x) << 20)
#define CCM_MMC_CTRL_OSCM24					(0x0 << 24)
#define CCM_MMC_CTRL_PERIPH0				(0x1 << 24)		//2x
#define CCM_MMC_CTRL_PERIPH1				(0x2 << 24)		//2x
#define CCM_MMC_CTRL_PLL6					CCM_MMC_CTRL_PERIPH0
#define CCM_MMC_CTRL_ENABLE					(0x1 << 31)
#define CCM_MMC_CTRL_MODE_SEL_NEW			0 //(0x1 << 30) /* No such bit in SMHCx_CLK_REG, just ignore */

/* Unknown values just taken from another header file */
#define CCM_PLL5_PATTERN		0xd1303333
#define CCM_PLL11_PATTERN		0xf5860000

//#define CCM_PLL5_TUN_LOCK_TIME(x)	(((x) & 0x7) << 24)
//#define CCM_PLL5_TUN_LOCK_TIME_MASK	CCM_PLL5_TUN_LOCK_TIME(0x7)
//#define CCM_PLL5_TUN_INIT_FREQ(x)	(((x) & 0x7f) << 16)
//#define CCM_PLL5_TUN_INIT_FREQ_MASK	CCM_PLL5_TUN_INIT_FREQ(0x7f)


/* ahb_gate0 offsets */
#define AHB_GATE_OFFSET_USB_OHCI	29
#define AHB_GATE_OFFSET_USB_EHCI	26
#define AHB_GATE_OFFSET_USB_OTG		24
#define AHB_GATE_OFFSET_SPI1		21
#define AHB_GATE_OFFSET_SPI0		20
#define AHB_GATE_OFFSET_MCTL		14			// DRAM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define AHB_GATE_OFFSET_MMC1		9
#define AHB_GATE_OFFSET_MMC0		8
#define AHB_GATE_OFFSET_MMC(n)		(AHB_GATE_OFFSET_MMC0 + (n))
#define AHB_GATE_OFFSET_DMA			6
#define AHB_GATE_OFFSET_SS			5			// CE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



/* ahb_reset0 offsets */
#define AHB_RESET_OFFSET_OHCI		29
#define AHB_RESET_OFFSET_EHCI		26
#define AHB_RESET_OFFSET_OTG		24
#define AHB_RESET_OFFSET_SPI1		21
#define AHB_RESET_OFFSET_SPI0		20
#define AHB_RESET_OFFSET_MCTL		14				// SDRAM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define AHB_RESET_OFFSET_MMC1		9
#define AHB_RESET_OFFSET_MMC0		8
#define AHB_RESET_OFFSET_MMC(n)		(AHB_RESET_OFFSET_MMC0 + (n))
#define AHB_RESET_OFFSET_DMA		6
#define AHB_RESET_OFFSET_SS			5				// CE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!





/* CCM bits common to all Display Engine (and IEP) clock ctrl regs */
#define CCM_DE_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_DE_CTRL_PLL_MASK		(0xf << 24)
#define CCM_DE_CTRL_PLL3		(0 << 24)
#define CCM_DE_CTRL_PLL7		(1 << 24)
#define CCM_DE_CTRL_PLL6_2X		(2 << 24)
#define CCM_DE_CTRL_PLL8		(3 << 24)
#define CCM_DE_CTRL_PLL9		(4 << 24)
#define CCM_DE_CTRL_PLL10		(5 << 24)
#define CCM_DE_CTRL_GATE		(1 << 31)

/* CCM bits common to all Display Engine 2.0 clock ctrl regs */
#define CCM_DE2_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_DE2_CTRL_PLL_MASK		(3 << 24)
#define CCM_DE2_CTRL_PLL6_2X		(0 << 24)
#define CCM_DE2_CTRL_PLL10		(1 << 24)
#define CCM_DE2_CTRL_GATE		(0x1 << 31)

/* CCU security switch, H3 only */
#define CCM_SEC_SWITCH_MBUS_NONSEC	(1 << 2)
#define CCM_SEC_SWITCH_BUS_NONSEC	(1 << 1)
#define CCM_SEC_SWITCH_PLL_NONSEC	(1 << 0)

#define CCM_USB_CTRL_PHY0_RST (0x1 << 0)
#define CCM_USB_CTRL_PHY1_RST (0x1 << 1)
#define CCM_USB_CTRL_PHY2_RST (0x1 << 2)
#define CCM_USB_CTRL_OHCI0_CLK (0x1 << 6)
#define CCM_USB_CTRL_OHCI1_CLK (0x1 << 7)
#define CCM_USB_CTRL_PHYGATE (0x1 << 8)
/* These 3 are sun6i only, define them as 0 on sun4i */
#define CCM_USB_CTRL_PHY0_CLK 0
#define CCM_USB_CTRL_PHY1_CLK 0
#define CCM_USB_CTRL_PHY2_CLK 0



#ifndef __ASSEMBLY__
void clock_set_pll1(unsigned int hz);
void clock_set_pll3(unsigned int hz);
void clock_set_pll3_factors(int m, int n);
void clock_set_pll5(unsigned int clk, bool sigma_delta_enable);
void clock_set_pll6(unsigned int hz);
void clock_set_pll10(unsigned int hz);
void clock_set_pll11(unsigned int clk, bool sigma_delta_enable);
void clock_set_mipi_pll(unsigned int hz);
unsigned int clock_get_pll3(void);
unsigned int clock_get_pll6(void);
unsigned int clock_get_mipi_pll(void);
#endif

#endif /* _SUNXI_CLOCK_SUN3IW2P_H */
