#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <rte_eal.h>
#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_hexdump.h>
#include <rte_ether.h>

struct queue_info{
	int num;
	struct pkt_queue *head;
	struct pkt_queue *tail;
};
struct pkt_queue {
	struct rte_mbuf *mbuf;	//uint8_t?
	size_t size;
	struct pkt_queue *next;
};

struct queue_info tx_queue;
struct queue_info rx_queue;

int queue_init() {
	tx_queue.num = 0;
	tx_queue.head = NULL;
	rx_queue.num = 0;
	rx_queue.head = NULL;
}

void tx_queue_push(struct rte_mbuf *mbuf) {
	tx_queue.num += 1;
	struct pkt_queue *pkt = (struct pkt_queue *)malloc(sizeof(struct pkt_queue));
	pkt->mbuf = mbuf;
	pkt->size = rte_pktmbuf_pkt_len(mbuf);
	if (tx_queue.head == NULL){
		tx_queue.head = pkt;
		tx_queue.tail = pkt;
	}

	tail->next = pkt;
	tail = pkt;
}

struct rte_mbuf* tx_queue_pop() {
	if (tx_queue.head == NULL) {
		return NULL;
	}
	tx_queue.num -= 1;

	struct rte_mbuf *ret = tx_queue.head->mbuf;
	struct pkt_queue *dust = tx_queue.head;
	tx_queue.head = tx_queue.head->next;
	free(dust);
	//if head is NULL, tail can be anything
	return ret;
}

void rx_queue_push(struct rte_buf *mbuf) {
	rx_queue.num += 1;
	struct pkt_queue *pkt = (struct pkt_queue *)malloc(sizeof(struct pkt_queue));
	pkt->mbuf = mbuf;
	if (tx_queue.head == NULL){
		rx_queue.head = pkt;
		rx_queue.tail = pkt;
	}

	tail->next = pkt;
	tail = pkt;
}

struct rte_mbuf* rx_queue_pop() {
	if (rx_queue.head == NULL) {
		return NULL;
	}
	rx_queue.num -= 1;

	struct rte_mbuf *ret = rx_queue.head->mbuf;
	struct pkt_queue *dust = rx_queue.head;
	rx_queue.head = rx_queue.head->next;
	free(dust);
	//if head is NULL, tail can be anything
	return ret;
}


struct port {
  uint8_t port_num;
}

struct rte_mempool *mbuf_pool;

static const struct rte_eth_conf port_conf_default = {
	.rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};


/*
 * Initializes a given port using global settings and with the RX buffers
 * coming from the mbuf_pool passed as a parameter.
 */
static int
port_init(uint16_t port)
{ 
	struct rte_eth_conf port_conf = port_conf_default;
	const uint16_t rx_rings = 1, tx_rings = 1;
	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	int retval;
	uint16_t q;
	struct ether_addr addr;
	
	if (port >= rte_eth_dev_count()) {
		return -1;
	}
	
	/* Configure the Ethernet port. */ 
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0) {
		return retval;
	}
	
	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
	if (retval != 0) {
		return retval;
	}
	
	/* Allocate and set up 1 RX queue per Ethernet port. */
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, nb_rxd, rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0) {
			return retval;
		}
	}
	
	/* Allocate and set up 1 TX queue per Ethernet port. */
	for (q = 0; q < tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(port, q, nb_txd, rte_eth_dev_socket_id(port), NULL);
		if (retval < 0) {
			return retval;
		}
	}
	
	/* Start the Ethernet port. */
	retval = rte_eth_dev_start(port);
	if (retval < 0) {
		return retval;
	}
  
	/* Display the port MAC address. */
	rte_eth_macaddr_get(port, &addr);
	printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
		port,
		addr.addr_bytes[0], addr.addr_bytes[1],
		addr.addr_bytes[2], addr.addr_bytes[3],
		addr.addr_bytes[4], addr.addr_bytes[5]);
	
	/* Enable RX in promiscuous mode for the Ethernet port. */
	rte_eth_promiscuous_enable(port);
  
	return 0;
}

int
dpdk_init(void){
	int ret;
	unsigned nb_ports;
	uint16_t portid;  
	char **pg_name;

	pg_name = malloc(sizeof(char *));
	*pg_name = "kkk";
	ret = rte_eal_init(1, pg_name);
	if (ret < 0) {
		return -1;
	}
  
	nb_ports = rte_eth_dev_count();
	if (nb_ports != 1) {
		return -1;
	}
	
	/* Creates a new mempool in memory to hold the mbufs. */
	mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports, MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
  
	if (mbuf_pool == NULL) {
		return -1;
	}
  
	return 0;
}


/* Wrapped a function to control port but not practically meaningful. It is expected that name will be assigned port number. */
port 
*port_open (const char *name) {
	port *port;
	if ((port = malloc(sizeof(*port))) == NULL) {
		perror("malloc");
		return NULL;
	}
	port->port_num = (uint16_t)atoi(name);
	
	if (port_init(port->port_num) < 0){
		free(port);
		return NULL;
	}
  
	return port;
}

void 
port_close (port *port) {
	printf("close port %u\n", port->port_num);
	rte_eth_dev_stop(port->port_num);
	rte_eth_dev_close(port->port_num);
	free(port);
}

void
rx_pkt (port *port) {
	uint16_t nb_ports;
	uint16_t nport = port->port_num;
	struct rte_mbuf *bufs[BURST_SIZE];
	uint16_t nb_rx;
	nb_ports = rte_eth_dev_count();

	/* Recv burst of RX packets */
	nb_rx = rte_eth_rx_burst(nport, 0, bufs, BURST_SIZE);
	int i;
	for (i = 0; i < nb_rx ; i++) {
		uint8_t *p = rte_pktmbuf_mtod(bufs[i], uint8_t*);
		//size_t size = rte_pktmbuf_pkt_len(bufs[i]);
  
		tx_queue_push(bufs[i]);
	}
}

size_t
tx_pkt (port *port) {
	struct rte_mbuf *bufs[BURST_SIZE];
	uint16_t nb_ports;
	uint16_t nport = port->port_num;
	uint8_t *p;
	nb_ports = rte_eth_dev_count();
 
  int i;
	for (i = 0; i < BURST_SIZE; i++){
		bufs[i] = tx_queue_pop();
		if (bufs[i] == NULL){
			break;
		}
	}
	/* Send burst of TX packets */
	//bufs[0] = rte_pktmbuf_alloc(mbuf_pool);
	//bufs[0]->pkt_len = length;
	//bufs[0]->data_len = length;
	//bufs[0]->port = nport;
	//bufs[0]->packet_type = 1;
  //
	//p = rte_pktmbuf_mtod(bufs[0], uint8_t*);
	//memcpy(p, buffer, length);
  
	size_t num_tx;
	num_tx = rte_eth_tx_burst(nport, 0, bufs, 1);
	if (num_tx < i){
		for (uint16_t j = 0; j < i - num_tx; j++){
			rte_pktmbuf_free(bufs[num_tx + j]);
		}
	}
	if (num_pkt > 0) {
		return num_tx;
	}
	return -1;
}

