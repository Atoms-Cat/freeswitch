# develop
```shell
# step 1
./bootstrap.sh

# step 2
export freeswitch_base_path=$(pwd)
./configure --prefix=$freeswitch_base_path/All/Debug

# step 3
make clean && make -j && make -j install 

```

# feat module
* event_handlers/mod_event_redis
```
you cat save channel info to redis
```

