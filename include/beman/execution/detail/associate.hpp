// include/beman/execution/detail/associate.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_ASSOCIATE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_ASSOCIATE

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/scope_token.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/nothrow_callable.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <type_traits>
#include <optional>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
BEMAN_EXECUTION_EXPORT template <::beman::execution::scope_token Token,
                                 ::beman::execution::sender      Sender> //-dk:TODO detail export
struct associate_data {
    using wrap_sender = ::std::remove_cvref_t<decltype(::std::declval<Token&>().wrap(::std::declval<Sender>()))>;

    explicit associate_data(Token t, Sender&& s) : token(t), sender(this->token.wrap(::std::forward<Sender>(s))) {
        if (!token.try_associate()) {
            this->sender.reset();
        }
    }
    associate_data(const associate_data& other) noexcept(::std::is_nothrow_copy_constructible_v<wrap_sender> &&
                                                         noexcept(token.try_associate()))
        : token(other.token), sender() {
        if (other.sender && this->token.try_associate()) {
            try {
                this->sender.emplace(*other.sender);
            } catch (...) {
                this->token.disassociate();
            }
        }
    }
    associate_data(associate_data&& other) noexcept(::std::is_nothrow_move_constructible_v<wrap_sender>)
        : token(other.token), sender(::std::move(other.sender)) {
        other.sender.reset();
    }
    auto operator=(const associate_data&) -> associate_data& = delete;
    auto operator=(associate_data&&) -> associate_data&      = delete;
    ~associate_data() {
        if (this->sender) {
            this->sender.reset();
            this->token.disassociate();
        }
    }

    auto release() -> ::std::optional<::std::pair<Token, wrap_sender>> {
        return this->sender ? (std::unique_ptr<std::optional<wrap_sender>, decltype([](auto* opt) { opt->reset(); })>(
                                   &this->sender),
                               ::std::optional{::std::pair{::std::move(this->token), ::std::move(*this->sender)}})
                            : ::std::optional<::std::pair<Token, wrap_sender>>{};
    }

    Token                        token;
    ::std::optional<wrap_sender> sender;
};
BEMAN_EXECUTION_EXPORT template <::beman::execution::scope_token Token, ::beman::execution::sender Sender>
associate_data(Token, Sender&&) -> associate_data<Token, Sender>;

struct associate_t {
    template <::beman::execution::sender Sender, ::beman::execution::scope_token Token>
    auto operator()(Sender&& sender, Token&& token) const {
        auto domain(::beman::execution::detail::get_domain_early(sender));
        return ::beman::execution::transform_sender(
            domain,
            ::beman::execution::detail::make_sender(
                *this,
                ::beman::execution::detail::associate_data(::std::forward<Token>(token),
                                                           ::std::forward<Sender>(sender))));
    }
};

template <>
struct impls_for<associate_t> : ::beman::execution::detail::default_impls {
    template <typename, typename>
    struct get_noexcept : ::std::false_type {};
    template <typename Tag, typename Data, typename Receiver>
    struct get_noexcept<::beman::execution::detail::basic_sender<Tag, Data>, Receiver>
        : ::std::bool_constant<
              ::std::is_nothrow_move_constructible_v<typename ::std::remove_cvref_t<Data>::wrap_sender>&& ::beman::
                  execution::detail::nothrow_callable<::beman::execution::connect_t,
                                                      typename ::std::remove_cvref_t<Data>::wrap_sender,
                                                      Receiver>> {};

    static constexpr auto get_state =
        []<typename Sender, typename Receiver>(Sender&& sender, Receiver& receiver) noexcept(
            ::std::is_nothrow_constructible_v<::std::remove_cvref_t<Sender>, Sender>&&
                get_noexcept<::std::remove_cvref_t<Sender>, Receiver>::value) {
            auto [_, data] = ::std::forward<Sender>(sender);
            auto dataParts{data.release()};

            using scope_token = decltype(dataParts->first);
            using wrap_sender = decltype(dataParts->second);
            using op_t        = decltype(::beman::execution::connect(::std::move(dataParts->second),
                                                              ::std::forward<Receiver>(receiver)));

            struct op_state {
                using sop_t        = op_t;
                using sscope_token = scope_token;
                struct assoc_t {
                    sscope_token tok;
                    sop_t        op;
                };

                bool associated{false};
                union {
                    Receiver* rcvr;
                    assoc_t   assoc;
                };
                explicit op_state(Receiver& r) noexcept : rcvr(::std::addressof(r)) {}
                explicit op_state(sscope_token tk, wrap_sender&& sndr, Receiver& r) try
                    : associated(true), assoc(tk, ::beman::execution::connect(::std::move(sndr), ::std::move(r))) {
                } catch (...) {
                    tk.disassociate();
                    throw;
                }
                op_state(op_state&&) = delete;
                ~op_state() {
                    if (this->associated) {
                        this->assoc.op.~sop_t();
                        this->assoc.tok.disassociate();
                        this->assoc.tok.~sscope_token();
                    }
                }
                auto run() noexcept -> void {
                    if (this->associated) {
                        ::beman::execution::start(this->assoc.op);
                    } else {
                        ::beman::execution::set_stopped(::std::move(*this->rcvr));
                    }
                }
            };
            return dataParts ? op_state(::std::move(dataParts->first), ::std::move(dataParts->second), receiver)
                             : op_state(receiver);
        };
    static constexpr auto start = [](auto& state, auto&&) noexcept -> void { state.run(); };
};

template <typename Data, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::associate_t, Data>,
    Env> {
    using type = ::beman::execution::completion_signatures<::beman::execution::set_value_t()>;
};
} // namespace beman::execution::detail

namespace beman::execution {
BEMAN_EXECUTION_EXPORT using associate_t = ::beman::execution::detail::associate_t;
BEMAN_EXECUTION_EXPORT inline constexpr associate_t associate{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_ASSOCIATE
