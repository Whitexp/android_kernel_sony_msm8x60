/* Copyright (c) 2010-2011, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <mach/msm_bus.h>
#include <mach/msm_bus_board.h>
#include <mach/board.h>
#include <mach/rpm.h>
#include "msm_bus_core.h"

#if 0
#define NMASTERS 39
#define NSLAVES 69
#else
#define NMASTERS 45
#define NSLAVES 75
#endif
#define NFAB_8660 5

enum msm_bus_fabric_tiered_slave_type {
	MSM_BUS_SYSTEM_TIERED_SLAVE_FAB_APPSS = 1,
	MSM_BUS_TIERED_SLAVE_SYSTEM_IMEM,

	MSM_BUS_TIERED_SLAVE_SMI = 1,
	MSM_BUS_MMSS_TIERED_SLAVE_FAB_APPS,
	MSM_BUS_TIERED_SLAVE_MM_IMEM,

	MSM_BUS_TIERED_SLAVE_EBI_CH0 = 1,
	MSM_BUS_TIERED_SLAVE_SMPSS_L2,
};

enum msm_bus_8660_master_ports_type {
	MSM_BUS_SYSTEM_MASTER_PORT_APPSS_FAB = 0,
	MSM_BUS_MASTER_PORT_SPS,
	MSM_BUS_MASTER_PORT_ADM0_PORT0,
	MSM_BUS_MASTER_PORT_ADM0_PORT1,
	MSM_BUS_MASTER_PORT_ADM1_PORT0,
	MSM_BUS_MASTER_PORT_ADM1_PORT1,
	MSM_BUS_MASTER_PORT_LPASS_PROC,
	MSM_BUS_MASTER_PORT_MSS_PROCI,
	MSM_BUS_MASTER_PORT_MSM_MSS_PROCD,
	MSM_BUS_MASTER_PORT_MSM_MDM_PORT0,
	MSM_BUS_MASTER_PORT_LPASS,
	MSM_BUS_SYSTEM_MASTER_PORT_CPSS_FPB,
	MSM_BUS_MASTER_PORT_SYSTEM_FPB,
	MSM_BUS_MASTER_PORT_MMSS_FPB,
	MSM_BUS_MASTER_PORT_ADM1_AHB_CI,
	MSM_BUS_MASTER_PORT_ADM0_AHB_CI,
	MSM_BUS_MASTER_PORT_MSS_MDM_PORT1,

	MSM_BUS_MASTER_PORT_MDP_PORT0 = 0,
	MSM_BUS_MASTER_PORT_MDP_PORT1,
	MSM_BUS_MMSS_MASTER_PORT_ADM1_PORT0,
	MSM_BUS_MASTER_PORT_ROTATOR,
	MSM_BUS_MASTER_PORT_GRAPHICS_3D,
	MSM_BUS_MASTER_PORT_JPEG_DEC,
	MSM_BUS_MASTER_PORT_GRAPHICS_2D_CORE0,
	MSM_BUS_MASTER_PORT_VFE,
	MSM_BUS_MASTER_PORT_VPE,
	MSM_BUS_MASTER_PORT_JPEG_ENC,
	MSM_BUS_MASTER_PORT_GRAPHICS_2D_CORE1,
	MSM_BUS_MMSS_MASTER_PORT_APPS_FAB,
	MSM_BUS_MASTER_PORT_HD_CODEC_PORT0,
	MSM_BUS_MASTER_PORT_HD_CODEC_PORT1,

	MSM_BUS_MASTER_PORT_SMPSS_M0 = 0,
	MSM_BUS_MASTER_PORT_SMPSS_M1,
	MSM_BUS_APPSS_MASTER_PORT_FAB_MMSS,
	MSM_BUS_APPSS_MASTER_PORT_FAB_SYSTEM,

};

static int tier2[] = {MSM_BUS_BW_TIER2,};

enum msm_bus_8660_slave_ports_type {
	MSM_BUS_SLAVE_PORT_SMI = 0,
	MSM_BUS_MMSS_SLAVE_PORT_APPS_FAB_0,
	MSM_BUS_MMSS_SLAVE_PORT_APPS_FAB_1,
	MSM_BUS_SLAVE_PORT_MM_IMEM,

	MSM_BUS_SLAVE_PORT_EBI_CH0 = 0,
	MSM_BUS_SLAVE_PORT_SMPSS_L2,
	MSM_BUS_APPSS_SLAVE_PORT_MMSS_FAB,
	MSM_BUS_SLAVE_PORT_SYSTEM_FAB,

	MSM_BUS_SYSTEM_SLAVE_PORT_APPSS_FAB = 0,
	MSM_BUS_SLAVE_PORT_SPS,
	MSM_BUS_SLAVE_PORT_SYSTEM_IMEM,
	MSM_BUS_SLAVE_PORT_SMPSS,
	MSM_BUS_SLAVE_PORT_MSS,
	MSM_BUS_SLAVE_PORT_LPASS,
	MSM_BUS_SYSTEM_SLAVE_PORT_CPSS_FPB,
	MSM_BUS_SYSTEM_SLAVE_PORT_SYSTEM_FPB,
	MSM_BUS_SLAVE_PORT_MMSS_FPB,
};

static uint32_t master_iids[NMASTERS];
static uint32_t slave_iids[NSLAVES];

static int ampss_m0_mports[] = {MSM_BUS_MASTER_PORT_SMPSS_M0,};
static int ampss_m1_mports[] = {MSM_BUS_MASTER_PORT_SMPSS_M1,};
static int mmss_mport_apps_fab[] = {MSM_BUS_MMSS_MASTER_PORT_APPS_FAB,};
static int system_mport_appss_fab[] = {MSM_BUS_SYSTEM_MASTER_PORT_APPSS_FAB,};

static int sport_ebi_ch0[] = {MSM_BUS_SLAVE_PORT_EBI_CH0,};
static int sport_smpss_l2[] = {MSM_BUS_SLAVE_PORT_SMPSS_L2,};
static int appss_sport_mmss_fab[] = {MSM_BUS_APPSS_SLAVE_PORT_MMSS_FAB,};
static int sport_system_fab[] = {MSM_BUS_SLAVE_PORT_SYSTEM_FAB,};

static int tiered_slave_ebi[] = {MSM_BUS_TIERED_SLAVE_EBI_CH0,};
static int tiered_slave_smpss[] = {MSM_BUS_TIERED_SLAVE_SMPSS_L2,};

static struct msm_bus_node_info apps_fabric_info[] = {
	{
		.id = MSM_BUS_MASTER_AMPSS_M0,
		.masterp = ampss_m0_mports,
		.num_mports = ARRAY_SIZE(ampss_m0_mports),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_AMPSS_M1,
		.masterp = ampss_m1_mports,
		.num_mports = ARRAY_SIZE(ampss_m1_mports),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_SLAVE_EBI_CH0,
		.slavep = sport_ebi_ch0,
		.num_sports = ARRAY_SIZE(sport_ebi_ch0),
		.tier = tiered_slave_ebi,
		.num_tiers = ARRAY_SIZE(tiered_slave_ebi),
		.buswidth = 8,
		.slaveclk[DUAL_CTX] = "mem_clk",
		.slaveclk[ACTIVE_CTX] = "mem_a_clk",
	},
	{
		.id = MSM_BUS_SLAVE_AMPSS_L2,
		.slavep = sport_smpss_l2,
		.num_sports = ARRAY_SIZE(sport_smpss_l2),
		.tier = tiered_slave_smpss,
		.num_tiers = ARRAY_SIZE(tiered_slave_smpss),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_FAB_MMSS,
		.gateway = 1,
		.slavep = appss_sport_mmss_fab,
		.num_sports = ARRAY_SIZE(appss_sport_mmss_fab),
		.masterp = mmss_mport_apps_fab,
		.num_mports = ARRAY_SIZE(mmss_mport_apps_fab),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_FAB_SYSTEM,
		.gateway = 1,
		.slavep = sport_system_fab,
		.num_sports = ARRAY_SIZE(sport_system_fab),
		.masterp = system_mport_appss_fab,
		.num_mports = ARRAY_SIZE(system_mport_appss_fab),
		.buswidth = 8,
	},
};

static int mport_sps[] = {MSM_BUS_MASTER_PORT_SPS,};
static int mport_adm0_port0[] = {MSM_BUS_MASTER_PORT_ADM0_PORT0,};
static int mport_adm0_port1[] = {MSM_BUS_MASTER_PORT_ADM0_PORT1,};
static int mport_adm1_port0[] = {MSM_BUS_MASTER_PORT_ADM1_PORT0,};
static int mport_adm1_port1[] = {MSM_BUS_MASTER_PORT_ADM1_PORT1,};
static int mport_lpass_proc[] = {MSM_BUS_MASTER_PORT_LPASS_PROC,};
static int mport_mss_proci[] = {MSM_BUS_MASTER_PORT_MSS_PROCI,};
static int mport_msm_mss_procd[] = {MSM_BUS_MASTER_PORT_MSM_MSS_PROCD,};
static int mport_mss_mdm_port0[] = {MSM_BUS_MASTER_PORT_MSM_MDM_PORT0,};
static int mport_lpass[] = {MSM_BUS_MASTER_PORT_LPASS,};
static int mport_mmss_fpb[] = {MSM_BUS_MASTER_PORT_MMSS_FPB,};
static int mport_adm1_ahb_ci[] = {MSM_BUS_MASTER_PORT_ADM1_AHB_CI,};
static int mport_adm0_ahb_ci[] = {MSM_BUS_MASTER_PORT_ADM0_AHB_CI,};
static int mport_mss_mdm_port1[] = {MSM_BUS_MASTER_PORT_MSS_MDM_PORT1,};
static int appss_mport_fab_system[] = {MSM_BUS_APPSS_MASTER_PORT_FAB_SYSTEM,};
static int mport_system_fpb[] = {MSM_BUS_MASTER_PORT_SYSTEM_FPB,};
static int system_mport_cpss_fpb[] = {MSM_BUS_SYSTEM_MASTER_PORT_CPSS_FPB,};

static int system_sport_appss_fab[] = {MSM_BUS_SYSTEM_SLAVE_PORT_APPSS_FAB,};
static int system_sport_system_fpb[] = {MSM_BUS_SYSTEM_SLAVE_PORT_SYSTEM_FPB,};
static int system_sport_cpss_fpb[] = {MSM_BUS_SYSTEM_SLAVE_PORT_CPSS_FPB,};
static int sport_sps[] = {MSM_BUS_SLAVE_PORT_SPS,};
static int sport_system_imem[] = {MSM_BUS_SLAVE_PORT_SYSTEM_IMEM,};
static int sport_smpss[] = {MSM_BUS_SLAVE_PORT_SMPSS,};
static int sport_mss[] = {MSM_BUS_SLAVE_PORT_MSS,};
static int sport_lpass[] = {MSM_BUS_SLAVE_PORT_LPASS,};
static int sport_mmss_fpb[] = {MSM_BUS_SLAVE_PORT_MMSS_FPB,};

static int tiered_slave_system_imem[] = {MSM_BUS_TIERED_SLAVE_SYSTEM_IMEM,};
static int system_tiered_slave_fab_appss[] = {
	MSM_BUS_SYSTEM_TIERED_SLAVE_FAB_APPSS,};

static struct msm_bus_node_info system_fabric_info[]  = {
	{
		.id = MSM_BUS_MASTER_SPS,
		.masterp = mport_sps,
		.num_mports = ARRAY_SIZE(mport_sps),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_ADM_PORT0,
		.masterp = mport_adm0_port0,
		.num_mports = ARRAY_SIZE(mport_adm0_port0),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_ADM_PORT1,
		.masterp = mport_adm0_port1,
		.num_mports = ARRAY_SIZE(mport_adm0_port1),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_SYSTEM_MASTER_ADM1_PORT0,
		.masterp = mport_adm1_port0,
		.num_mports = ARRAY_SIZE(mport_adm1_port0),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_ADM1_PORT1,
		.masterp = mport_adm1_port1,
		.num_mports = ARRAY_SIZE(mport_adm1_port1),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_LPASS_PROC,
		.masterp = mport_lpass_proc,
		.num_mports = ARRAY_SIZE(mport_lpass_proc),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_MSS_PROCI,
		.masterp = mport_mss_proci,
		.num_mports = ARRAY_SIZE(mport_mss_proci),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_MSS_PROCD,
		.masterp = mport_msm_mss_procd,
		.num_mports = ARRAY_SIZE(mport_msm_mss_procd),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_MSS_MDM_PORT0,
		.masterp = mport_mss_mdm_port0,
		.num_mports = ARRAY_SIZE(mport_mss_mdm_port0),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_LPASS,
		.masterp = mport_lpass,
		.num_mports = ARRAY_SIZE(mport_lpass),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_SYSTEM_MASTER_MMSS_FPB,
		.masterp = mport_mmss_fpb,
		.num_mports = ARRAY_SIZE(mport_mmss_fpb),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_ADM1_CI,
		.masterp = mport_adm1_ahb_ci,
		.num_mports = ARRAY_SIZE(mport_adm1_ahb_ci),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_ADM0_CI,
		.masterp = mport_adm0_ahb_ci,
		.num_mports = ARRAY_SIZE(mport_adm0_ahb_ci),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_MSS_MDM_PORT1,
		.masterp = mport_mss_mdm_port1,
		.num_mports = ARRAY_SIZE(mport_mss_mdm_port1),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_FAB_APPSS,
		.gateway = 1,
		.slavep = system_sport_appss_fab,
		.num_sports = ARRAY_SIZE(system_sport_appss_fab),
		.masterp = appss_mport_fab_system,
		.num_mports = ARRAY_SIZE(appss_mport_fab_system),
		.tier = system_tiered_slave_fab_appss,
		.num_tiers = ARRAY_SIZE(system_tiered_slave_fab_appss),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_FAB_SYSTEM_FPB,
		.gateway = 1,
		.slavep = system_sport_system_fpb,
		.num_sports = ARRAY_SIZE(system_sport_system_fpb),
		.masterp = mport_system_fpb,
		.num_mports = ARRAY_SIZE(mport_system_fpb),
		.buswidth = 4,
	},
	{
		.id = MSM_BUS_FAB_CPSS_FPB,
		.gateway = 1,
		.slavep = system_sport_cpss_fpb,
		.num_sports = ARRAY_SIZE(system_sport_cpss_fpb),
		.masterp = system_mport_cpss_fpb,
		.num_mports = ARRAY_SIZE(system_mport_cpss_fpb),
		.buswidth = 4,
	},
	{
		.id = MSM_BUS_SLAVE_SPS,
		.slavep = sport_sps,
		.num_sports = ARRAY_SIZE(sport_sps),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_SLAVE_SYSTEM_IMEM,
		.slavep = sport_system_imem,
		.num_sports = ARRAY_SIZE(sport_system_imem),
		.tier = tiered_slave_system_imem,
		.num_tiers = ARRAY_SIZE(tiered_slave_system_imem),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_SLAVE_AMPSS,
		.slavep = sport_smpss,
		.num_sports = ARRAY_SIZE(sport_smpss),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_SLAVE_MSS,
		.slavep = sport_mss,
		.num_sports = ARRAY_SIZE(sport_mss),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_SLAVE_LPASS,
		.slavep = sport_lpass,
		.num_sports = ARRAY_SIZE(sport_lpass),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_SYSTEM_SLAVE_MMSS_FPB,
		.slavep = sport_mmss_fpb,
		.num_sports = ARRAY_SIZE(sport_mmss_fpb),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
		.buswidth = 8,
	},
};

static int mport_mdp_port0[] = {MSM_BUS_MASTER_PORT_MDP_PORT0,};
static int mport_mdp_port1[] = {MSM_BUS_MASTER_PORT_MDP_PORT1,};
static int mmss_mport_adm1_port0[] = {MSM_BUS_MMSS_MASTER_PORT_ADM1_PORT0,};
static int mport_rotator[] = {MSM_BUS_MASTER_PORT_ROTATOR,};
static int mport_graphics_3d[] = {MSM_BUS_MASTER_PORT_GRAPHICS_3D,};
static int mport_jpeg_dec[] = {MSM_BUS_MASTER_PORT_JPEG_DEC,};
static int mport_graphics_2d_core0[] = {MSM_BUS_MASTER_PORT_GRAPHICS_2D_CORE0,};
static int mport_vfe[] = {MSM_BUS_MASTER_PORT_VFE,};
static int mport_vpe[] = {MSM_BUS_MASTER_PORT_VPE,};
static int mport_jpeg_enc[] = {MSM_BUS_MASTER_PORT_JPEG_ENC,};
static int mport_graphics_2d_core1[] = {MSM_BUS_MASTER_PORT_GRAPHICS_2D_CORE1,};
static int mport_hd_codec_port0[] = {MSM_BUS_MASTER_PORT_HD_CODEC_PORT0,};
static int mport_hd_codec_port1[] = {MSM_BUS_MASTER_PORT_HD_CODEC_PORT1,};
static int appss_mport_fab_mmss[] = {MSM_BUS_APPSS_MASTER_PORT_FAB_MMSS,};

static int sport_smi[] = {MSM_BUS_SLAVE_PORT_SMI,};
static int mmss_sport_apps_fab_0[] = {MSM_BUS_MMSS_SLAVE_PORT_APPS_FAB_0,};
static int sport_mm_imem[] = {MSM_BUS_SLAVE_PORT_MM_IMEM,};

static int tiered_slave_smi[] = {MSM_BUS_TIERED_SLAVE_SMI,};
static int mmss_tiered_slave_fab_apps[] = {MSM_BUS_MMSS_TIERED_SLAVE_FAB_APPS,};
static int tiered_slave_mm_imem[] = {MSM_BUS_TIERED_SLAVE_MM_IMEM,};

static struct msm_bus_node_info mmss_fabric_info[]  = {
	{
		.id = MSM_BUS_MASTER_MDP_PORT0,
		.masterp = mport_mdp_port0,
		.num_mports = ARRAY_SIZE(mport_mdp_port0),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_MDP_PORT1,
		.masterp = mport_mdp_port1,
		.num_mports = ARRAY_SIZE(mport_mdp_port1),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MMSS_MASTER_ADM1_PORT0,
		.masterp = mmss_mport_adm1_port0,
		.num_mports = ARRAY_SIZE(mmss_mport_adm1_port0),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_ROTATOR,
		.masterp = mport_rotator,
		.num_mports = ARRAY_SIZE(mport_rotator),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_GRAPHICS_3D,
		.masterp = mport_graphics_3d,
		.num_mports = ARRAY_SIZE(mport_graphics_3d),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_JPEG_DEC,
		.masterp = mport_jpeg_dec,
		.num_mports = ARRAY_SIZE(mport_jpeg_dec),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_GRAPHICS_2D_CORE0,
		.masterp = mport_graphics_2d_core0,
		.num_mports = ARRAY_SIZE(mport_graphics_2d_core0),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_VFE,
		.masterp = mport_vfe,
		.num_mports = ARRAY_SIZE(mport_vfe),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_VPE,
		.masterp = mport_vpe,
		.num_mports = ARRAY_SIZE(mport_vpe),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_JPEG_ENC,
		.masterp = mport_jpeg_enc,
		.num_mports = ARRAY_SIZE(mport_jpeg_enc),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	/* This port has been added for V2. It is absent in V1 */
	{
		.id = MSM_BUS_MASTER_GRAPHICS_2D_CORE1,
		.masterp = mport_graphics_2d_core1,
		.num_mports = ARRAY_SIZE(mport_graphics_2d_core1),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_HD_CODEC_PORT0,
		.masterp = mport_hd_codec_port0,
		.num_mports = ARRAY_SIZE(mport_hd_codec_port0),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_HD_CODEC_PORT1,
		.masterp = mport_hd_codec_port1,
		.num_mports = ARRAY_SIZE(mport_hd_codec_port1),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_SLAVE_SMI,
		.slavep = sport_smi,
		.num_sports = ARRAY_SIZE(sport_smi),
		.tier = tiered_slave_smi,
		.num_tiers = ARRAY_SIZE(tiered_slave_smi),
		.buswidth = 16,
		.slaveclk[DUAL_CTX] = "smi_clk",
		.slaveclk[ACTIVE_CTX] = "smi_a_clk",
	},
	{
		.id = MSM_BUS_MMSS_SLAVE_FAB_APPS_1,
		.slavep = mmss_sport_apps_fab_0,
		.num_sports = ARRAY_SIZE(mmss_sport_apps_fab_0),
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
		.buswidth = 8,
	},
	{
		.id = MSM_BUS_FAB_APPSS,
		.gateway = 1,
		.slavep = mmss_sport_apps_fab_0,
		.num_sports = ARRAY_SIZE(mmss_sport_apps_fab_0),
		.masterp = appss_mport_fab_mmss,
		.num_mports = ARRAY_SIZE(appss_mport_fab_mmss),
		.tier = mmss_tiered_slave_fab_apps,
		.num_tiers = ARRAY_SIZE(mmss_tiered_slave_fab_apps),
		.buswidth = 16,
	},
	{
		.id = MSM_BUS_SLAVE_MM_IMEM,
		.slavep = sport_mm_imem,
		.num_sports = ARRAY_SIZE(sport_mm_imem),
		.tier = tiered_slave_mm_imem,
		.num_tiers = ARRAY_SIZE(tiered_slave_mm_imem),
		.buswidth = 8,
	},
};

static struct msm_bus_node_info sys_fpb_fabric_info[]  = {
	{
		.id = MSM_BUS_FAB_SYSTEM,
		.gateway = 1,
		.slavep = system_sport_system_fpb,
		.num_sports = ARRAY_SIZE(system_sport_system_fpb),
		.masterp = mport_system_fpb,
		.num_mports = ARRAY_SIZE(mport_system_fpb),
		.buswidth = 4,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_MASTER_SPDM,
		.ahb = 1,
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_MASTER_RPM,
		.ahb = 1,
		.tier = tier2,
		.num_tiers = ARRAY_SIZE(tier2),
	},
	{
		.id = MSM_BUS_SLAVE_SPDM,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_RPM,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_RPM_MSG_RAM,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_MPM,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_PMIC1_SSBI1_A,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_PMIC1_SSBI1_B,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_PMIC1_SSBI1_C,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_PMIC2_SSBI2_A,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_PMIC2_SSBI2_B,
		.buswidth = 4,
		.ahb = 1,
	},
};

static struct msm_bus_node_info cpss_fpb_fabric_info[] = {
	{
		.id = MSM_BUS_FAB_SYSTEM,
		.gateway = 1,
		.slavep = system_sport_cpss_fpb,
		.num_sports = ARRAY_SIZE(system_sport_cpss_fpb),
		.masterp = system_mport_cpss_fpb,
		.num_mports = ARRAY_SIZE(system_mport_cpss_fpb),
		.buswidth = 4,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI1_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI2_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI3_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI4_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI5_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI6_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI7_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI8_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI9_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI10_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI11_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI12_UART,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI1_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI2_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI3_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI4_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI5_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI6_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI7_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI8_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI9_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI10_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI11_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_GSBI12_QUP,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_EBI2_NAND,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_EBI2_CS0,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_EBI2_CS1,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_EBI2_CS2,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_EBI2_CS3,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_EBI2_CS4,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_EBI2_CS5,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_USB_FS1,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_USB_FS2,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_TSIF,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_MSM_TSSC,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_MSM_PDM,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_MSM_DIMEM,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_MSM_TCSR,
		.buswidth = 8,
		.ahb = 1,
	},
	{
		.id = MSM_BUS_SLAVE_MSM_PRNG,
		.buswidth = 4,
		.ahb = 1,
	},
};

static void msm_bus_board_assign_iids(struct msm_bus_fabric_registration
	*fabreg, int fabid)
{
	int i;
	for (i = 0; i < fabreg->len; i++) {
		if (!fabreg->info[i].gateway) {
			fabreg->info[i].priv_id = fabid + fabreg->info[i].id;
			if (fabreg->info[i].id < SLAVE_ID_KEY)
				master_iids[fabreg->info[i].id] =
					fabreg->info[i].priv_id;
			else
				slave_iids[fabreg->info[i].id - (SLAVE_ID_KEY)]
					= fabreg->info[i].priv_id;
		} else
			fabreg->info[i].priv_id = fabreg->info[i].id;
	}
}

static int msm_bus_board_8660_get_iid(int id)
{
	if ((id < SLAVE_ID_KEY && id >= NMASTERS) ||
		id >= (SLAVE_ID_KEY + NSLAVES)) {
		MSM_BUS_ERR("Cannot get iid. Invalid id %d passed\n", id);
		return -EINVAL;
	}

	return CHECK_ID(((id < SLAVE_ID_KEY) ? master_iids[id] :
		slave_iids[id - SLAVE_ID_KEY]), id);
}

static struct msm_bus_board_algorithm msm_bus_board_algo = {
	.board_nfab = NFAB_8660,
	.get_iid = msm_bus_board_8660_get_iid,
	.assign_iids = msm_bus_board_assign_iids,
};

struct msm_bus_fabric_registration msm_bus_apps_fabric_pdata = {
	.id = MSM_BUS_FAB_APPSS,
	.name = "msm_apps_fab",
	.info = apps_fabric_info,
	.len = ARRAY_SIZE(apps_fabric_info),
	.ahb = 0,
	.fabclk[DUAL_CTX] = "bus_clk",
	.fabclk[ACTIVE_CTX] = "bus_a_clk",
	.haltid = MSM_RPM_ID_APPS_FABRIC_HALT_0,
	.offset = MSM_RPM_ID_APPS_FABRIC_ARB_0,
	.nmasters = 4,
	.nslaves = 4,
	.ntieredslaves = 2,
	.board_algo = &msm_bus_board_algo,
};

struct msm_bus_fabric_registration msm_bus_sys_fabric_pdata = {
	.id = MSM_BUS_FAB_SYSTEM,
	.name = "msm_sys_fab",
	system_fabric_info,
	ARRAY_SIZE(system_fabric_info),
	.ahb = 0,
	.fabclk[DUAL_CTX] = "bus_clk",
	.fabclk[ACTIVE_CTX] = "bus_a_clk",
	.haltid = MSM_RPM_ID_SYSTEM_FABRIC_HALT_0,
	.offset = MSM_RPM_ID_SYSTEM_FABRIC_ARB_0,
	.nmasters = 17,
	.nslaves = 9,
	.ntieredslaves = 2,
	.board_algo = &msm_bus_board_algo,
};

struct msm_bus_fabric_registration msm_bus_mm_fabric_pdata = {
	.id = MSM_BUS_FAB_MMSS,
	.name = "msm_mm_fab",
	mmss_fabric_info,
	ARRAY_SIZE(mmss_fabric_info),
	.ahb = 0,
	.fabclk[DUAL_CTX] = "bus_clk",
	.fabclk[ACTIVE_CTX] = "bus_a_clk",
	.haltid = MSM_RPM_ID_MM_FABRIC_HALT_0,
	.offset = MSM_RPM_ID_MM_FABRIC_ARB_0,
	.nmasters = 14,
	.nslaves = 4,
	.ntieredslaves = 3,
	.board_algo = &msm_bus_board_algo,
};

struct msm_bus_fabric_registration msm_bus_sys_fpb_pdata = {
	.id = MSM_BUS_FAB_SYSTEM_FPB,
	.name = "msm_sys_fpb",
	sys_fpb_fabric_info,
	ARRAY_SIZE(sys_fpb_fabric_info),
	.ahb = 1,
	.fabclk[DUAL_CTX] = "bus_clk",
	.fabclk[ACTIVE_CTX] = "bus_a_clk",
	.nmasters = 0,
	.nslaves = 0,
	.ntieredslaves = 0,
	.board_algo = &msm_bus_board_algo,
};

struct msm_bus_fabric_registration msm_bus_cpss_fpb_pdata = {
	.id = MSM_BUS_FAB_CPSS_FPB,
	.name = "msm_cpss_fpb",
	cpss_fpb_fabric_info,
	ARRAY_SIZE(cpss_fpb_fabric_info),
	.ahb = 1,
	.fabclk[DUAL_CTX] = "bus_clk",
	.fabclk[ACTIVE_CTX] = "bus_a_clk",
	.nmasters = 0,
	.nslaves = 0,
	.ntieredslaves = 0,
	.board_algo = &msm_bus_board_algo,
};

int msm_bus_board_rpm_get_il_ids(uint16_t id[])
{
	return -ENXIO;
}
