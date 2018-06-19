// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef PROVISIONING_SC_DEVICE_CAPABILITIES_H
#define PROVISIONING_SC_DEVICE_CAPABILITIES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_c_shared_utility/umock_c_prod.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "parson.h"

typedef struct DEVICE_CAPABILITIES_TAG* DEVICE_CAPABILITIES_HANDLE;

/* Accessor Functions */
MOCKABLE_FUNCTION(, bool, deviceCapabilities_getIotEdge, DEVICE_CAPABILITIES_HANDLE, capabilities);
MOCKABLE_FUNCTION(, void, deviceCapabilities_setIotEdge, DEVICE_CAPABILITIES_HANDLE, capabilities, bool, iotEdge);


/*---INTERNAL USAGE ONLY---*/
// MOCKABLE_FUNCTION(, JSON_Value*, deviceCapab)
// MOCKABLE_FUNCTION(, DEVICE_CAPABILITIES_HANDLE, deviceCapabilities_fromJson, JSON_Object*, root_object);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PROVISIONING_SC_DEVICE_CAPABILITIES_H */