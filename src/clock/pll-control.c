/******************************************************************************
 * Copyright (c) 2016, NXP Semiconductors
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

static void sja1105_cgu_pll_control_access(void *buf, struct sja1105_cgu_pll_control *pll_control, int write)
{
	int (*get_or_set)(void*, uint64_t*, int, int, int);
	int   size = 4;

	if (write == 0) {
		get_or_set = generic_table_field_get;
		memset(pll_control, 0, sizeof(*pll_control));
	} else {
		get_or_set = generic_table_field_set;
		memset(buf, 0, size);
	}
	get_or_set(buf, &pll_control->pllclksrc, 28, 24, 4);
	get_or_set(buf, &pll_control->msel,      23, 16, 4);
	get_or_set(buf, &pll_control->autoblock, 11, 11, 4);
	get_or_set(buf, &pll_control->psel,       9,  8, 4);
	get_or_set(buf, &pll_control->direct,     7,  7, 4);
	get_or_set(buf, &pll_control->fbsel,      6,  6, 4);
	get_or_set(buf, &pll_control->bypass,     1,  1, 4);
	get_or_set(buf, &pll_control->pd,         0,  0, 4);
}

void sja1105_cgu_pll_control_set(void *buf, struct sja1105_cgu_pll_control *pll_control)
{
	sja1105_cgu_pll_control_access(buf, pll_control, 1);
}

void sja1105_cgu_pll_control_get(void *buf, struct sja1105_cgu_pll_control *pll_control)
{
	sja1105_cgu_pll_control_access(buf, pll_control, 0);
}

void sja1105_cgu_pll_control_show(struct sja1105_cgu_pll_control *pll_control)
{
	printf("PLLCLKSEL %" PRIX64 "\n", pll_control->pllclksrc);
	printf("MSEL      %" PRIX64 "\n", pll_control->msel);
	printf("AUTOBLOCK %" PRIX64 "\n", pll_control->autoblock);
	printf("PSEL      %" PRIX64 "\n", pll_control->psel);
	printf("DIRECT    %" PRIX64 "\n", pll_control->direct);
	printf("FBSEL     %" PRIX64 "\n", pll_control->fbsel);
	printf("BYPASS    %" PRIX64 "\n", pll_control->bypass);
	printf("PD        %" PRIX64 "\n", pll_control->pd);
}

int sja1105_cgu_rmii_pll_config(struct spi_setup *spi_setup)
{
	const int MSG_SIZE = SIZE_SPI_MSG_HEADER + 4;
	const int PLL1_OFFSET = 0x0A;
	struct  sja1105_cgu_pll_control pll;
	struct  sja1105_spi_message msg;
	uint8_t tx_buf[MSG_SIZE];
	uint8_t rx_buf[MSG_SIZE];
	int     rc;

	/* PLL1 must be enabled and output 50 Mhz.
	 * This is done by writing first 0x0A010941 to
	 * the PLL_1_C register and then deasserting
	 * power down (PD) 0x0A010940. */

	memset(tx_buf, 0, MSG_SIZE);
	memset(rx_buf, 0, MSG_SIZE);

	/* Header */
	msg.access     = SPI_WRITE;
	msg.read_count = 0;
	msg.address    = CGU_ADDR + PLL1_OFFSET;
	sja1105_spi_message_set(tx_buf, &msg);

	/* Payload */
	pll.pllclksrc = 0xA;
	pll.msel      = 0x1;
	pll.autoblock = 0x1;
	pll.psel      = 0x1;
	pll.direct    = 0x0;
	pll.fbsel     = 0x1;
	pll.bypass    = 0x0;
	pll.pd        = 0x1;

	/* Step 1: PLL1 setup for 50Mhz */
	sja1105_cgu_pll_control_set(tx_buf + 4, &pll);
	rc = spi_transfer(spi_setup, tx_buf, rx_buf, MSG_SIZE);
	if (rc < 0) {
		loge("failed to configure PLL1 for 50MHz");
		goto out;
	}

	/* Step 2: Enable PLL1 */
	pll.pd        = 0x0;

	sja1105_cgu_pll_control_set(tx_buf + 4, &pll);
	rc = spi_transfer(spi_setup, tx_buf, rx_buf, MSG_SIZE);
	if (rc < 0) {
		loge("failed to enable PLL1");
		goto out;
	}
out:
	return rc;
}

