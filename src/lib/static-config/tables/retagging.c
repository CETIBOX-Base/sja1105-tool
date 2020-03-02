/******************************************************************************
 * Copyright (c) 2020, CETiTEC GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/* These are our own include files */
#include <lib/include/static-config.h>
#include <lib/include/gtable.h>
#include <common.h>

static void sja1105_retagging_entry_access(
		void *buf,
		struct sja1105_retagging_entry *entry,
		int write)
{
	int  (*pack_or_unpack)(void*, uint64_t*, int, int, int);
	int    size = SIZE_RETAGGING_ENTRY;

	if (write == 0) {
		pack_or_unpack = gtable_unpack;
		memset(entry, 0, sizeof(*entry));
	} else {
		pack_or_unpack = gtable_pack;
		memset(buf, 0, size);
	}
	pack_or_unpack(buf, &entry->egr_port, 63, 59, size);
	pack_or_unpack(buf, &entry->ing_port, 58, 54, size);
	pack_or_unpack(buf, &entry->vlan_ing, 53, 42, size);
	pack_or_unpack(buf, &entry->vlan_egr, 41, 30, size);
	pack_or_unpack(buf, &entry->do_not_learn, 29, 29, size);
	pack_or_unpack(buf, &entry->use_dest_ports, 28, 28, size);
	pack_or_unpack(buf, &entry->destports, 27, 23, size);
}

DEFINE_COMMON_PACK_UNPACK_ACCESSORS(retagging);

void sja1105_retagging_entry_fmt_show(
		char *print_buf,
		char *fmt,
		struct sja1105_retagging_entry *entry)
{
	formatted_append(print_buf, fmt, "EGR_PORT       0x%" PRIX64, entry->egr_port);
	formatted_append(print_buf, fmt, "ING_PORT       0x%" PRIX64, entry->ing_port);
	formatted_append(print_buf, fmt, "VLAN_ING       0x%" PRIX64, entry->vlan_ing);
	formatted_append(print_buf, fmt, "VLAN_EGR       0x%" PRIX64, entry->vlan_egr);
	formatted_append(print_buf, fmt, "DO_NOT_LEARN   0x%" PRIX64, entry->do_not_learn);
	formatted_append(print_buf, fmt, "USE_DEST_PORTS 0x%" PRIX64, entry->use_dest_ports);
	formatted_append(print_buf, fmt, "DESTPORTS      0x%" PRIX64, entry->destports);
}

void sja1105_retagging_entry_show(struct sja1105_retagging_entry *entry)
{
	char print_buf[MAX_LINE_SIZE];
	char *fmt = "%s\n";
	memset(print_buf, 0, MAX_LINE_SIZE);
	sja1105_retagging_entry_fmt_show(print_buf, fmt, entry);
	puts(print_buf);
}
