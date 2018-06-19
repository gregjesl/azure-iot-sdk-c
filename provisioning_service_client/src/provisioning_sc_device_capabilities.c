// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/gballoc.h"

#include "prov_service_client/provisioning_sc_device_capabilities.h"
#include "prov_service_client/provisioning_sc_json_const.h"
#include "prov_service_client/provisioning_sc_shared_helpers.h"
#include "prov_service_client/provisioning_sc_models_serializer.h"
#include "parson.h"

typedef struct DEVICE_CAPABILITIES_TAG
{
    bool iotEdge
} DEVICE_CAPABILITIES;

DEVICE_CAPABILITIES_HANDLE deviceCapabilities_create(void)
{
    DEVICE_CAPABILITES_HANDLE new_capabilities = NULL;

    if ((new_capabilites = malloc(sizeof(DEVICE_CAPABILITIES))) == NULL)
    {
        LogError("Failed to allocate device capabilities");
    }
    else
    {
        new_capabilites->iotEdge = false;
    }
    
    return new_capabilites
}

static void deviceCapabilities_destroy(DEVICE_CAPABILITIES_HANDLE capabilities)
{
    if (capabilities != NULL)
    {
        free(capabilities);
    }
}

DEVICE_CAPABILITIES_HANDLE deviceCapabilities_fromJson(JSON_Object* root_object)
{
    DEVICE_CAPABILITIES_HANDLE new_capabilities = NULL;

    if (root_object == NULL)
    {
        LogError("No device capabilites in JSON");
    }
    else if ((new_capabilities = malloc(sizeof(DEVICE_CAPABILITIES))) == NULL)
    {
        LogError("Allocation of Device Capabilities failed");
    }
    else
    {
        memset(new_capabilities, 0, sizeof(DEVICE_CAPABILITIES));
        new_capabilities->iotEdge = (bool)json_object_get_boolean(root_object, DEVICE_CAPABILITIES_JSON_KEY_IOT_EDGE);
    }

    return new_capabilities;
}

JSON_Value* deviceCapabilities_toJson(const DEVICE_CAPABILITIES_HANDLE capabilities)
{
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    const char* ps_str = NULL;

    //Setup
    if (capabilities == NULL)
    {
        LogError("capabilities is NULL");
    }
    else if ((root_value = json_value_init_object()) == NULL)
    {
        LogError("json_value_init_object failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("json_value_get_object failed");
        json_value_free(root_value);
        root_value = NULL;
    }

    //Set data
    //else if (json_object_set_string(root_object, INDIVIDUAL_ENROLLMENT_JSON_KEY_REG_ID, capabilities->registration_id) != JSONSuccess)
    else if (json_object_set_boolean(root_object, DEVICE_CAPABILITIES_JSON_KEY_IOT_EDGE, capabilities->iotEdge) != JSONSuccess)
    {
        LogError("Failed to set '%s' in JSON string", DEVICE_CAPABILITIES_JSON_KEY_IOT_EDGE);
        json_value_free(root_value);
        root_value = NULL;
    }

    return root_value;
}

DEVICE_CAPABILITIES_HANDLE deviceCapabilities_deserializeFromJson(const char* json_string)
{
    DEVICE_CAPABILITIES_HANDLE new_capabilities = NULL;
    JSON_Value* root_value = NULL;
    JSON_Object* root_object = NULL;

    if (json_string == NULL)
    {
        LogError("Cannot deserialize NULL");
    }
    else if ((root_value = json_parse_string(json_string)) == NULL)
    {
        LogError("Parsing JSON string failed");
    }
    else if ((root_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("Creating JSON object failed");
    }
    else
    {
        if ((new_capabilities = deviceCapabilities_fromJson(root_object)) == NULL)
        {
            LogError("Creating new Device Capabilities failed");
        }
        json_value_free(root_value); //implicitly frees root_object
        root_value = NULL;
    }

    return new_capabilities;
}

char* deviceCapabilities_serializeToJson(const DEVICE_CAPABILITIES_HANDLE capabilities)
{
    char* result = NULL;
    char* serialized_string = NULL;
    JSON_Value* root_value = NULL;

    if (capabilities == NULL)
    {
        LogError("Cannot serialize NULL");
    }
    else if ((root_value = deviceCapabilities_toJson(capabilities)) == NULL)
    {
        LogError("Creating json object failed");
    }
    else if ((serialized_string = json_serialize_to_string(root_value)) == NULL)
    {
        LogError("Failed to serialize to JSON");
    }
    else if (mallocAndStrcpy_s(&result, serialized_string) != 0)
    {
        LogError("Failed to copy serialized string");
    }

    if (root_value != NULL)
    {
        json_value_free(root_value); 
        root_value = NULL;
    }
    if (serialized_string != NULL)
    {
        json_free_serialized_string(serialized_string);
        serialized_string = NULL;
    }

    return result;
}


/* Acessor Functions */
bool deviceCapabilities_getIotEdge(DEVICE_CAPABILITIES_HANDLE capabilities)
{
    bool result;

    if (capabilities == NULL)
    {
        LogError("device capabilities is NULL");
        result = false;

    }
    else
    {
        result = capabilities->iotEdge;
    }

    return result;
}

void deviceCapabilities_setIotEdge(DEVICE_CAPABILITIES_HANDLE capabilities, bool iotEdge)
{
    if (capabilities != NULL)
    {
        capabilities->iotEdge = iotEdge;
    }
}


