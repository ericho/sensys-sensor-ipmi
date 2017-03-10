#
# Copyright (c) 2016     Intel Corporation. All rights reserved.
#
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

EXTRA_DIST =
CLEANFILES =
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
AM_MAKEFLAGS = --no-print-directory

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-I${top_srcdir}/src

AM_CFLAGS = ${my_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections

AM_LDFLAGS = \
	-Wl,--gc-sections \
	-Wl,--as-needed \
	-module

SED_PROCESS = \
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(SED) \
	-e 's,@VERSION\@,$(VERSION),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@exec_prefix\@,$(exec_prefix),g' \
	-e 's,@libdir\@,$(libdir),g' \
	-e 's,@includedir\@,$(includedir),g' \
	< $< > $@ || rm $@

%.pc: %.pc.in Makefile
	$(SED_PROCESS)

pkginclude_HEADERS = \
		src/sensor_ipmi_ts.h \
		src/ipmiSensorFactory.hpp \
		src/ipmiSensorInterface.h \
		src/ipmiLibInterface.h \
		src/ipmiResponse.h \
		src/ipmiHAL.h \
		src/ipmiutilAgent.h \
		src/ipmiutilDFx.h \
		src/ipmiResponseFactory.hpp \
		src/ipmiutilAgent_exceptions.h \
		src/ipmi_ts_persist_sel_record_id.h \
		src/ipmi_ts_sel_callback_defs.h \
		src/ipmi_ts_sel_collector.h \
		src/ipmi_ts_sel_record.h

pkglib_LTLIBRARIES = mca_sensor_ipmi_ts.la


sources = \
        src/sensor_ipmi_ts_component.c \
        src/sensor_ipmi_ts.h \
        src/sensor_ipmi_ts.cpp \
        src/ipmiSensorFactory.hpp \
        src/ipmiSensorFactory.cpp \
        src/ipmiSensorInterface.h \
        src/ipmiLibInterface.h \
        src/ipmiResponse.h \
        src/ipmiHAL.h \
        src/ipmiHAL.cpp \
        src/ipmiutilAgent.h \
        src/ipmiutilAgent.cpp \
        src/ipmiutilDFx.h \
        src/ipmiutilDFx.cpp \
        src/ipmiResponseFactory.hpp \
        src/ipmiResponseFactory.cpp \
        src/ipmiutilAgent_exceptions.h \
        src/ipmiutilAgent_exceptions.cpp \
        src/ipmi_ts_persist_sel_record_id.cpp \
        src/ipmi_ts_persist_sel_record_id.h \
        src/ipmi_ts_sel_callback_defs.h \
        src/ipmi_ts_sel_collector.cpp \
        src/ipmi_ts_sel_collector.h \
        src/ipmi_ts_sel_record.cpp \
        src/ipmi_ts_sel_record.h
 


mca_sensor_ipmi_ts_la_CPPFLAGS = $(sensor_ipmi_ts_CPPFLAGS) $(ipmiutil_CPPFLAGS) -I/opt/sensys/include/openmpi \
				-I/opt/sensys/include/openmpi/opal/mca/hwloc/hwloc1110/hwloc/include/ \
				-I/opt/sensys/include/openmpi/opal/mca/event/libevent2022/libevent/include/ \
				-I/opt/sensys/include/openmpi/opal/mca/event/libevent2022/libevent/ \
				-Isrc/
mca_sensor_ipmi_ts_la_CXXFLAGS = $(sensor_ipmi_ts_CXXFLAGS) $(ipmiutil_CXXFLAGS) -std=c++11
mca_sensor_ipmi_ts_la_SOURCES = $(sources)
mca_sensor_ipmi_ts_la_LDFLAGS = -module -avoid-version $(ipmiutil_LDFLAGS)
mca_sensor_ipmi_ts_la_LIBADD = /opt/sensys/lib/libsensysplugins.la \
                               /opt/sensys/lib/libsensysplugins_helper.la \
                               $(sensor_ipmi_ts_LIBS) $(ipmiutil_LIBS) -lm

include src/ipmi_test_sensor/Makefile.am
include src/ipmi_sensor/Makefile.am
