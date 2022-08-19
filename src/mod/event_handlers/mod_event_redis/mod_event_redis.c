/*
 * FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
 * Copyright (C) 2005-2014, Anthony Minessale II <anthm@freeswitch.org>
 *
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
 *
 * The Initial Developer of the Original Code is
 * Anthony Minessale II <anthm@freeswitch.org>
 * Portions created by the Initial Developer are Copyright (C)
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Howell Yang <th15817161961@gmail.com>
 *
 *
 * mod_event_redis.c -- Framework Demo Module
 *
 */

#include "mod_event_redis.h"

// 定义全局变量
static struct {
	char *profile_name;
	uint32_t shutdown;
	switch_memory_pool_t *pool;
	switch_hash_t *profiles;
} globals;


// 定义宏函数
// 模块加载完成调用该函数
SWITCH_MODULE_LOAD_FUNCTION(mod_event_redis_load);
// 模块停止时调用该函数
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_event_redis_shutdown);
// 模块运行时调用该函数
SWITCH_MODULE_RUNTIME_FUNCTION(mod_event_redis_runtime);

// 模块定义入口
// SWITCH_MODULE_DEFINITION(模块name，load函数，shutdown函数，runtime函数)
SWITCH_MODULE_DEFINITION(mod_event_redis, mod_event_redis_load, mod_event_redis_shutdown, mod_event_redis_runtime);

static void get_channel(char *uuid) {
	switch_core_session_t *session;
	switch_channel_t *channel;

	if (zstr(uuid)) return;
	if (!(session = switch_core_session_locate(uuid))) {
		return;
	}
	channel = switch_core_session_get_channel(session);
	switch_channel_set_variable(channel, "event_redis", uuid);
	switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_INFO, "%s\n", uuid);
}


static void save_channel_info_to_redis(switch_event_t *event, char *unique_id, switch_stream_handle_t stream)
{
	char *redis_set_cmd;
	char *json;
	cJSON *data = NULL;
	switch_event_serialize_json_obj(event, &data);

	switch_event_serialize_json(event, &json);
	//组装redis命令字符串
	redis_set_cmd = switch_core_sprintf(globals.pool, "%s set %s %s", globals.profile_name, unique_id, switch_core_sprintf(globals.pool, "'%s'", json));
	if(switch_api_execute("hiredis_raw", redis_set_cmd, NULL, &stream) != SWITCH_STATUS_SUCCESS) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Channel unique-id %s %s ,not save redis\n", unique_id, redis_set_cmd);
	}
	// 配置redis key 失效时间
	redis_set_cmd = switch_core_sprintf(globals.pool, "%s expire %s %s", globals.profile_name, unique_id, "86400");
	switch_api_execute("hiredis_raw", redis_set_cmd, NULL, &stream);

	switch_safe_free(data);

	get_channel(unique_id);
}

static void event_handler(switch_event_t *event)
{
	char *buf;
	char *xmlstr = "N/A";
	char *redis_set_cmd;
	char *unique_id = switch_event_get_header_nil(event, "unique-id");
	char *direction = switch_event_get_header(event, "Caller-Direction");
	char *core_uuid = switch_event_get_header(event, "Core-UUID");
	char *number = "N/A";

	switch_xml_t xml;
	uint8_t dofree = 0;
	switch_stream_handle_t stream = { 0 };
	SWITCH_STANDARD_STREAM(stream);

	// todo get to config.xml
	globals.profile_name = "default";
	switch (event->event_id) {
		case SWITCH_EVENT_LOG:
			return;
		case SWITCH_EVENT_CHANNEL_CREATE:
		{
			switch_event_serialize(event, &buf, SWITCH_TRUE);
			if ((xml = switch_event_xmlize(event, SWITCH_VA_NONE))) {
				xmlstr = switch_xml_toxml(xml, SWITCH_FALSE);
				dofree++;
			}
			if (!strcasecmp(direction, "inbound")) {
				number = switch_event_get_header(event, "Caller-Caller-ID-Number");
			} else if (!strcasecmp(direction, "outbound")) {
				number = switch_event_get_header(event, "Caller-Callee-ID-Number");
			}
			redis_set_cmd = switch_core_sprintf(globals.pool, "%s hmset %s %s %s", globals.profile_name, number, unique_id, core_uuid);
			if(switch_api_execute("hiredis_raw", redis_set_cmd, NULL, &stream) == SWITCH_STATUS_SUCCESS) {
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Channel unique-id %s save redis\n", unique_id);
			} else {
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Channel unique-id %s not save redis\n", unique_id);
			}
			save_channel_info_to_redis(event, unique_id, stream);
		} break ;

		case SWITCH_EVENT_CHANNEL_DESTROY:
		{
			switch_event_serialize(event, &buf, SWITCH_TRUE);
			if ((xml = switch_event_xmlize(event, SWITCH_VA_NONE))) {
				xmlstr = switch_xml_toxml(xml, SWITCH_FALSE);
				dofree++;
			}
			if (!strcasecmp(direction, "inbound")) {
				number = switch_event_get_header(event, "Caller-Caller-ID-Number");
			} else if (!strcasecmp(direction, "outbound")) {
				number = switch_event_get_header(event, "Caller-Callee-ID-Number");
			}
			redis_set_cmd = switch_core_sprintf(globals.pool, "%s hdel %s %s", globals.profile_name, number, unique_id);
			if(switch_api_execute("hiredis_raw", redis_set_cmd, NULL, &stream) == SWITCH_STATUS_SUCCESS) {
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "Channel unique-id %s del redis\n", unique_id);
			} else {
				switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Channel unique-id %s not del redis\n", unique_id);
			}
			save_channel_info_to_redis(event, unique_id, stream);
		} break ;

		case SWITCH_EVENT_CHANNEL_UUID:
		case SWITCH_EVENT_CHANNEL_ANSWER:
		case SWITCH_EVENT_CHANNEL_PROGRESS_MEDIA:
		case SWITCH_EVENT_CODEC:
		case SWITCH_EVENT_CHANNEL_HOLD:
		case SWITCH_EVENT_CHANNEL_UNHOLD:
		case SWITCH_EVENT_CHANNEL_EXECUTE:
		case SWITCH_EVENT_CHANNEL_ORIGINATE:
		case SWITCH_EVENT_CALL_UPDATE:
		case SWITCH_EVENT_CHANNEL_CALLSTATE:
		case SWITCH_EVENT_CHANNEL_STATE:
		case SWITCH_EVENT_CHANNEL_BRIDGE:
		case SWITCH_EVENT_CHANNEL_UNBRIDGE:
		case SWITCH_EVENT_CALL_SECURE:
		{
			switch_event_serialize(event, &buf, SWITCH_TRUE);
			if ((xml = switch_event_xmlize(event, SWITCH_VA_NONE))) {
				xmlstr = switch_xml_toxml(xml, SWITCH_FALSE);
				dofree++;
			}
			save_channel_info_to_redis(event, unique_id, stream);
		} break ;

		default:
			switch_event_serialize(event, &buf, SWITCH_TRUE);
			if ((xml = switch_event_xmlize(event, SWITCH_VA_NONE))) {
				xmlstr = switch_xml_toxml(xml, SWITCH_FALSE);
				dofree++;
			}
			break;
	}

	switch_safe_free(buf);

	if (dofree) {
		if (xml) {
			switch_xml_free(xml);
		}
		if (xmlstr) {
			free(xmlstr);
		}
	}
}

// 实现模块加载时被调用函数的逻辑
SWITCH_MODULE_LOAD_FUNCTION(mod_event_redis_load)
{
	/* connect my internal structure to the blank pointer passed to me */
	*module_interface = switch_loadable_module_create_module_interface(pool, modname);

	// 绑定事件，并且回调指定的函数 event_handler
	if (switch_event_bind(modname, SWITCH_EVENT_ALL, SWITCH_EVENT_SUBCLASS_ANY, event_handler, NULL) != SWITCH_STATUS_SUCCESS) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Couldn't bind!\n");
		return SWITCH_STATUS_GENERR;
	}

	globals.pool = pool;

	/* indicate that the module should continue to be loaded */
	return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_event_redis_shutdown)
{
	globals.shutdown = 1;
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, "\nmod_event_redis_shutdown:\n--------------------------------\n");
	return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_RUNTIME_FUNCTION(mod_event_redis_runtime)
{
	switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CONSOLE, "\nmod_event_redis_runtime:\n--------------------------------\n");
	return SWITCH_STATUS_TERM;
}

/* For Emacs:
 * Local Variables:
 * mode:c
 * indent-tabs-mode:t
 * tab-width:4
 * c-basic-offset:4
 * End:
 * For VIM:
 * vim:set softtabstop=4 shiftwidth=4 tabstop=4 noet:
 */
