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

# specify rtp ip by variable
```c
 //switch_core_session.c -> switch_core_session_outgoing_channel =>
 // mod_sofia.c -> sofia_outgoing_channel -> sofia_glue_attach_private(nsession, profile, tech_pvt, dest);
	sofia_glue_attach_private(nsession, profile, tech_pvt, dest);
 // specify rtp ip by variable
	if (!zstr(switch_event_get_header(var_event, "rtp_ip_v4"))) {
		tech_pvt->mparams.rtpip4 = switch_core_strdup(profile->pool, switch_event_get_header(var_event, "rtp_ip_v4"));
		tech_pvt->mparams.rtpip = tech_pvt->mparams.rtpip4;
	}
    
```

```shell
originate {rtp_ip_v4=127.0.0.4}sofia/external/opensips/1002 &park()
originate {rtp_ip_v4=127.0.0.4}sofia/gateway/opensips/1002 &park()
```