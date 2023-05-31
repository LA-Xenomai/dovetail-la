# SPDX-License-Identifier: GPL-2.0
dtb-$(CONFIG_LOONGSON64_GENERIC)	+= loongson3_ls7a.dtb
dtb-$(CONFIG_LOONGSON_2K500)		+= ls2k500_hl_mb.dtb ls2k500_mini_dp.dtb ls2k500_dayu400_mb.dtb ls2k500_modi_hct.dtb 
dtb-$(CONFIG_LOONGSON_2K1000)		+= ls2k1000_jl_mb.dtb ls2k1000_jl_mb_mu.dtb ls2k1000_jl_mb_nodvo.dtb ls2k1000_dp.dtb ksec_hac_mb.dtb

obj-y				+= $(patsubst %.dtb, %.dtb.o, $(dtb-y))
