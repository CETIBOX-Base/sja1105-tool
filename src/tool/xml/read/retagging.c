/******************************************************************************
 * Copyright (c) 2016, NXP Semiconductors
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
#include "internal.h"

static int entry_get(xmlNode *node, struct sja1105_retagging_entry *entry)
{
	int rc;

	rc = xml_read_field(&entry->egr_port, "egr_port", node);
	if (rc < 0) {
		loge("egr_port read failed");
		goto error;
	}
	rc = xml_read_field(&entry->ing_port, "ing_port", node);
	if (rc < 0) {
		loge("ing_port read failed");
		goto error;
	}
	rc = xml_read_field(&entry->vlan_ing, "vlan_ing", node);
	if (rc < 0) {
		loge("vlan_ing read failed");
		goto error;
	}
	rc = xml_read_field(&entry->vlan_egr, "vlan_egr", node);
	if (rc < 0) {
		loge("vlan_egr read failed");
		goto error;
	}
	rc = xml_read_field(&entry->do_not_learn, "do_not_learn", node);
	if (rc < 0) {
		loge("do_not_learn read failed");
		goto error;
	}
	rc = xml_read_field(&entry->use_dest_ports, "use_dest_ports", node);
	if (rc < 0) {
		loge("use_dest_ports read failed");
		goto error;
	}
	rc = xml_read_field(&entry->destports, "destports", node);
	if (rc < 0) {
		loge("destports read failed");
		goto error;
	}

error:
	if (rc < 0) {
		loge("Retagging entry is incomplete!");
		return -EINVAL;
	}
	return 0;
}

static int parse_entry(xmlNode *node, struct sja1105_static_config *config)
{
	struct sja1105_retagging_entry entry;
	int rc;

	if (config->retagging_count >= MAX_RETAGGING_COUNT) {
		loge("Cannot have more than %d Retagging "
		     "Table entries!", MAX_RETAGGING_COUNT);
		return -1;
	}
	memset(&entry, 0, sizeof(entry));
	rc = entry_get(node, &entry);
	if (rc != 0) {
		goto error;
	}
	config->retagging[config->retagging_count++] = entry;
	return 0;
error:
	return -1;
}

int
retagging_table_parse(xmlNode *node, struct sja1105_static_config *config)
{
	int rc = 0;
	xmlNode *c;

	if (node->type != XML_ELEMENT_NODE) {
		loge("Retagging Table node must be of element type!");
		return -1;
	}
	for (c = node->children; c != NULL; c = c->next) {
		if (c->type != XML_ELEMENT_NODE) {
			continue;
		}
		rc = parse_entry(c, config);
		if (rc < 0) {
			goto out;
		}
	}
	logv("read %d Retagging entries", config->retagging_count);
out:
	return rc;
}


