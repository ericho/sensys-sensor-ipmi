#
# Copyright (c) 2015  Intel Corporation. All rights reserved.
# Copyright (c) 2016      Intel Corporation. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

TESTS = ipmi_ts_tests

check_PROGRAMS = ipmi_ts_tests

ipmi_ts_tests_SOURCES = \
    src/tests/ipmi_ts_tests.h \
    src/tests/ipmi_ts_tests.cpp \
    src/tests/ipmiHAL_tests.h \
    src/tests/ipmiHAL_tests.cpp \
    src/tests/ipmiutilAgent_tests.h \
    src/tests/ipmiutilAgent_tests.cpp \
    src/tests/ipmiutilAgent_mocks.h \
    src/tests/ipmiutilAgent_mocks.cpp \
    src/tests/ipmiResponseFactory_tests.hpp \
    src/tests/ipmiResponseFactory_tests.cpp \
    src/tests/ipmi_sensor_tests.hpp \
    src/tests/ipmi_sensor_tests.cpp \
    src/tests/timeoutHelper.hpp \
    src/tests/timeoutHelper.cpp \
    src/tests/persist_sel_record_id_tests.cpp \
    src/tests/persist_sel_record_id_tests.h \
    src/tests/sensor_ipmi_sel_mocked_functions.cpp \
    src/tests/sensor_ipmi_sel_mocked_functions.h \
    src/tests/ipmi_sel_collector_tests.h \
    src/tests/ipmi_sel_collector_tests.cpp

MOCKING = \
    -Wl,--wrap=opal_progress_thread_init \
    -Wl,--wrap=orcm_sensor_base_runtime_metrics_do_collect \
    -Wl,--wrap=opal_dss_pack \
    -Wl,--wrap=opal_dss_unpack \
    -Wl,--wrap=orcm_analytics_base_send_data \
    -Wl,--wrap=_ZN19dataContainerHelper12serializeMapERSt3mapISs13dataContainerSt4lessISsESaISt4pairIKSsS1_EEEPv \
    -Wl,--wrap=_ZN19dataContainerHelper14deserializeMapERSt3mapISs13dataContainerSt4lessISsESaISt4pairIKSsS1_EEEPv \
    -Wl,--wrap=_ZN14IpmiTestSensor8finalizeEv \
    -Wl,--wrap=_ZN14IpmiTestSensor4initEv \
    -Wl,--wrap=_ZN10ipmiSensor4initEv \
    -Wl,--wrap=_ZN10ipmiSensor8finalizeEv \
    -Wl,--wrap=set_lan_options \
    -Wl,--wrap=ipmi_cmd_mc \
    -Wl,--wrap=ipmi_cmd \
    -Wl,--wrap=ipmi_cmdraw \
    -Wl,--wrap=get_sdr_cache \
    -Wl,--wrap=find_sdr_next \
    -Wl,--wrap=rename \
    -Wl,--wrap=opal_output_verbose \
    -Wl,--wrap=localtime \
    -Wl,--wrap=strdup \
    -Wl,--wrap=opal_libevent2022_event_new

ipmi_ts_tests_LDADD = \
                /usr/local/lib/libgtest_main.a \
                .libs/libmca_sensor_ipmi_ts.a \
                /opt/sensys/lib/liborcm.la \
                /opt/sensys/lib/libsensysplugins.la \
                /opt/sensys/lib/libsensysplugins_helper.la
ipmi_ts_tests_LDFLAGS = -lorcm -lorcmopen-rte -lorcmopen-pal -lsensysplugins -lpthread -lcrypto -lipmiutil $(MOCKING)
ipmi_ts_tests_CPPFLAGS=-I/usr/local/include $(mca_sensor_ipmi_ts_la_CPPFLAGS)
