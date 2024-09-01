#include "librados.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
* Author: atrmat@gmail.com
* Ceph check-plugin
* Copyright (c) 2013
* All rights reserved.
*
* function: query the cluster usage statistics
*/

int main(int args, char* argv[]){
    int err;
    rados_t cluster;

    err = rados_create(&cluster,NULL);
    if (err < 0) {
        fprintf(stderr, "%s: cannot create a cluster handle: %s\n", argv[0], strerror(-err));
        exit(1);
    }
    err = rados_conf_read_file(cluster, "/etc/ceph/ceph.conf");
    if (err < 0) {
        fprintf(stderr, "%s: cannot read config file: %s\n", argv[0], strerror(-err));
        exit(1);
    }
    err = rados_connect(cluster);
    if (err < 0) {
        fprintf(stderr, "%s: cannot connect to cluster: %s\n", argv[0], strerror(-err));
        exit(1);
    }

    //query the cluster statistics
    struct rados_cluster_stat_t *result;
    err = rados_cluster_stat(cluster, result);

    if (err < 0) {
        fprintf(stderr, "%s: cannot return result: %s\n", argv[0], strerror(-err));
        rados_shutdown(cluster);
        exit(1);
    }
    printf("cluster stat: \n");
    printf("\t total space:%llu kb\n",result->kb);
    printf("\t used space:%llu kb\n",result->kb_used);
    printf("\t avail space:%llu kb\n",result->kb_avail);
    printf("\t object num:%llu \n",result->num_objects);
	
    return 0;
}
