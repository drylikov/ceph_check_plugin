ceph-check-plugin
=================

ceph check plugin (for nagios, shiken etc)

in nagios-plugin, the check-plugin should be put under the {nagios path}/libexec/ , it's same as the NRPE.

in librados-plugin, use the orginal librados c api:

1. ensure that Ceph is installed in /usr/lib or in /usr/local/lib, and the rados library is in /usr/local/include/rados/ 
   or somewhere else.

2. compile the program with -L {Ceph library path}, -lrados, -I {rados library path} 
3. for more details visit: ceph.com/docs/master/rados/api/librados/#rados_t
