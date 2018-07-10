#ifndef __PKT_IO_H_
#define __PKT_IO_H

#define BURST_SIZE 32
#define RX_RING_SIZE 128
#define TX_RING_SIZE 512
#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 32

#include<pthread.h>

extern struct rte_mempool *mbuf_pool;

struct pkt_queue {
	struct rte_mbuf *mbuf;
	uint32_t size;
	struct pkt_queue *next;
};
struct queue_info {
	uint32_t num;
	struct pkt_queue *head;
	struct pkt_queue *tail;
	pthread_mutex_t mutex;
};
struct port {
	uint8_t port_num;
};

extern struct queue_info tx_queue;
extern struct queue_info rx_queue;

//void queue_init();
void tx_queue_push(struct rte_mbuf *mbuf, uint32_t size);
//struct rte_mbuf* tx_queue_pop();
//void rx_queue_push(struct rte_mbuf *mbuf, uint32_t size);
struct rte_mbuf* rx_queue_pop(uint32_t *size);
int dpdk_init(void);
int port_init(/*uint16_t port*/struct port_config *port);
struct port *port_open(uint8_t num);
//void port_close(struct port *port);
//void rx_pkt(struct port *port);
//size_t tx_pkt(struct port *port);

int launch_lcore_rxtx(void *arg);


#endif
