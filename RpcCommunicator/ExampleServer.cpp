/******************************************************************************
 *
 * Copyright (C) 2016 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "lwip/sockets.h"
#include "netif/xadapter.h"
#include "lwipopts.h"
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"

#define THREAD_STACKSIZE 1024

	u16_t server_port = 5001;

void print_echo_app_header() {
	xil_printf("Test server running on port: %6d\r\n", server_port);

}

/* thread spawned for each connection */
void process_test_add(void* p) {
	int sd = (int)p;
	//	int RECV_BUF_SIZE = 64;
	//	int RECV_BUF_SIZE = 2048;
	//	int RECV_BUF_SIZE = 64 * 1024 * 1024;
	int RECV_BUF_SIZE = 128 * 1024 * 1024;
	//	int max_transfers = 1;
	int max_transfers = 16;
	int curr_tr;
	int bytes_left;
	int status;
	int* recv_buf_int = memalign(16, RECV_BUF_SIZE);
	int* snd_buf_int = recv_buf_int;
	char* buff;
	int n, nwrote;

	while (1) {
		//start receiving data
		for (curr_tr = 0; curr_tr < max_transfers; curr_tr++) {
			bytes_left = RECV_BUF_SIZE;
			buff = (char*)recv_buf_int;
			status = 0;
			//receive packets until all data arrived
			while (bytes_left > 0) {
				if ((n = read(sd, buff, bytes_left)) < 0) {
					xil_printf(
						"%s: error reading from socket %d, closing socket\r\n",
						__FUNCTION__, sd);
					status = -1;
					break;
				}
				if (n <= 0) {
					xil_printf("Connection closed.\r\n");
					status = -2;
					break;
				}
				bytes_left -= n;
				buff += n;
				//xil_printf("Packet received, length: %d bytes left: %d bytes\r\n",
				//		n, bytes_left);
			}
			if (status < 0) {
				break;
			}
		}
		if (status < 0) {
			break;
		}

		xil_printf("Computing\r\n");
		for (int i = 0; i < RECV_BUF_SIZE / 4; i++) {
			snd_buf_int[i] = recv_buf_int[i] + 2;
			//xil_printf("recv_buf[%d] %d, snd_buf[%d] %d\r\n", i,
		}
		xil_printf("Finished\r\n");

		//star sending back
		for (curr_tr = 0; curr_tr < max_transfers; curr_tr++) {
			bytes_left = RECV_BUF_SIZE;
			buff = (char*)snd_buf_int;
			status = 0;
			//send packets until no data left
			while (bytes_left > 0) {
				//send packet
				if ((nwrote = write(sd, buff, bytes_left)) < 0) {
					xil_printf(
						"%s: ERROR responding to client echo request. received = %d, written = %d\r\n",
						__FUNCTION__, n, nwrote);
					xil_printf("Closing socket %d\r\n", sd);
					status = -3;
					break;
				}
				bytes_left -= nwrote;
				buff += nwrote;
				//xil_printf("%d bytes sent back left: %d bytes\r\n", nwrote,
				//		bytes_left);
			}
			if (status < 0) {
				break;
			}
		}
		if (status < 0) {
			break;
		}

	}
	free(recv_buf_int);
	xil_printf("Memory freed.\r\n");
	/* close connection */
	close(sd);
	vTaskDelete(NULL);
}

void echo_application_thread() {
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(server_port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
		return;

	lwip_listen(sock, 0);

	size = sizeof(remote);

	while (1) {
		if ((new_sd = lwip_accept(sock, (struct sockaddr*)&remote,
			(socklen_t*)&size)) > 0) {
			sys_thread_new("test_add", process_test_add, (void*)new_sd,
				THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
		}
	}
}