#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <rte_mbuf.h>
#include <rte_ether.h>

#include"include/pkt_io.h"



void get_macaddr (ethernet_addr *addr) {
	struct ether_addr rte_addr;
}

void tx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/void){
	//struct queue_pkt queue_pkt = rx_queue_pop()
	//uint8_t *p = rte_pktmbuf_mtod(mbuf, uint8_t*);
}

void rx_ether(/*struct rte_mbuf *mbuf, uint32_t size*/void){
	uint32_t pop_size;
	int rx_pop_num;

	struct pkt_queue pkt_queue = rx_queue_pop(&pop_size);

	uint32_t *p = rte_pktmbuf_mtod(bufs[i], uint8_t*);
	rte_hexdump(stdout, "", (const void *)p, pop_size);

	return 0;
}
