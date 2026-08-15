// include/beman/execution/detail/inlinable_receiver.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_INLINABLE_RECEIVER
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_INLINABLE_RECEIVER

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.receiver;
#else
#include <beman/execution/detail/receiver.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution {
    template <typename Receiver, typename ChildOp>
    concept inlinable_receiver = ::beman::execution::receiver<Receiver>
        && requires(ChildOp* child) {
            { ::std::remove_cvref_t<Receiver>::make_receiver_for(*child) } noexcept -> ::std::same_as<::std::remove_cvref_t<Receiver>> ;
    };
}

// ----------------------------------------------------------------------------

#endif
