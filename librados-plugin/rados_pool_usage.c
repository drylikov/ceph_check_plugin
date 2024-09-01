#include "rados/librados.h"
#include "rados/rados_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
* Author: atrmat@gmail.com
* Ceph check-plugin
* Copyright (c) 2013
* All rights reserved.
* 
* function: query the pools usage statistics
*/
int main(int args, char* argv[]){
    int err;
    rados_t cluster;
    rados_ioctx_t pool_io;
    struct rados_pool_stat_t pool_stats;
    //create a cluster handle
    err = rados_create(&cluster,NULL);
    if (err < 0) {
        fprintf(stderr, "%s: cannot create a cluster handle: %s\n", argv[0], strerror(-err));
        exit(1);
    }
    //read config file 
    err = rados_conf_read_file(cluster, "/etc/ceph/ceph.conf");
    if (err < 0) {
        fprintf(stderr, "%s: cannot read config file: %s\n", argv[0], strerror(-err));
        exit(1);
    }
    //connect to cluster
    err = rados_connect(cluster);
    if (err < 0) {
        fprintf(stderr, "%s: cannot connect to cluster: %s\n", argv[0], strerror(-err));
        exit(1);
    }
    char pools_name[200];
    //list all pools in cluster
    err = rados_pool_list(cluster,&pools_name,200);
    if(err < 0){
        fprintf(stderr, "%s: cannot connect to cluster: %s\n", argv[0], strerror(-err));
        exit(1);
    }

    //query the pools usage statistics
    int i = 0;
    char *pool_name = pools_name;
    while(*pool_name!='\0'){
	printf("%s\n",pool_name);
	err = rados_ioctx_create(cluster, pool_name, &pool_io);
	if (err < 0) {
		fprintf(stderr, "%s: cannot open rados pool %s: %s\n", argv[0], pool_name, strerror(-err));
		rados_shutdown(cluster);
		exit(1);
	}
	err = rados_ioctx_pool_stat(pool_io, &pool_stats);
	if (err < 0) {
		fprintf(stderr, "%s: cannot open rados pool stat: %s\n", argv[0], strerror(-err));
		rados_shutdown(cluster);
		exit(1);
	}
	printf("space used: %lld KB\n",pool_stats.num_kb);
	printf("total %lld objects in the pool %s\n",pool_stats.num_objects,pool_name);
	printf("total %lld objects(include replicas) in the pool %s\n",pool_stats.num_object_copies,pool_name);
	printf("%lld objects is missing on primary in the pool %s\n",pool_stats.num_objects_missing_on_primary,pool_name);
	printf("%lld objects is unfound on OSDs in the pool %s\n",pool_stats.num_objects_unfound,pool_name);
	printf("%lld objects is degraded in the pool %s\n",pool_stats.num_objects_degraded,pool_name);

	rados_ioctx_destroy(pool_io);

	while(*pool_name != '\0')
		pool_name++;
        pool_name++;
    }
    
    return 0;
}
