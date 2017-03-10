/*
 * Copyright (c) 2016      Intel Corporation. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ipmi_ts_tests.h"

// C
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include "ipmiSensorFactory.hpp"
#include "orcm/common/dataContainer.hpp"
#include "orcm/common/dataContainerHelper.hpp"

using namespace std;

extern "C" {
    #include "opal/mca/installdirs/installdirs.h"
    #include "opal_config.h"
    #include "sensor_ipmi_ts.h"
    #include "opal/mca/installdirs/installdirs.h"
    #include "orte/runtime/orte_globals.h"
    #include "orte/util/proc_info.h"
    #include "orcm/util/utils.h"
    #include "orcm/runtime/orcm_globals.h"
    #include "orcm/mca/analytics/analytics.h"
    #include "orcm/mca/sensor/base/base.h"
    #include "orcm/mca/sensor/base/sensor_private.h"
    #include "opal/runtime/opal_progress_threads.h"
    #include "opal/runtime/opal.h"
    #include "orcm/mca/sensor/base/sensor_runtime_metrics.h"
    extern orcm_sensor_base_module_t orcm_sensor_ipmi_ts_module;
    extern orcm_sensor_ipmi_ts_component_t mca_sensor_ipmi_ts_component;
    extern int ipmi_ts_enable_sampling(const char* sensor_specification);
    extern int ipmi_ts_disable_sampling(const char* sensor_specification);
    extern int ipmi_ts_reset_sampling(const char* sensor_specification);
    extern int orcm_sensor_ipmi_ts_open(void);
    extern int orcm_sensor_ipmi_ts_close(void);
    extern int orcm_sensor_ipmi_ts_query(mca_base_module_t **module, int *priority);
    extern int ipmi_ts_component_register(void);
    extern orcm_sensor_ipmi_ts_t orcm_sensor_ipmi_ts;
    extern void ipmi_ts_define_init_params(dataContainer* dc);

    extern opal_event_base_t* __real_opal_progress_thread_init(const char* name);
    opal_event_base_t* __wrap_opal_progress_thread_init(const char* name)
    {
        if(ut_ipmi_ts_tests::use_pt_) {
            return __real_opal_progress_thread_init(name);
        } else {
            return NULL;
        }
    }
};

#define N_MOCKED_PLUGINS 1
bool ut_ipmi_ts_tests::use_pt_ = true;

static orte_proc_type_t tmp_proc_type;

void set_aggregator_mode()
{
    tmp_proc_type = orte_process_info.proc_type;
    orte_process_info.proc_type |= ORTE_PROC_AGGREGATOR;
}

void unset_aggregator_mode()
{
    orte_process_info.proc_type = tmp_proc_type;
}

void ut_ipmi_ts_tests::SetUpTestCase()
{
    opal_init_test();
    mca_sensor_ipmi_ts_component.test = true;

    static const char MCA_DFX_FLAG[] = "ORCM_MCA_sensor_ipmi_ts_dfx";
    setenv(MCA_DFX_FLAG, "1", 1);
}

void ut_ipmi_ts_tests::TearDownTestCase()
{
    mca_sensor_ipmi_ts_component.test = true;

    static const char MCA_DFX_FLAG[] = "ORCM_MCA_sensor_ipmi_ts_dfx";
    setenv(MCA_DFX_FLAG, "1", 1);
}

void ut_ipmi_ts_tests::setFullMock(bool mockStatus, int nPlugins)
{
    n_mocked_plugins = nPlugins;
    n_opal_calls = 0;
    mock_plugin = mockStatus;
    mock_do_collect = mockStatus;
    mock_opal_pack = mockStatus;
    mock_opal_unpack = mockStatus;
    do_collect_expected_value = mockStatus;
    opal_pack_expected_value = mockStatus;
    opal_secpack_expected_value = mockStatus;
    opal_unpack_expected_value = mockStatus;
    mock_serializeMap = mockStatus;
    mock_deserializeMap = mockStatus;
    getPluginNameMock = mockStatus;
    initPluginMock = mockStatus;
    dlopenReturnHandler = mockStatus;

}

bool ut_ipmi_ts_tests::isOutputError(std::string output)
{
    std::string error = ("ERROR");
    std::size_t found = output.find(error);
    return (found!=std::string::npos);
}

TEST_F(ut_ipmi_ts_tests, ipmi_ts_api_tests)
{
    void* object = orcm_sensor_base_runtime_metrics_create("ipmi_ts", false, false);
    mca_sensor_ipmi_ts_component.runtime_metrics = object;

    ipmi_ts_enable_sampling("ipmi_ts");
    EXPECT_TRUE(orcm_sensor_base_runtime_metrics_do_collect(object, NULL));
    ipmi_ts_disable_sampling("ipmi_ts");
    EXPECT_FALSE(orcm_sensor_base_runtime_metrics_do_collect(object, NULL));
    ipmi_ts_enable_sampling("ipmi_ts");
    EXPECT_TRUE(orcm_sensor_base_runtime_metrics_do_collect(object, NULL));
    ipmi_ts_reset_sampling("ipmi_ts");
    EXPECT_FALSE(orcm_sensor_base_runtime_metrics_do_collect(object, NULL));
    ipmi_ts_enable_sampling("not_the_right_datagroup");
    EXPECT_FALSE(orcm_sensor_base_runtime_metrics_do_collect(object, NULL));
    ipmi_ts_enable_sampling("all");
    EXPECT_TRUE(orcm_sensor_base_runtime_metrics_do_collect(object, NULL));

    orcm_sensor_base_runtime_metrics_destroy(object);
    mca_sensor_ipmi_ts_component.runtime_metrics = NULL;
}

TEST_F(ut_ipmi_ts_tests, start_stop_progress_thread_negative)
{
    use_pt_ = false;
    mca_sensor_ipmi_ts_component.use_progress_thread = true;

    orcm_sensor_ipmi_ts_module.start(5);
    EXPECT_FALSE(orcm_sensor_ipmi_ts.ev_active);

    use_pt_ = true;
}

TEST_F(ut_ipmi_ts_tests, double_start)
{
    mca_sensor_ipmi_ts_component.use_progress_thread = true;
    orcm_sensor_ipmi_ts_module.start(5);
    EXPECT_TRUE(orcm_sensor_ipmi_ts.ev_active);
    orcm_sensor_ipmi_ts_module.start(5);
    EXPECT_TRUE(orcm_sensor_ipmi_ts.ev_active);

    orcm_sensor_ipmi_ts_module.stop(5);
    EXPECT_FALSE(orcm_sensor_ipmi_ts.ev_active);
}

TEST_F(ut_ipmi_ts_tests, sample_rate_tests)
{
    int rate = 5;
    mca_sensor_ipmi_ts_component.use_progress_thread = false;
    orcm_sensor_ipmi_ts_module.set_sample_rate(rate);
    EXPECT_NE(rate, mca_sensor_ipmi_ts_component.sample_rate);

    mca_sensor_ipmi_ts_component.use_progress_thread = true;
    orcm_sensor_ipmi_ts_module.set_sample_rate(rate);
    EXPECT_EQ(rate, mca_sensor_ipmi_ts_component.sample_rate);

    int *get_rate = NULL;
    orcm_sensor_ipmi_ts_module.get_sample_rate(get_rate);
    EXPECT_EQ(NULL, get_rate);

    get_rate = (int *)malloc(sizeof(int));
    orcm_sensor_ipmi_ts_module.get_sample_rate(get_rate);
    EXPECT_EQ(rate, *get_rate);

    mca_sensor_ipmi_ts_component.sample_rate = 1;
}


TEST_F(ut_ipmi_ts_tests, ipmi_ts_component)
{
    int priority;
    mca_base_module_t* module = NULL;

    EXPECT_EQ(ORCM_SUCCESS, orcm_sensor_ipmi_ts_open());
    EXPECT_EQ(ORCM_SUCCESS, orcm_sensor_ipmi_ts_close());
    EXPECT_EQ(ORCM_SUCCESS, orcm_sensor_ipmi_ts_query(&module, &priority));
}

TEST_F(ut_ipmi_ts_tests, component_register_defaults)
{
    EXPECT_EQ(ORCM_SUCCESS, ipmi_ts_component_register());
    EXPECT_FALSE(mca_sensor_ipmi_ts_component.use_progress_thread);
    EXPECT_EQ(0, mca_sensor_ipmi_ts_component.sample_rate);
    EXPECT_EQ(orcm_sensor_base.collect_metrics,mca_sensor_ipmi_ts_component.collect_metrics);
}

TEST_F(ut_ipmi_ts_sample, start_stop_progress_thread)
{
    int sample_rate = 8;
    mca_sensor_ipmi_ts_component.use_progress_thread = true;
    mca_sensor_ipmi_ts_component.sample_rate = sample_rate;

    orcm_sensor_ipmi_ts_module.start(5);
    EXPECT_TRUE(orcm_sensor_ipmi_ts.ev_active);
    EXPECT_EQ(sample_rate,mca_sensor_ipmi_ts_component.sample_rate);
    orcm_sensor_ipmi_ts_module.stop(5);
    EXPECT_FALSE(orcm_sensor_ipmi_ts.ev_active);

    mca_sensor_ipmi_ts_component.sample_rate = 1;
}

void ut_ipmi_ts_init::SetUp()
{
    set_aggregator_mode();
    setFullMock(false, 0);
    throwOnInit = false;
    throwOnSample = false;
    emptyContainer = false;
}

void ut_ipmi_ts_init::TearDown()
{
    unset_aggregator_mode();
}

TEST_F(ut_ipmi_ts_init, test_vector)
{
    mca_sensor_ipmi_ts_component.test = true;
    EXPECT_EQ(ORCM_SUCCESS, orcm_sensor_ipmi_ts_module.init());
}


void ut_ipmi_ts_sample::SetUp()
{
    opal_init_test();
    mca_sensor_ipmi_ts_component.test = true;

    set_aggregator_mode();

    static const char MCA_DFX_FLAG[] = "ORCM_MCA_sensor_ipmi_ts_dfx";
    setenv(MCA_DFX_FLAG, "1", 1);

    setFullMock(true, N_MOCKED_PLUGINS);
    throwOnInit = false;
    emptyContainer = false;
    mock_serializeMap = false;
    opal_pack_expected_value = false;

    orcm_sensor_ipmi_ts_module.init();
    object = orcm_sensor_base_runtime_metrics_create("ipmi_ts", false, false);
    mca_sensor_ipmi_ts_component.runtime_metrics = object;

    orcm_sensor_sampler_t *sampler = (orcm_sensor_sampler_t*) OBJ_NEW(orcm_sensor_sampler_t);
    samplerPtr = (void*) sampler;
}

void ut_ipmi_ts_sample::TearDown()
{
    unset_aggregator_mode();
    orcm_sensor_ipmi_ts_module.finalize();
    setFullMock(false, 0);
    throwOnInit = false;
    throwOnSample = false;
    do_collect_expected_value = true;

    orcm_sensor_sampler_t *sampler = (orcm_sensor_sampler_t*) samplerPtr;

    mca_sensor_ipmi_ts_component.diagnostics = 0;

    OBJ_RELEASE(sampler);
}

TEST_F(ut_ipmi_ts_sample, with_progress_thread)
{
    orcm_sensor_sampler_t *sampler = (orcm_sensor_sampler_t*) samplerPtr;
    mca_sensor_ipmi_ts_component.use_progress_thread = true;
    orcm_sensor_ipmi_ts_module.sample(sampler);
    EXPECT_EQ(0, (mca_sensor_ipmi_ts_component.diagnostics & 0x1));
}
#if 0
TEST_F(ut_ipmi_ts_sample, no_progress_thread)
{
    orcm_sensor_sampler_t *sampler = (orcm_sensor_sampler_t*) samplerPtr;
    mca_sensor_ipmi_ts_component.use_progress_thread = false;
    orcm_sensor_base_runtime_metrics_set(object, true, "ipmi_ts");
    orcm_sensor_ipmi_ts_module.sample(sampler);
    EXPECT_EQ(1, (mca_sensor_ipmi_ts_component.diagnostics & 0x1));
}
#endif
TEST_F(ut_ipmi_ts_sample, no_collect_metrics)
{
    orcm_sensor_sampler_t *sampler = (orcm_sensor_sampler_t*) samplerPtr;
    mca_sensor_ipmi_ts_component.use_progress_thread = false;
    do_collect_expected_value = false;
    orcm_sensor_ipmi_ts_module.sample(sampler);
    EXPECT_EQ(0, (mca_sensor_ipmi_ts_component.diagnostics & 0x1));
}

TEST_F(ut_ipmi_ts_sample, sensor_perthread)
{
    mca_sensor_ipmi_ts_component.use_progress_thread = true;

    orcm_sensor_ipmi_ts_module.set_sample_rate(1);
    orcm_sensor_ipmi_ts_module.start(0);
    sleep(5);
    orcm_sensor_ipmi_ts_module.set_sample_rate(2);
    sleep(5);

    orcm_sensor_ipmi_ts_module.stop(0);
    mca_sensor_ipmi_ts_component.use_progress_thread = false;
}

TEST_F(ut_ipmi_ts_sample, udsensors_start_stop)
{
    int sample_rate = 8;
    mca_sensor_ipmi_ts_component.use_progress_thread = false;
    mca_sensor_ipmi_ts_component.sample_rate = sample_rate;

    orcm_sensor_ipmi_ts_module.start(5);
    EXPECT_FALSE(orcm_sensor_ipmi_ts.ev_active);
    EXPECT_EQ(orcm_sensor_base.sample_rate, mca_sensor_ipmi_ts_component.sample_rate);
    orcm_sensor_ipmi_ts_module.stop(5);
    EXPECT_FALSE(orcm_sensor_ipmi_ts.ev_active);

    mca_sensor_ipmi_ts_component.sample_rate = 1;
}


void ut_ipmi_ts_log::SetUp()
{
    set_aggregator_mode();
    setFullMock(false, 0);
    struct timeval current_time;
    const char *name = "ipmi_ts_test";
    const char *nodename = "localhost";

    opal_buffer_t* buffer = (opal_buffer_t*) OBJ_NEW(opal_buffer_t);

    opal_dss.pack(buffer, &nodename, 1, OPAL_STRING);
    gettimeofday(&current_time, NULL);
    opal_dss.pack(buffer, &current_time, 1, OPAL_TIMEVAL);

    bufferPtr = (void*) buffer;
}

void ut_ipmi_ts_log::TearDown()
{
    unset_aggregator_mode();
    opal_buffer_t* buffer = (opal_buffer_t*) bufferPtr;
    ORCM_RELEASE(buffer);
}

void ut_ipmi_ts_inventory::SetUp()
{
    set_aggregator_mode();
    setFullMock(true, N_MOCKED_PLUGINS);
    throwOnSample = false;
    mock_opal_pack = false;
    mock_serializeMap = false;
    orcm_sensor_ipmi_ts_module.init();
    object = orcm_sensor_base_runtime_metrics_create("ipmi_ts", false, false);
    mca_sensor_ipmi_ts_component.runtime_metrics = object;

}

void ut_ipmi_ts_inventory::TearDown()
{
    unset_aggregator_mode();
    orcm_sensor_ipmi_ts_module.finalize();
    setFullMock(false, 0);
}

void ut_ipmi_ts_inventory_log::SetUp()
{
    set_aggregator_mode();
    setFullMock(true, N_MOCKED_PLUGINS);
    mock_opal_unpack = false;
    mock_serializeMap = false;
    mock_deserializeMap = false;
    orcm_sensor_ipmi_ts_module.init();
    object = orcm_sensor_base_runtime_metrics_create("ipmi_ts", false, false);
    mca_sensor_ipmi_ts_component.runtime_metrics = object;
}

void ut_ipmi_ts_inventory_log::TearDown()
{
    unset_aggregator_mode();
    orcm_sensor_ipmi_ts_module.finalize();
    setFullMock(false, 0);
}
