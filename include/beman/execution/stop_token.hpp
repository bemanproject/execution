// include/beman/execution/stop_token.hpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_STOP_TOKEN
#define INCLUDED_BEMAN_EXECUTION_STOP_TOKEN

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.check_type_alias_exist;
import beman.execution.detail.inplace_stop_source;
import beman.execution.detail.never_stop_token;
import beman.execution.detail.nostopstate;
import beman.execution.detail.stop_callback_for_t;
import beman.execution.detail.stop_source;
import beman.execution.detail.stoppable_source;
import beman.execution.detail.stoppable_token;
import beman.execution.detail.unstoppable_token;
#else
#include <beman/execution/detail/check_type_alias_exist.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/never_stop_token.hpp>
#include <beman/execution/detail/nostopstate.hpp>
#include <beman/execution/detail/stop_callback_for_t.hpp>
#include <beman/execution/detail/stop_source.hpp>
#include <beman/execution/detail/stoppable_source.hpp>
#include <beman/execution/detail/stoppable_token.hpp>
#include <beman/execution/detail/unstoppable_token.hpp>
#endif

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_STOP_TOKEN
