//  Copyright (c) 2007-2013 Hartmut Kaiser
//  Copyright (c) 2011      Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_EXCEPTION_DETAIL_HPX_CATEGORY_HPP)
#define HPX_EXCEPTION_DETAIL_HPX_CATEGORY_HPP

#include <hpx/config.hpp>
#include <hpx/error.hpp>

#include <boost/system/error_code.hpp>

#include <string>

#if !defined(BOOST_SYSTEM_NOEXCEPT)
#define BOOST_SYSTEM_NOEXCEPT HPX_NOEXCEPT
#endif

namespace hpx { namespace detail {
    class hpx_category : public boost::system::error_category
    {
    public:
        const char* name() const BOOST_SYSTEM_NOEXCEPT
        {
            return "HPX";
        }

        std::string message(int value) const
        {
            if (value >= success && value < last_error)
                return std::string("HPX(") + error_names[value] + ")"; //-V108

            return "HPX(unknown_error)";
        }
    };

    struct lightweight_hpx_category : hpx_category {};

    // this doesn't add any text to the exception what() message
    class hpx_category_rethrow : public boost::system::error_category
    {
        public:
            const char* name() const BOOST_SYSTEM_NOEXCEPT
            {
                return "";
            }

            std::string message(int) const HPX_NOEXCEPT
            {
                return "";
            }
    };

    struct lightweight_hpx_category_rethrow : hpx_category_rethrow {};
}}

#endif
