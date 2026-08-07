#ifndef MC_LAB_SEMANTIC_FACET_TRANSITION_LABELLING_HPP
#define MC_LAB_SEMANTIC_FACET_TRANSITION_LABELLING_HPP

#include <mc_lab/semantic/detail/transition_label_cpo.hpp>

#include <type_traits>
#include <utility>

/**
 * @file
 * Defines structural labelling of transition witnesses.
 *
 * The operation deliberately uses the neutral name `transition_label`.
 * Composed systems may call the associated value an action or a symbol through
 * domain aliases, without creating competing customization mechanisms.
 *
 * In SemTL v0.1, a model supplies a member operation:
 *
 * @code
 * struct system {
 *   auto transition_label(const transition&) const;
 * };
 * @endcode
 *
 * Generic code invokes the public CPO below. Domain aliases may later expose
 * its result as `action_t` or `symbol_t`, but they do not introduce separate
 * operations named `action` or `symbol`.
 */
namespace mc_lab::semantic {

/** Returns the neutral semantic label carried by a transition witness. */
inline constexpr detail::transition_label_fn transition_label{};

namespace detail {

template <class System, class Transition>
concept TransitionLabelCallable =
    requires(const std::remove_cvref_t<System>& system, Transition&& transition) {
        transition_label(system, std::forward<Transition>(transition));
    };

template <class System, class Transition>
requires TransitionLabelCallable<System, Transition>
using transition_label_for_t = std::remove_cvref_t<decltype(transition_label(
    std::declval<const std::remove_cvref_t<System>&>(), std::declval<Transition>()))>;

// A semantic label must denote a value type. In particular, an accidental
// `void` customization is an invalid labelling operation.
template <class System, class Transition>
concept TransitionLabelValue = TransitionLabelCallable<System, Transition>
    && std::is_object_v<transition_label_for_t<System, Transition>>;

}  // namespace detail

/** Stable label value type for the supplied transition iteration form. */
template <class System, class Transition>
requires detail::TransitionLabelCallable<System, Transition>
using transition_label_for_t = detail::transition_label_for_t<System, Transition>;

/**
 * A model assigning a structural label to the supplied transition form.
 *
 * Compile-time contract:
 * - the public `transition_label` CPO is callable with `const System&` and
 *   the supplied transition form;
 * - its normalized result is an object type.
 *
 * Semantic law:
 * - the returned value represents exactly the formal label carried by the
 *   transition witness.
 *
 * `Transition` should normally be the reference or proxy obtained from an
 * outgoing-transition range. The concept neither owns nor extends the lifetime
 * of that witness.
 */
template <class System, class Transition>
concept TransitionLabelling = detail::TransitionLabelValue<System, Transition>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_FACET_TRANSITION_LABELLING_HPP
