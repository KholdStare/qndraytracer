#ifndef _QND_TEMPLATE_UTILS_H_
#define _QND_TEMPLATE_UTILS_H_

/* This file contains various template classes to aide with 
 * template metaprogramming */

namespace qnd {

    /**
     * Generates empty struct to represent a type.
     *
     * Can be used for partial function template specialization,
     * by using this dummy struct to select specialization through
     * overloads. In the final compiled code, struct is optimized away.
     */
    template <typename T>
    struct Type2Type {
        typedef T original_type;
    };


    /**
     * Suppose you have a TypeList, and you want to instantiate a
     * variadic template class VarTemplate with a modified version
     * of the TypeList, where each type wrapped by TypeWrapper
     * e.g:
     * Given <int, double>,
     * you want <std::unique_ptr<int>, std::unique_ptr<double> >
     *
     * Answer:
     * Variadic variadic template templates :P
     */
    template <template <typename...> class VarTemplate,
              template <typename> class TypeWrapper,
              typename... TypeList >
    struct VariadicTemplateTypeWrapper {
        typedef VarTemplate< TypeWrapper<TypeList> ... > Type;
    };


    /**
     * Internal variadic template classes for handling type tuples
     */
    namespace internal {

        template <typename... Tail > class TypeTupleInternal;

        template <class T, typename... Tail >
        class TypeTupleInternal<T, Tail...> : public TypeTupleInternal<Tail... > {

        protected:
            using TypeTupleInternal<Tail... >::get;

            T&       get(Type2Type<T>)       { return val; }
            T const& get(Type2Type<T>) const { return val; }

        private:
            T val;

        };

        // Have to define base case
        template <class T>
        class TypeTupleInternal<T> {

        protected:
            T&       get(Type2Type<T>)       { return val; }
            T const& get(Type2Type<T>) const { return val; }

        private:
            T val;

        };


    }

    /**
     * A class containing values of types specified in the TypeList.
     * The TypeList should contain a list of unique types.
     * To access these values, there are templated get methods, on the types.
     * This allows getting values by their type, rather than their name in
     * a structure. Sort of like a discriminated union, accept all types
     * are present.
     *
     * e.g.
     * TypeTuple<int, double> container;
     * container.get<int>() = 5;
     */
    template <typename... TypeList>
    class TypeTuple : internal::TypeTupleInternal<TypeList... > {

    public:
        using internal::TypeTupleInternal<TypeList... >::get;

        template <class T>
        T&       get()       { return this->get(Type2Type<T>()); }

        template <class T>
        T const& get() const { return this->get(Type2Type<T>()); }

    };

    /**
     * A class containing values of wrapped types specified in the TypeList.
     * This is similar to TypeTuple, except each type is now passed
     * as a template argument to TypeWrapper.
     *
     * This is useful when a tuple of types is needed, each of which
     * is wrapped by a common type. As a convenience, the getter methods
     * are templated on the inner type, as opposed to the full wrapped type
     *
     * e.g.
     * TypeWrapperTuple<std::unique_ptr, Widget, Gadget> container;
     * std::unique_ptr<Widget>& = container.get<Widget>();
     */
    template <template <typename> class TypeWrapper,
              typename... TypeList>
    class TypeWrapperTuple : VariadicTemplateTypeWrapper<internal::TypeTupleInternal,
                                                                      TypeWrapper, TypeList... >::Type {

    public:
        // use all the definitions of the method get from the internal class
        using typename VariadicTemplateTypeWrapper<internal::TypeTupleInternal, TypeWrapper, TypeList... >::Type::get;

        template <class T>
        TypeWrapper<T>&       get()       { return this->get(Type2Type<TypeWrapper<T>>()); }

        template <class T>
        TypeWrapper<T> const& get() const { return this->get(Type2Type<TypeWrapper<T>>()); }

    };


}


#endif // _QND_TEMPLATE_UTILS_H_
