#ifndef __iop_crc_par_defs_asm_h
#define __iop_crc_par_defs_asm_h

/*
 * This file is autogenerated from
 *   file:           ../../inst/io_proc/rtl/iop_crc_par.r
 *     id:           <not found>
 *     last modfied: Mon Apr 11 16:08:45 2005
 *
 *   by /n/asic/design/tools/rdesc/src/rdes2c -asm --outfile asm/iop_crc_par_defs_asm.h ../../inst/io_proc/rtl/iop_crc_par.r
 *      id: $Id: //depot/sw/releases/Aquila_9.2.0_U10/linux/kernels/mips-linux-2.6.31/arch/cris/include/arch-v32/arch/hwregs/iop/asm/iop_crc_par_defs_asm.h#1 $
 * Any changes here will be lost.
 *
 * -*- buffer-read-only: t -*-
 */

#ifndef REG_FIELD
#define REG_FIELD( scope, reg, field, value ) \
  REG_FIELD_X_( value, reg_##scope##_##reg##___##field##___lsb )
#define REG_FIELD_X_( value, shift ) ((value) << shift)
#endif

#ifndef REG_STATE
#define REG_STATE( scope, reg, field, symbolic_value ) \
  REG_STATE_X_( regk_##scope##_##symbolic_value, reg_##scope##_##reg##___##field##___lsb )
#define REG_STATE_X_( k, shift ) (k << shift)
#endif

#ifndef REG_MASK
#define REG_MASK( scope, reg, field ) \
  REG_MASK_X_( reg_##scope##_##reg##___##field##___width, reg_##scope##_##reg##___##field##___lsb )
#define REG_MASK_X_( width, lsb ) (((1 << width)-1) << lsb)
#endif

#ifndef REG_LSB
#define REG_LSB( scope, reg, field ) reg_##scope##_##reg##___##field##___lsb
#endif

#ifndef REG_BIT
#define REG_BIT( scope, reg, field ) reg_##scope##_##reg##___##field##___bit
#endif

#ifndef REG_ADDR
#define REG_ADDR( scope, inst, reg ) REG_ADDR_X_(inst, reg_##scope##_##reg##_offset)
#define REG_ADDR_X_( inst, offs ) ((inst) + offs)
#endif

#ifndef REG_ADDR_VECT
#define REG_ADDR_VECT( scope, inst, reg, index ) \
         REG_ADDR_VECT_X_(inst, reg_##scope##_##reg##_offset, index, \
			 STRIDE_##scope##_##reg )
#define REG_ADDR_VECT_X_( inst, offs, index, stride ) \
                          ((inst) + offs + (index) * stride)
#endif

/* Register rw_cfg, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_cfg___mode___lsb 0
#define reg_iop_crc_par_rw_cfg___mode___width 1
#define reg_iop_crc_par_rw_cfg___mode___bit 0
#define reg_iop_crc_par_rw_cfg___crc_out___lsb 1
#define reg_iop_crc_par_rw_cfg___crc_out___width 1
#define reg_iop_crc_par_rw_cfg___crc_out___bit 1
#define reg_iop_crc_par_rw_cfg___rev_out___lsb 2
#define reg_iop_crc_par_rw_cfg___rev_out___width 1
#define reg_iop_crc_par_rw_cfg___rev_out___bit 2
#define reg_iop_crc_par_rw_cfg___inv_out___lsb 3
#define reg_iop_crc_par_rw_cfg___inv_out___width 1
#define reg_iop_crc_par_rw_cfg___inv_out___bit 3
#define reg_iop_crc_par_rw_cfg___trig___lsb 4
#define reg_iop_crc_par_rw_cfg___trig___width 2
#define reg_iop_crc_par_rw_cfg___poly___lsb 6
#define reg_iop_crc_par_rw_cfg___poly___width 3
#define reg_iop_crc_par_rw_cfg_offset 0

/* Register rw_init_crc, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_init_crc_offset 4

/* Register rw_correct_crc, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_correct_crc_offset 8

/* Register rw_ctrl, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_ctrl___en___lsb 0
#define reg_iop_crc_par_rw_ctrl___en___width 1
#define reg_iop_crc_par_rw_ctrl___en___bit 0
#define reg_iop_crc_par_rw_ctrl_offset 12

/* Register rw_set_last, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_set_last___tr_dif___lsb 0
#define reg_iop_crc_par_rw_set_last___tr_dif___width 1
#define reg_iop_crc_par_rw_set_last___tr_dif___bit 0
#define reg_iop_crc_par_rw_set_last_offset 16

/* Register rw_wr1byte, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_wr1byte___data___lsb 0
#define reg_iop_crc_par_rw_wr1byte___data___width 8
#define reg_iop_crc_par_rw_wr1byte_offset 20

/* Register rw_wr2byte, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_wr2byte___data___lsb 0
#define reg_iop_crc_par_rw_wr2byte___data___width 16
#define reg_iop_crc_par_rw_wr2byte_offset 24

/* Register rw_wr3byte, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_wr3byte___data___lsb 0
#define reg_iop_crc_par_rw_wr3byte___data___width 24
#define reg_iop_crc_par_rw_wr3byte_offset 28

/* Register rw_wr4byte, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_wr4byte___data___lsb 0
#define reg_iop_crc_par_rw_wr4byte___data___width 32
#define reg_iop_crc_par_rw_wr4byte_offset 32

/* Register rw_wr1byte_last, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_wr1byte_last___data___lsb 0
#define reg_iop_crc_par_rw_wr1byte_last___data___width 8
#define reg_iop_crc_par_rw_wr1byte_last_offset 36

/* Register rw_wr2byte_last, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_wr2byte_last___data___lsb 0
#define reg_iop_crc_par_rw_wr2byte_last___data___width 16
#define reg_iop_crc_par_rw_wr2byte_last_offset 40

/* Register rw_wr3byte_last, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_wr3byte_last___data___lsb 0
#define reg_iop_crc_par_rw_wr3byte_last___data___width 24
#define reg_iop_crc_par_rw_wr3byte_last_offset 44

/* Register rw_wr4byte_last, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_wr4byte_last___data___lsb 0
#define reg_iop_crc_par_rw_wr4byte_last___data___width 32
#define reg_iop_crc_par_rw_wr4byte_last_offset 48

/* Register r_stat, scope iop_crc_par, type r */
#define reg_iop_crc_par_r_stat___err___lsb 0
#define reg_iop_crc_par_r_stat___err___width 1
#define reg_iop_crc_par_r_stat___err___bit 0
#define reg_iop_crc_par_r_stat___busy___lsb 1
#define reg_iop_crc_par_r_stat___busy___width 1
#define reg_iop_crc_par_r_stat___busy___bit 1
#define reg_iop_crc_par_r_stat_offset 52

/* Register r_sh_reg, scope iop_crc_par, type r */
#define reg_iop_crc_par_r_sh_reg_offset 56

/* Register r_crc, scope iop_crc_par, type r */
#define reg_iop_crc_par_r_crc_offset 60

/* Register rw_strb_rec_dif_in, scope iop_crc_par, type rw */
#define reg_iop_crc_par_rw_strb_rec_dif_in___last___lsb 0
#define reg_iop_crc_par_rw_strb_rec_dif_in___last___width 2
#define reg_iop_crc_par_rw_strb_rec_dif_in_offset 64


/* Constants */
#define regk_iop_crc_par_calc                     0x00000001
#define regk_iop_crc_par_ccitt                    0x00000002
#define regk_iop_crc_par_check                    0x00000000
#define regk_iop_crc_par_crc16                    0x00000001
#define regk_iop_crc_par_crc32                    0x00000000
#define regk_iop_crc_par_crc5                     0x00000003
#define regk_iop_crc_par_crc5_11                  0x00000004
#define regk_iop_crc_par_dif_in                   0x00000002
#define regk_iop_crc_par_hi                       0x00000000
#define regk_iop_crc_par_neg                      0x00000002
#define regk_iop_crc_par_no                       0x00000000
#define regk_iop_crc_par_pos                      0x00000001
#define regk_iop_crc_par_pos_neg                  0x00000003
#define regk_iop_crc_par_rw_cfg_default           0x00000000
#define regk_iop_crc_par_rw_ctrl_default          0x00000000
#define regk_iop_crc_par_yes                      0x00000001
#endif /* __iop_crc_par_defs_asm_h */
