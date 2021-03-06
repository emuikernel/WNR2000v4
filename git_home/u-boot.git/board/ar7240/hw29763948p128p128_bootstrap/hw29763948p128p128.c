#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include "ar7240_soc_hw29763948p128p128.h"
#include <nand.h>

extern int wasp_ddr_initial_config(uint32_t refresh);
extern int ar7240_ddr_find_size(void);

#ifdef COMPRESSED_UBOOT
#	define prmsg(...)
#	define args		char *s
#	define board_str(a)	strcpy(s, a)
#else
#	define prmsg		printf
#	define args		void
#	define board_str(a)	printf(a)
#endif

void
wasp_usb_initial_config(void)
{
#define unset(a)	(~(a))

	if ((ar7240_reg_rd(WASP_BOOTSTRAP_REG) & WASP_REF_CLK_25) == 0) {
		ar7240_reg_wr_nf(AR934X_SWITCH_CLOCK_SPARE,
			ar7240_reg_rd(AR934X_SWITCH_CLOCK_SPARE) |
			SWITCH_CLOCK_SPARE_USB_REFCLK_FREQ_SEL_SET(2));
	} else {
		ar7240_reg_wr_nf(AR934X_SWITCH_CLOCK_SPARE,
			ar7240_reg_rd(AR934X_SWITCH_CLOCK_SPARE) |
			SWITCH_CLOCK_SPARE_USB_REFCLK_FREQ_SEL_SET(5));
	}

	udelay(1000);
	ar7240_reg_wr(AR7240_RESET,
		ar7240_reg_rd(AR7240_RESET) |
		RST_RESET_USB_PHY_SUSPEND_OVERRIDE_SET(1));
	udelay(1000);
	ar7240_reg_wr(AR7240_RESET,
		ar7240_reg_rd(AR7240_RESET) &
		unset(RST_RESET_USB_PHY_RESET_SET(1)));
	udelay(1000);
	ar7240_reg_wr(AR7240_RESET,
		ar7240_reg_rd(AR7240_RESET) &
		unset(RST_RESET_USB_PHY_ARESET_SET(1)));
	udelay(1000);
	ar7240_reg_wr(AR7240_RESET,
		ar7240_reg_rd(AR7240_RESET) &
		unset(RST_RESET_USB_HOST_RESET_SET(1)));
	udelay(1000);
	if ((ar7240_reg_rd(AR7240_REV_ID) & 0xf) == 0) {
		/* Only for WASP 1.0 */
		ar7240_reg_wr(0xb8116c84 ,
			ar7240_reg_rd(0xb8116c84) & unset(1<<20));
	}
}

/* GPIO */
#define AR9344_GPIO_OE              AR7240_GPIO_BASE
#define AR9344_GPIO_IN              AR7240_GPIO_BASE+0x04
#define AR9344_GPIO_OUT             AR7240_GPIO_BASE+0x08
#define AR9344_GPIO_FUNCTION        AR7240_GPIO_BASE+0x6c
#define AR9344_GPIO_OUT_FUNCTION0   AR7240_GPIO_BASE+0x2c
#define AR9344_GPIO_OUT_FUNCTION1   AR7240_GPIO_BASE+0x30
#define AR9344_GPIO_OUT_FUNCTION2   AR7240_GPIO_BASE+0x34
#define AR9344_GPIO_OUT_FUNCTION3   AR7240_GPIO_BASE+0x38
#define AR9344_GPIO_OUT_FUNCTION4   AR7240_GPIO_BASE+0x3c
#define AR9344_GPIO_OUT_FUNCTION5   AR7240_GPIO_BASE+0x40

#define ar9344_gpio_set(_pin)    ar7240_reg_rmw_set(AR9344_GPIO_OUT, (1 << _pin))
#define ar9344_gpio_clear(_pin)  ar7240_reg_rmw_clear(AR9344_GPIO_OUT, (1 << _pin))
#define ar9344_power_led_on()    ar9344_gpio_clear(POWER_LED_GPIO)
#define ar9344_power_led_off()   ar9344_gpio_set(POWER_LED_GPIO)
#define ar9344_status_led_on()   ar9344_gpio_clear(STATUS_LED_GPIO)
#define ar9344_status_led_off()  ar9344_gpio_set(STATUS_LED_GPIO)

/* return : 0 (not pressed) / 1 (pressed) */
static int ar9344_reset_button_is_press(void)
{
	if ( ar7240_reg_rd(AR9344_GPIO_IN) & (1 << RESET_BUTTON_GPIO))
		return 0;
	return 1;
}

static void ar9344_gpio_init(void)
{
	unsigned int rddata;

	/* gpio 0~3 are default to JTAG pins, we must disable JTAG function. */
	ar7240_reg_rmw_set(AR9344_GPIO_FUNCTION, (1 << 1));

	rddata = ar7240_reg_rd(AR9344_GPIO_OUT_FUNCTION0);
	ar7240_reg_wr(AR9344_GPIO_OUT_FUNCTION0, (rddata & 0xff00ff00)); /* 03, 02, 01, 00 */
//	rddata = ar7240_reg_rd(AR9344_GPIO_OUT_FUNCTION1);
//	ar7240_reg_wr(AR9344_GPIO_OUT_FUNCTION1, (rddata & 0xffffffff)); /* 07, 06, 05, 04 */
//	rddata = ar7240_reg_rd(AR9344_GPIO_OUT_FUNCTION2);
//	ar7240_reg_wr(AR9344_GPIO_OUT_FUNCTION2, (rddata & 0xffffffff)); /* 11, 10, 09, 08 */
//	rddata = ar7240_reg_rd(AR9344_GPIO_OUT_FUNCTION3);
//	ar7240_reg_wr(AR9344_GPIO_OUT_FUNCTION3, (rddata & 0xffffffff)); /* 15, 14, 13, 12 */
//	rddata = ar7240_reg_rd(AR9344_GPIO_OUT_FUNCTION4);
//	ar7240_reg_wr(AR9344_GPIO_OUT_FUNCTION4, (rddata & 0xffffffff)); /* 19, 18, 17, 16 */
//	rddata = ar7240_reg_rd(AR9344_GPIO_OUT_FUNCTION5);
//	ar7240_reg_wr(AR9344_GPIO_OUT_FUNCTION5, (rddata & 0xffffffff)); /* 23, 22, 21, 20 */

	/* let gpio pin of power, status leds, xLAN chain0 & xLAN chain1 output enabled. */
	ar7240_reg_rmw_clear(AR9344_GPIO_OE,
	                     (1 << POWER_LED_GPIO) | (1 << STATUS_LED_GPIO)
	                      | (1 << xLNA_0_GPIO) | (1 << xLNA_1_GPIO) );
	/* let gpio pin of reset button be input enabled. */
	ar7240_reg_rmw_set(AR9344_GPIO_OE, (1 << RESET_BUTTON_GPIO));
}

#define AR7240_GPIO_OUT_FUNCTION4       AR7240_GPIO_BASE+0x3c

void wasp_gpio_config(void)
{
	ar9344_gpio_init();

	/* disable the CLK_OBS on GPIO_4 and set GPIO4 as input */
	ar7240_reg_rmw_clear(GPIO_OE_ADDRESS, (1 << 4));
	ar7240_reg_rmw_clear(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_MASK);
	ar7240_reg_rmw_set(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_SET(0x80));
	ar7240_reg_rmw_set(GPIO_OE_ADDRESS, (1 << 4));
	prmsg("\nQualcomm xLNA control routine\n");
	ar7240_reg_wr (AR7240_GPIO_OUT_FUNCTION4,(ar7240_reg_rd(AR7240_GPIO_OUT_FUNCTION4) & 0xffff0000) | 0x2f2e0000);
}

void ath_set_tuning_caps(void)
{
	typedef struct {
		u_int8_t	pad[0x28],
				params_for_tuning_caps[2],
				featureEnable;
	} __attribute__((__packed__)) ar9300_eeprom_t;

	ar9300_eeprom_t	*eep = (ar9300_eeprom_t *)WLANCAL;
	uint32_t	val;


	val = 0;
	/* checking feature enable bit 6 and caldata is valid */
	if ((eep->featureEnable & 0x40) && (eep->pad[0x0] != 0xff)) {
		/* xtal_capin -bit 17:23 and xtag_capout -bit 24:30*/
		val = (eep->params_for_tuning_caps[0] & 0x7f) << 17;
		val |= (eep->params_for_tuning_caps[0] & 0x7f) << 24;
	} else {
		/* default when no caldata available*/
		/* checking clock in bit 4 */
		if (ar7240_reg_rd(RST_BOOTSTRAP_ADDRESS) & 0x10) {
			val = (0x1020 << 17);  /*default 0x2040 for 40Mhz clock*/
		} else {
			val = (0x2040 << 17); /*default 0x4080 for 25Mhz clock*/
		}
	}
	val |= (ar7240_reg_rd(XTAL_ADDRESS) & (((1 << 17) - 1) | (1 << 31)));
	ar7240_reg_wr(XTAL_ADDRESS, val);
	prmsg("Setting 0xb8116290 to 0x%x\n", val);
	return;
}

int
wasp_mem_config(void)
{
	extern void ath_ddr_tap_cal(void);
	unsigned int type, reg32;

	type = wasp_ddr_initial_config(CFG_DDR_REFRESH_VAL);

	ath_ddr_tap_cal();

	prmsg("Tap value selected = 0x%x [0x%x - 0x%x]\n",
		ar7240_reg_rd(AR7240_DDR_TAP_CONTROL0),
		ar7240_reg_rd(0xbd007f10), ar7240_reg_rd(0xbd007f14));

	/* Take WMAC out of reset */
	reg32 = ar7240_reg_rd(AR7240_RESET);
	reg32 = reg32 &  ~AR7240_RESET_WMAC;
	ar7240_reg_wr_nf(AR7240_RESET, reg32);

#if !defined(CONFIG_ATH_NAND_BR)
	/* Switching regulator settings */
	ar7240_reg_wr_nf(0x18116c40, 0x633c8176); /* AR_PHY_PMU1 */
	ar7240_reg_wr_nf(0x18116c44, 0x10380000); /* AR_PHY_PMU2 */

	wasp_usb_initial_config();

#endif /* !defined(CONFIG_ATH_NAND_BR) */

	wasp_gpio_config();

	ath_set_tuning_caps(); /* Needed here not to mess with Ethernet clocks */ 

	reg32 = ar7240_ddr_find_size();

	return reg32;
}

long int initdram(int board_type)
{
	return (wasp_mem_config());
}

int	checkboard(args)
{
	board_str("DNI HW ID: 29763948 flash 128MB RAM 128MB U-boot dni29 V1.0\n");
	return 0;
}

/*ledstat 0:on; 1:off*/
void board_power_led(int ledstat)
{
	if (ledstat == 1){
		ar9344_power_led_off();
	} else {
		ar9344_status_led_off();
		ar9344_power_led_on();
	}
}

/*ledstat 0:on; 1:off*/
void board_test_led(int ledstat)
{
	if (ledstat == 1){
		ar9344_status_led_off();
	} else {
		ar9344_power_led_off();
		ar9344_status_led_on();
	}
}
