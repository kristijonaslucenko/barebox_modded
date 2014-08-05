#include <common.h>
#include <sizes.h>
#include <io.h>
#include <init.h>
#include <asm/barebox-arm-head.h>
#include <asm/barebox-arm.h>
#include <mach/am33xx-silicon.h>
#include <mach/am33xx-clock.h>
#include <mach/generic.h>
#include <mach/sdrc.h>
#include <mach/sys_info.h>
#include <mach/syslib.h>
#include <mach/am33xx-mux.h>
#include <mach/am33xx-generic.h>
#include <mach/wdt.h>
#include <debug_ll.h>

#define CLK_M_OSC_MHZ 	25
#define DDR_IOCTRL	0x18B

static const struct am33xx_cmd_control pfla03_cmd = {
	.slave_ratio0	= 0x80,
	.dll_lock_diff0	= 0x0,
	.invert_clkout0	= 0x0,
	.slave_ratio1	= 0x80,
	.dll_lock_diff1	= 0x0,
	.invert_clkout1	= 0x0,
	.slave_ratio2	= 0x80,
	.dll_lock_diff2	= 0x0,
	.invert_clkout2	= 0x0,
};

static const struct am33xx_emif_regs MT41K128M16JT_256M_regs = {
	.emif_read_latency	= 0x7,
	.emif_tim1		= 0x0AAAD4DB,
	.emif_tim2		= 0x26437FDA,
	.emif_tim3		= 0x501F83FF,
	.sdram_config		= 0x61C052B2,
	.zq_config		= 0x50074BE4,
	.sdram_ref_ctrl		= 0x00000C30,
};

static const struct am33xx_ddr_data MT41K128M16JT_256M_data = {
	.rd_slave_ratio0	= 0x34,	/* PHY_RD_DQS_SLAVE_RATIO */
	.wr_dqs_slave_ratio0	= 0x47,	/* PHY_WR_DQS_SLAVE_RATIO */
	.fifo_we_slave_ratio0	= 0x9a,	/* PHY_FIFO_WE_SLAVE_RATIO */
	.wr_slave_ratio0	= 0x7e,	/* PHY_WR_DATA_SLAVE_RATIO */
	.use_rank0_delay	= 0x0,
	.dll_lock_diff0		= 0x0,
};

static const struct am33xx_emif_regs MT41K256M16HA_512M_regs = {
	.emif_read_latency	= 0x7,
	.emif_tim1		= 0x0AAAE4DB,
	.emif_tim2		= 0x266B7FDA,
	.emif_tim3		= 0x501F867F,
	.sdram_config		= 0x61C05332,
	.zq_config		= 0x50074BE4,
	.sdram_ref_ctrl		= 0x00000C30,
};

static const struct am33xx_ddr_data MT41K256M16HA_512M_data = {
	.rd_slave_ratio0	= 0x36,	/* PHY_RD_DQS_SLAVE_RATIO */
	.wr_dqs_slave_ratio0	= 0x47,	/* PHY_WR_DQS_SLAVE_RATIO */
	.fifo_we_slave_ratio0	= 0x95,	/* PHY_FIFO_WE_SLAVE_RATIO */
	.wr_slave_ratio0	= 0x7f,	/* PHY_WR_DATA_SLAVE_RATIO */
	.use_rank0_delay	= 0x0,
	.dll_lock_diff0		= 0x0,
};

extern char __dtb_am335x_phytec_phyflex_start[];

/**
 * @brief The basic entry point for board initialization.
 *
 * This is called as part of machine init (after arch init).
 * This is again called with stack in SRAM, so not too many
 * constructs possible here.
 *
 * @return void
 */
static noinline void pfla03_board_init(void)
{
	void *fdt;

	/* WDT1 is already running when the bootloader gets control
	 * Disable it to avoid "random" resets
	 */
	writel(WDT_DISABLE_CODE1, AM33XX_WDT_REG(WSPR));
	while (readl(AM33XX_WDT_REG(WWPS)) != 0x0);

	writel(WDT_DISABLE_CODE2, AM33XX_WDT_REG(WSPR));
	while (readl(AM33XX_WDT_REG(WWPS)) != 0x0);

	/* Setup PLLs and peripheral clocks */
	am33xx_pll_init(MPUPLL_M_600, CLK_M_OSC_MHZ, DDRPLL_M_400);
/*	if (IS_ENABLED(CONFIG_256MB_MT41K128M16JT))
		am335x_sdram_init(DDR_IOCTRL, &pfla03_cmd,
		                  &MT41K128M16JT_256M_regs,
		                  &MT41K128M16JT_256M_data);
	else if (IS_ENABLED(CONFIG_512MB_MT41K256M16HA))
*/		am335x_sdram_init(DDR_IOCTRL, &pfla03_cmd,
		                  &MT41K256M16HA_512M_regs,
		                  &MT41K256M16HA_512M_data);

	am33xx_uart_soft_reset((void *)AM33XX_UART0_BASE);
	am33xx_enable_uart0_pin_mux();
	omap_uart_lowlevel_init((void *)AM33XX_UART0_BASE);
	putc_ll('>');

	fdt = __dtb_am335x_phytec_phyflex_start - get_runtime_offset();

	barebox_arm_entry(0x80000000, SZ_512M, fdt);
}

ENTRY_FUNCTION(start_am33xx_phytec_phyflex_sram, bootinfo, r1, r2)
{
	am33xx_save_bootinfo((void *)bootinfo);

	arm_cpu_lowlevel_init();

	/*
	 * Setup C environment, the board init code uses global variables.
	 * Stackpointer has already been initialized by the ROM code.
	 */
	relocate_to_current_adr();
	setup_c();

	pfla03_board_init();
}

ENTRY_FUNCTION(start_am33xx_phytec_phyflex_sdram, r0, r1, r2)
{
	void *fdt;

	fdt = __dtb_am335x_phytec_phyflex_start - get_runtime_offset();

	barebox_arm_entry(0x80000000, SZ_512M, fdt);
}
