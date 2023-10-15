# develop
```shell
# step 1
./bootstrap.sh

# step 2
export freeswitch_base_path=$(pwd)
./configure --prefix="${freeswitch_base_path}/All/Debug"

# step 3
make -j all && make -j install 

```

# clean
```shell
make -j clean
```

# feat module
* event_handlers/mod_event_redis
```
you cat save channel info to redis
```

```shell
sofia status profile external

reload mod_sofia
```

# self_rtp_ip
```c
 //switch_core_session.c -> switch_core_session_outgoing_channel =>
 // mod_sofia.c -> sofia_outgoing_channel -> sofia_glue_attach_private(nsession, profile, tech_pvt, dest);
	if (!zstr(switch_event_get_header(var_event, "self_rtp_ip"))) {
		while (rtp_ip_index <= 50) {
			profile->rtpip[rtp_ip_index] = switch_core_strdup(profile->pool, switch_event_get_header(var_event, "self_rtp_ip"));
			rtp_ip_index++;
			if (zstr(profile->rtpip[rtp_ip_index])) { break; }
		}
		profile->rtpip_next = 0;
		profile->rtpip_index = 0;
	}
```

```shell
originate {self_rtp_ip=127.0.0.4}sofia/external/opensips/1002 &park()
originate {self_rtp_ip=127.0.0.4}sofia/gateway/opensips/1002 &park()
```