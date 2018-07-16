#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include"include/arp.h"
#include"include/ip.h"
#include"include/ethernet.h"

#define ARP_TABLE_SIZE 4096

struct arp_entry {
	//ip_addr_t pa;
	uint32_t pa;
	ethernet_addr ha;
	time_t timestamp;
	//pthread_cond_t cond;
	void *data;
	size_t size;
	struct arp_entry *next;
};

static struct {
	time_t timestamp;
	struct {
		struct arp_entry table[ARP_TABLE_SIZE];
		struct arp_entry *head;
		struct arp_entry *pool;
	} table;
	//pthread_mutex_t mutex;
} arp_table;

void arp_init() {
	int i;
	struct arp_entry *entry;
	
	time(&arp_table.timestamp);
	for (i = 0; i < ARP_TABLE_SIZE; i++) {
		entry = arp_table.table.table + i;
		entry->pa = 0;
		entry->timestamp = 0;
		memset(&entry->ha, 0, sizeof(ethernet_addr));
		//pthread_cond_init(&entry->cond, NULL);
		entry->data = NULL;
		entry->size = 0;
		entry->next = (i != ARP_TABLE_SIZE) ? (entry + 1) : NULL;
	}
	arp_table.table.head = NULL;
	arp_table.table.pool = arp_table.table.table;
	//pthread_mutex_init(&arp.mutex, NULL);
	return 0;
}

int arp_table_select(const uint32_t *pa, ethernet_addr *ha) {
	struct arp_entry *entry;
	for (entry = arp_table.table.head; entry; entry = entry->next) {
		if (entry->pa == *pa) {
			memcpy(ha, &entry->ha, sizeof(ethernet_addr));
			return 0;
		}
	}
	return -1;
}

int arp_resolve(const uint32_t *pa, ethernet_addr *ha, const void *data, uint32_t size) {
#if 1	/* test */
	ha->addr[0] = 0xff;
	ha->addr[1] = 0xff;
	ha->addr[2] = 0xff;
	ha->addr[3] = 0xff;
	ha->addr[4] = 0xff;
	ha->addr[5] = 0x0f;
	return 1;
#else
	struct arp_entry *entry;
	
	//pthread_mutex_lock(&arp.mutex);
	if (arp_table_select(pa, ha) == 0) {
	    //pthread_mutex_unlock(&arp.mutex);
	    return 1;
	}
	/* If it does not exist in arp_table, save the data in the table and send arp_req. */
	if (!data) {
	    //pthread_mutex_unlock(&arp.mutex);
	    return -1;
	}
	entry = arp_table.table.pool;
	if (!entry) {
	    //pthread_mutex_unlock(&arp.mutex);
	    return -1;
	}
	entry->data = malloc(size);
	if (!entry->data) {
	    //pthread_mutex_unlock(&arp.mutex);
	    return -1;
	}
	memcpy(entry->data, data, size);
	entry->size = size;
	arp_table.table.pool = entry->next;
	entry->next = arp_table.table.head;
	arp_table.table.head = entry;
	entry->pa = *pa;
	time(&entry->timestamp);
	send_req(pa);
	//pthread_mutex_unlock(&arp.mutex);
	return  0;
#endif
}

void send_req() {

}

void send_rep() {

}

void tx_arp() {

}

void rx_arp(uint8_t *packet, uint32_t size, struct port_config *port) {
	struct arp_ether *hdr;
	
	if (size < sizeof(struct arp_ether)) 
		return;

	hdr = (struct arp_ether *)packet;
	if (ntohs(hdr->arphdr.hrd_type) != ARP_HRD_ETHERNET) return;
	if (ntohs(hdr->arphdr.proto_type) != ETHERTYPE_IP) return;
	if (hdr->arphdr.hrd_len != ETHER_ADDR_LEN) return;
	if (hdr->arphdr.proto_len != IP_ADDR_LEN) return;

	//arp req or rep
	switch(hdr->arphdr.ar_op) {
		case ARPOP_REQUEST:
		{
			printf("arp req\n");
			break;
		}
		case ARPOP_REPLY:
		{
			printf("arp rep\n");
			break;
		}
		default:
		{
			printf("arp uooooooooooooooooo\n");
			break;
		}
	}


	return;
}
