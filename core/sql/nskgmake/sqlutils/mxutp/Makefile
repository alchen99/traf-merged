#######################################################################
# @@@ START COPYRIGHT @@@
#
# (C) Copyright 2008-2014 Hewlett-Packard Development Company, L.P.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
# @@@ END COPYRIGHT @@@
#######################################################################

CPPSRC := uofsTaskServerProcess.cpp

ifneq ($(TARGTYPE),linux)
CPPSRC += vprocutp.cpp
endif

YINC := tdm_sqlmxmsg_intl

DEP_LIBS := \
	tdm_ds \
	tdm_uofs \
	tdm_refresh

ifeq ($(TARGTYPE),oss)
	DEP_LIBS += tdm_ddol tdm_sqlcli

	ifeq ($(FLAVOR),release)
		DEP_LIBS += \
			common \
			tdm_sqlexport \
			tdm_arkfsddl \
			tdm_sqlmxevents

		LDFLAGS := -lzusrdll -lclip
	endif

	ifeq ($(FLAVOR),debug)
		LDFLAGS := -lzusrdll
	endif

	ifeq ($(TARGTYPE),oss)
		TARGET := mxutp
	endif
endif

SRCPATH := sqlutils/uofs

$(TARGOBJDIR)/uofsTaskServerProcess.$(OBJSUFFIX): tdm_sqlmxmsg_intl/$(TARGTYPE)/$(ARCHBITS)/$(FLAVOR)/sqlmxmsg_msg.h
