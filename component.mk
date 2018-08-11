#
# Component Makefile
#
 
# Component configuration in preprocessor defines
CFLAGS += -DUSE_LWIP_SOCKET_FOR_AZURE_IOT
 
COMPONENT_ADD_INCLUDEDIRS :=  \
../pal/inc  \
cpp_wrapper \
c-utility/inc  \
c-utility/inc/azure_c_shared_utility \
c-utility/pal/inc \
c-utility/pal/freertos \
c-utility/pal/generic \
iothub_client/inc \
serializer/inc \
umqtt/inc  \
umqtt/inc/azure_umqtt_c                 \
deps/parson
 
COMPONENT_OBJS =  \
c-utility/pal/freertos/lock.o                \
c-utility/pal/dns_async.o     \
c-utility/pal/socket_async.o               \
c-utility/pal/freertos/threadapi.o    \
c-utility/pal/freertos/tickcounter.o                \
c-utility/pal/lwip/sntp_lwip.o            \
c-utility/pal/tlsio_options.o            \
c-utility/adapters/agenttime.o         \
\
../pal/src/platform_openssl_compact.o    \
../pal/src/tlsio_openssl_compact.o     \
\
c-utility/src/xlogging.o          \
c-utility/src/singlylinkedlist.o             \
c-utility/src/buffer.o             \
c-utility/src/consolelogger.o              \
c-utility/src/constbuffer.o  \
c-utility/src/constmap.o      \
c-utility/src/crt_abstractions.o          \
c-utility/src/doublylinkedlist.o          \
c-utility/src/gballoc.o            \
c-utility/src/gb_stdio.o         \
c-utility/src/gb_time.o         \
c-utility/src/hmac.o               \
c-utility/src/hmacsha256.o \
c-utility/src/httpapiex.o       \
c-utility/src/httpapiexsas.o                \
c-utility/src/httpheaders.o \
c-utility/src/map.o \
c-utility/src/optionhandler.o             \
c-utility/src/sastoken.o        \
c-utility/src/sha1.o \
c-utility/src/sha224.o            \
c-utility/src/sha384-512.o    \
c-utility/src/strings.o             \
c-utility/src/string_tokenizer.o         \
c-utility/src/urlencode.o      \
c-utility/src/usha.o \
c-utility/src/vector.o             \
c-utility/src/xio.o    \
c-utility/src/base64.o \
\
\
iothub_client/src/iothub_client_ll.o               \
iothub_client/src/iothub_client_core_ll.o               \
iothub_client/src/iothub_device_client_ll.o               \
iothub_client/src/iothub_client_ll_uploadtoblob.o \
iothub_client/src/iothub_client_authorization.o      \
iothub_client/src/iothub_client_retry_control.o      \
iothub_client/src/iothub_client_diagnostic.o            \
iothub_client/src/iothub_message.o            \
iothub_client/src/iothubtransport.o              \
iothub_client/src/iothubtransportmqtt.o    \
iothub_client/src/iothubtransport_mqtt_common.o             \
iothub_client/src/version.o               \
\
\
umqtt/src/mqtt_client.o     \
umqtt/src/mqtt_codec.o    \
umqtt/src/mqtt_message.o              \
\
\
deps/parson/parson.o         \
\
serializer/src/codefirst.o   \
serializer/src/agenttypesystem.o   \
serializer/src/commanddecoder.o   \
serializer/src/datamarshaller.o   \
serializer/src/datapublisher.o   \
serializer/src/dataserializer.o   \
serializer/src/iotdevice.o   \
serializer/src/jsondecoder.o   \
serializer/src/jsonencoder.o   \
serializer/src/methodreturn.o   \
serializer/src/multitree.o   \
serializer/src/schema.o   \
serializer/src/schemalib.o   \
serializer/src/schemaserializer.o   \
\
cpp_wrapper/aioth_client.o \
\
 
COMPONENT_SRCDIRS :=  \
../pal/src  \
cpp_wrapper \
c-utility/pal  \
c-utility/pal/freertos  \
c-utility/pal/lwip  \
c-utility/src \
c-utility/adapters  \
umqtt/src   \
iothub_client/src  \
serializer/src  \
deps/parson