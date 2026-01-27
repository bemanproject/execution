// include/beman/execution/detail/class_type.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_CLASS_TYPE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_CLASS_TYPE

#include <beman/execution/detail/decays_to.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Auxiliary concept used to detect class types. [execution.syn#concept:class-type]
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename Tp>
concept class_type = ::beman::execution::detail::decays_to<Tp, Tp> && ::std::is_class_v<Tp>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_CLASS_TYPE
