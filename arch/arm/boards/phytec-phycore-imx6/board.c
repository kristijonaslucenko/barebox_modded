/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH,
 * Author: Christian Hemp <c.hemp@phytec.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <envfs.h>
#include <environment.h>
#include <bootsource.h>
#include <common.h>
#include <gpio.h>
#include <init.h>
#include <of.h>
#include <mach/bbu.h>
#include <fec.h>

#include <mach/imx6.h>

static int pcm058_device_init(void)
{
	if (!of_machine_is_compatible("phytec,imx6q-pcm058-nand") &&
		!of_machine_is_compatible("phytec,imx6q-pcm058-emmc"))
		return 0;


	imx6_bbu_nand_register_handler("nand", BBU_HANDLER_FLAG_DEFAULT);

	switch (bootsource_get()) {
	case BOOTSOURCE_MMC:
		if (of_machine_is_compatible("phytec,imx6q-pcm058-emmc"))
			of_device_enable_path("/chosen/environment-emmc");
		else
			of_device_enable_path("/chosen/environment-sd");
		break;
	case BOOTSOURCE_NAND:
		of_device_enable_path("/chosen/environment-nand");
		break;
	default:
		of_device_enable_path("/chosen/environment-spinor");
		break;
	}

	defaultenv_append_directory(defaultenv_phycore_imx6);

	return 0;
}
device_initcall(pcm058_device_init);
