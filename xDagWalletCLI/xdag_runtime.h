#pragma once

#pragma unmanaged

#ifndef XDAG_RUNTIME_HEADER_H
#define XDAG_RUNTIME_HEADER_H

#include "stdafx.h"

#define _TIMESPEC_DEFINED


// This is to disable security warning from Visual C++
//// #define _CRT_SECURE_NO_WARNINGS

//#include "../xDagWallet/src/client/init.h"
#include "../xDagWallet/src/client/common.h"
#include "../xDagWallet/src/client/commands.h"
#include "../xDagWallet/src/client/client.h"
#include "../xDagWallet/src/client/events.h"
#include "../xDagWallet/src/client/utils/utils.h"
#include "../xDagWallet/src/client/address.h"
#include "../xDagWallet/src/client/dnet_crypt.h"
#include "../xDagWallet/src/client/xdag_wrapper.h"

////---- Duplicated from dnet_crypt.c ----
#define KEYFILE	    "dnet_key.dat"
struct dnet_keys {
	struct dnet_key priv;
	struct dnet_key pub;
};
////------------------------------------


////---- Duplicated from commands.c ----
struct account_callback_data {
	char out[128];
	int count;
};

struct xfer_callback_data {
	struct xdag_field fields[XFER_MAX_IN + 1];
	int keys[XFER_MAX_IN + 1];
	xdag_amount_t todo, done, remains;
	int fieldsCount, keysCount, outsig;
	xdag_hash_t transactionBlockHash;
};
////------------------------------------

int xdag_event_callback(void* thisObj, xdag_event *event);


////---- Exporting functions ----
NATIVE_LIB_EXPORT int xdag_init_wrap(int argc, char **argv, const char * pool_address, bool is_test_net);
NATIVE_LIB_EXPORT int xdag_set_password_callback_wrap(int(*callback)(const char *prompt, char *buf, unsigned size));
NATIVE_LIB_EXPORT int xdag_set_event_callback_wrap(int(*callback)(void*, xdag_event *));
NATIVE_LIB_EXPORT int xdag_get_state_wrap(void);
NATIVE_LIB_EXPORT int xdag_get_balance_wrap(void);
NATIVE_LIB_EXPORT int xdag_get_address_wrap(void);


NATIVE_LIB_EXPORT int xdag_transfer_wrap(const char* toAddress, const char* amountString, const char* remarkString);
NATIVE_LIB_EXPORT bool xdag_is_valid_wallet_address(const char* address);
NATIVE_LIB_EXPORT bool xdag_dnet_crpt_found();
NATIVE_LIB_EXPORT bool xdag_is_valid_remark(const char* remark);


////------------------------------------
extern pthread_t g_client_thread;
static int g_client_init_done = 0;
////---- Exporting functions wrapping functions ----
int xdag_init_wrap(int argc, char **argv, const char* pool_address, bool is_test_net)
{
	xdag_init_path(argv[0]);

	////const char *pool_arg = "de1.xdag.org:13654";

	////xdag_set_event_callback(&xdag_event_callback);

	xdag_thread_param_t param;
	strncpy(param.pool_arg, pool_address, 255);
	param.testnet = is_test_net?1:0;;

	int err = pthread_create(&g_client_thread, 0, xdag_client_thread, (void*)&param);
	if (err != 0) {
		printf("create client_thread failed, error : %s\n", strerror(err));
		return -1;
	}
	while (!g_client_init_done) {
		sleep(1);
	}

	return 0;
}

int xdag_set_event_callback_wrap(int(*callback)(void*, xdag_event *))
{
	return xdag_set_event_callback(callback);
}

int xdag_get_state_wrap(void)
{
	xdag_wrapper_state();
	return 0;
}

int xdag_get_balance_wrap(void)
{
	xdag_wrapper_balance();
	return 0;
}

int xdag_get_address_wrap(void)
{
	xdag_wrapper_address();
	return 0;
}

int xdag_event_callback(void* thisObj, xdag_event *event)
{
	if (!event) {
		return -1;
	}

	switch (event->event_id) {
	case event_id_init_done:
	{
		g_client_init_done = 1;
		break;
	}
	case event_id_log:
	{
		
		break;
	}

	case event_id_interact:
	{
		
		break;
	}

	//		case event_id_err:
	//		{
	//			fprintf(stdout, "error : %x, msg : %s\n", event->error_no, event->event_data);
	//			fflush(stdout);
	//			break;
	//		}

	case event_id_err_exit:
	{
		
		break;
	}

	case event_id_account_done:
	{
		
		break;
	}

	case event_id_address_done:
	{
		
		break;
	}

	case event_id_balance_done:
	{
		
		break;
	}

	case event_id_xfer_done:
	{
		
		break;
	}

	case event_id_level_done:
	{
		
		break;
	}

	case event_id_state_done:
	{
		
		break;
	}

	case event_id_exit_done:
	{
		
		break;
	}

	case event_id_passwd:
	{
		break;
	}

	case event_id_set_passwd:
	{
		break;
	}

	case event_id_set_passwd_again:
	{
		break;
	}

	case event_id_random_key:
	{
		break;
	}

	case event_id_state_change:
	{
		
		break;
	}

	default:
	{
		
		break;
	}
	}
	return 0;
}

int xdag_set_password_callback_wrap(int(*callback)(const char *prompt, char *buf, unsigned size))
{
	//// return xdag_set_password_callback(callback);
	return xdag_user_crypt_action((uint32_t *)(void *)callback, 0, 0, 6);
}

int xdag_transfer_wrap(const char* toAddress, const char* amountString, const char* remarkString)
{
	char* address = new char[strlen(toAddress) + 1];
	char* amount = new char[strlen(amountString) + 1];
	char* remark = new char[strlen(remarkString) + 1];

	strcpy_s(address, strlen(toAddress) + 1, toAddress);
	strcpy_s(amount, strlen(amountString) + 1, amountString);
	strcpy_s(remark, strlen(remarkString) + 1, remarkString);

	char *result = NULL;
	int err = processXferCommand(amount, address, remark, &result);

	if (err != error_none) {
		xdag_wrapper_event(event_id_log, (xdag_error_no)err, result);
	}
	else {
		xdag_wrapper_event(event_id_xfer_done, (xdag_error_no)0, result);
	}

	return err;
}

bool xdag_is_valid_wallet_address(const char* address)
{
	struct xfer_callback_data xfer;
	if (xdag_address2hash(address, xfer.fields[XFER_MAX_IN].hash) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool xdag_dnet_crpt_found()
{
	FILE *f = NULL;
	struct dnet_keys *keys = (struct dnet_keys*)malloc(sizeof(struct dnet_keys));

	bool is_found = false;
	f = xdag_open_file(KEYFILE, "rb");

	if (f)
	{
		if (fread(keys, sizeof(struct dnet_keys), 1, f) == 1)
		{
			is_found = true;
		}
		xdag_close_file(f);
	}

	free(keys);
	return is_found;
}

bool xdag_is_valid_remark(const char* remark)
{
	size_t s = validate_remark(remark);
	if (s < 1 || s > 33)
	{
		return false;
	}
	else
	{
		return true;
	}
}

#endif