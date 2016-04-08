#ifndef PAGMO_EXCEPTIONS_HPP
#define PAGMO_EXCEPTIONS_HPP

/** \file exceptions.hpp
 * \brief Exceptions.
 *
 * This header contains custom exceptions used within pagmo and related utilities. It
 * is taken "as is" from the project piranha by Francesco Biscani
 */

#include <boost/lexical_cast.hpp>
#include <exception>
#include <string>

namespace pagmo
{
namespace detail
{

template <typename Exception>
struct ex_thrower
{
    // Determine the type of the __LINE__ macro.
    typedef std::decay<decltype(__LINE__)>::type line_type;
    explicit ex_thrower(const char *file, line_type line, const char *func):m_file(file),m_line(line),m_func(func)
    {}
    template <typename ... Args, typename = typename std::enable_if<std::is_constructible<Exception,Args...>::value>::type>
    void operator()(Args && ... args) const
    {
        Exception e(std::forward<Args>(args)...);
        throw e;
    }
    template <typename Str, typename ... Args, typename = typename std::enable_if<
        std::is_constructible<Exception,std::string,Args...>::value && (
        std::is_same<typename std::decay<Str>::type,std::string>::value ||
        std::is_same<typename std::decay<Str>::type,char *>::value ||
        std::is_same<typename std::decay<Str>::type,const char *>::value)>::type>
    void operator()(Str &&desc, Args && ... args) const
    {
        std::string msg("\nfunction: ");
        msg += m_func;
        msg += "\nwhere: ";
        msg += m_file;
        msg += ", ";
        msg += boost::lexical_cast<std::string>(m_line);
        msg += "\nwhat: ";
        msg += desc;
        msg += "\n";
        throw Exception(msg,std::forward<Args>(args)...);
    }
    const char  *m_file;
    const line_type m_line;
    const char  *m_func;
};

}}

/// Exception-throwing macro. 
/**
 * By default, this variadic macro will throw an exception of type \p exception_type, using the variadic
 * arguments for the construction of the exception object. The macro will check if the exception can be constructed
 * from the variadic arguments, and will produce a compilation error in case no suitable constructor is found.
 *
 * Additionally, given a set of variadic arguments <tt>[arg0,arg1,...]</tt>, and
 *
 * - if the first variadic argument \p arg0 is a string type (either C or C++),
 * - and if the exception can be constructed from the set of arguments <tt>[str,arg1,...]</tt>,
 *   where \p str is an instance of \p std::string,
 *
 * then the first argument \p arg0 is interpreted as the error message associated to the exception object, and it will be decorated
 * with information about the context in which the exception was thrown (file, line, function) before being
 * passed on for construction.
 *
 * Note that, in order to be fully standard-compliant, for use with exceptions that take no arguments on construction
 * the invocation must include a closing comma. E.g.,
 @code
 pagmo_throw(std::bad_alloc);
 @endcode
 * is not correct, whereas
 @code
 pagmo_throw(std::bad_alloc,);
 @endcode
 * is correct.
 */
#define pagmo_throw(exception_type,...) pagmo::detail::ex_thrower<exception_type>(__FILE__,__LINE__,__func__)(__VA_ARGS__);throw

#endif