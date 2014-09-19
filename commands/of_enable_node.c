/*
 * of_enable_node.c - Register a fixup to enable nodes in the device tree
 *
 * Copyright (c) 2014 Teresa Gámez <t.gamez@phytec.de> PHYTEC Messtechnik GmbH
 * Copyright (c) 2014 Wadim Egorov <w.egorov@phytec.de> PHYTEC Messtechnik GmbH
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <of.h>
#include <command.h>
#include <malloc.h>
#include <complete.h>
#include <asm/byteorder.h>
#include <linux/err.h>
#include <string.h>

static int of_enable_node(struct device_node *root, void *context)
{
	char *node_path = (char *)context;
	struct device_node *node;

	node = of_find_node_by_path_from(root, node_path);
	if (!node) {
		pr_err("Could not find node in tree: %s\n", node_path);
		return -EINVAL;
	}

	return of_device_enable(node);
}

static int do_of_enable_node(int argc, char *argv[])
{
	char *node = NULL;

	if (argc < 2)
		return COMMAND_ERROR_USAGE;

	node = xstrdup(argv[1]);
	if (node)
		of_register_fixup(of_enable_node, node);

	return 0;
}

BAREBOX_CMD_HELP_START(of_enable_node)
BAREBOX_CMD_HELP_TEXT("Options:")
BAREBOX_CMD_HELP_OPT("path",  "Node path\n")
BAREBOX_CMD_HELP_END

BAREBOX_CMD_START(of_enable_node)
	.cmd	= do_of_enable_node,
	BAREBOX_CMD_DESC("Register a fixup to enable nodes in the device tree")
	BAREBOX_CMD_OPTS("path")
	BAREBOX_CMD_GROUP(CMD_GRP_MISC)
	BAREBOX_CMD_COMPLETE(devicetree_file_complete)
	BAREBOX_CMD_HELP(cmd_of_enable_node_help)
BAREBOX_CMD_END
