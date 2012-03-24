#ifdef _MSC_VER
	#pragma once
#endif
#ifndef __LOG_HPP_20120323200834__
#define __LOG_HPP_20120323200834__

/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-23
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <string>

//#include <boost/serialization/singleton.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

namespace cg
{
    namespace bofu = boost::fusion;

#define CG_DEFINE_LOG_TAG(type, name, s)\
    struct name { static std::string str() { return s; } };
#define CG_DEFINE_LOG_TAG_(r, data, elem) CG_DEFINE_LOG_TAG elem
#define CG_DEFINE_LOG_PAIR(type, name, s)\
    bofu::pair<tags::name, type>
#define CG_DEFINE_LOG_PAIR_(r, data, elem) BOOST_PP_COMMA() CG_DEFINE_LOG_PAIR elem
#define CG_DEFINE_LOG_ZERO(z, n, text) BOOST_PP_COMMA_IF(n) text
#define CG_DEFINE_LOG(seq)\
    namespace tags\
    {\
        BOOST_PP_SEQ_FOR_EACH(CG_DEFINE_LOG_TAG_, _, seq)\
    }\
    namespace log_detail\
    {\
        struct log\
        {\
            typedef bofu::map<\
                CG_DEFINE_LOG_PAIR BOOST_PP_SEQ_HEAD(seq)\
                BOOST_PP_SEQ_FOR_EACH(CG_DEFINE_LOG_PAIR_, _, BOOST_PP_SEQ_TAIL(seq))\
            > type;\
            static type& instance()\
            {\
                static type t(BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), CG_DEFINE_LOG_ZERO, 0));\
                return t;\
            }\
        };\
    }

    CG_DEFINE_LOG(
        ((int, render_count, "render count"))
        ((double, render_time, "render time"))
        ((double, transform_time, "transform time"))
        //((double, planf_time, "eval plane formular time"))
        ((double, make_table_time, "make edge table time"))
        ((double, make_aet_time, "make active edge table time"))
        ((double, draw_time, "draw time"))
        ((double, update_aet_time, "update active edge table time"))
        )

#undef CG_DEFINE_LOG
#undef CG_DEFINE_LOG_ZERO
#undef CG_DEFINE_LOG_PAIR_
#undef CG_DEFINE_LOG_PAIR
#undef CG_DEFINE_LOG_TAG_
#undef CG_DEFINE_LOG_TAG

    namespace
    {
        log_detail::log::type &log = log_detail::log::instance();
    }
}

#endif // __LOG_HPP_20120323200834__
