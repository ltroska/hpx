//  Copyright (c) 2014 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/include/parallel_minmax.hpp>
#include <hpx/util/lightweight_test.hpp>

#include "test_utils.hpp"

///////////////////////////////////////////////////////////////////////////////
template <typename ExPolicy, typename IteratorTag>
void test_minmax_element(ExPolicy const& policy, IteratorTag)
{
    BOOST_STATIC_ASSERT(hpx::parallel::is_execution_policy<ExPolicy>::value);

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c = test::random_iota(10007);

    iterator end(boost::end(c));
    base_iterator ref_end(boost::end(c));

    std::pair<iterator, iterator> r = hpx::parallel::minmax_element(policy,
        iterator(boost::begin(c)), iterator(end),
        std::less<std::size_t>());
    HPX_TEST(r.first != end && r.second != end);

    std::pair<base_iterator, base_iterator> ref = std::minmax_element(
        boost::begin(c), boost::end(c), std::less<std::size_t>());
    HPX_TEST(ref.first != ref_end && ref.second != ref_end);

    HPX_TEST_EQ(*ref.first, *r.first);
    HPX_TEST_EQ(*ref.second, *r.second);

    r = hpx::parallel::minmax_element(policy,
        iterator(boost::begin(c)), iterator(boost::end(c)));
    HPX_TEST(r.first != end && r.second != end);

    ref = std::minmax_element(boost::begin(c), boost::end(c));
    HPX_TEST(ref.first != ref_end && ref.second != ref_end);

    HPX_TEST_EQ(*ref.first, *r.first);
    HPX_TEST_EQ(*ref.second, *r.second);
}

template <typename IteratorTag>
void test_minmax_element(hpx::parallel::task_execution_policy, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::test_iterator<base_iterator, IteratorTag> iterator;

    std::vector<std::size_t> c = test::random_iota(10007);

    iterator end(boost::end(c));
    base_iterator ref_end(boost::end(c));

    hpx::future<std::pair<iterator, iterator> > r =
        hpx::parallel::minmax_element(hpx::parallel::task,
            iterator(boost::begin(c)), iterator(end),
            std::less<std::size_t>());
    std::pair<iterator, iterator> rit = r.get();
    HPX_TEST(rit.first != end && rit.second != end);

    std::pair<base_iterator, base_iterator> ref = std::minmax_element(
        boost::begin(c), boost::end(c), std::less<std::size_t>());
    HPX_TEST(ref.first != ref_end && ref.second != ref_end);

    HPX_TEST_EQ(*ref.first, *rit.first);
    HPX_TEST_EQ(*ref.second, *rit.second);

    r = hpx::parallel::minmax_element(hpx::parallel::task,
        iterator(boost::begin(c)), iterator(boost::end(c)));
    rit = r.get();
    HPX_TEST(rit.first != end && rit.second != end);

    ref = std::minmax_element(boost::begin(c), boost::end(c));
    HPX_TEST(ref.first != ref_end && ref.second != ref_end);

    HPX_TEST_EQ(*ref.first, *rit.first);
    HPX_TEST_EQ(*ref.second, *rit.second);
}

template <typename IteratorTag>
void test_minmax_element()
{
    using namespace hpx::parallel;

    test_minmax_element(seq, IteratorTag());
    test_minmax_element(par, IteratorTag());
    test_minmax_element(par_vec, IteratorTag());
    test_minmax_element(task, IteratorTag());

    test_minmax_element(execution_policy(seq), IteratorTag());
    test_minmax_element(execution_policy(par), IteratorTag());
    test_minmax_element(execution_policy(par_vec), IteratorTag());
    test_minmax_element(execution_policy(task), IteratorTag());
}

void minmax_element_test()
{
    test_minmax_element<std::random_access_iterator_tag>();
    test_minmax_element<std::forward_iterator_tag>();
}

///////////////////////////////////////////////////////////////////////////////
template <typename ExPolicy, typename IteratorTag>
void test_minmax_element_exception(ExPolicy const& policy, IteratorTag)
{
    BOOST_STATIC_ASSERT(hpx::parallel::is_execution_policy<ExPolicy>::value);

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::decorated_iterator<base_iterator, IteratorTag>
        decorated_iterator;

    std::vector<std::size_t> c = test::random_iota(10007);

    {
        bool caught_exception = false;
        try {
            hpx::parallel::minmax_element(policy,
                decorated_iterator(
                    boost::begin(c),
                    [](){ throw std::runtime_error("test"); }),
                decorated_iterator(boost::end(c)),
                std::less<std::size_t>());

            HPX_TEST(false);
        }
        catch(hpx::exception_list const& e) {
            caught_exception = true;
            test::test_num_exceptions<ExPolicy, IteratorTag>::call(policy, e);
        }
        catch(...) {
            HPX_TEST(false);
        }
        HPX_TEST(caught_exception);
    }

    {
        bool caught_exception = false;
        try {
            hpx::parallel::minmax_element(policy,
                decorated_iterator(
                    boost::begin(c),
                    [](){ throw std::runtime_error("test"); }),
                decorated_iterator(boost::end(c)));

            HPX_TEST(false);
        }
        catch(hpx::exception_list const& e) {
            caught_exception = true;
            test::test_num_exceptions<ExPolicy, IteratorTag>::call(policy, e);
        }
        catch(...) {
            HPX_TEST(false);
        }
        HPX_TEST(caught_exception);
    }
}

template <typename IteratorTag>
void test_minmax_element_exception(hpx::parallel::task_execution_policy, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::decorated_iterator<base_iterator, IteratorTag>
        decorated_iterator;

    std::vector<std::size_t> c = test::random_iota(10007);

    {
        bool returned_from_algorithm = false;
        bool caught_exception = false;

        try {
            hpx::future<std::pair<decorated_iterator, decorated_iterator> > f =
                hpx::parallel::minmax_element(hpx::parallel::task,
                    decorated_iterator(
                        boost::begin(c),
                        [](){ throw std::runtime_error("test"); }),
                    decorated_iterator(boost::end(c)),
                    std::less<std::size_t>());

            returned_from_algorithm = true;

            f.get();

            HPX_TEST(false);
        }
        catch(hpx::exception_list const& e) {
            caught_exception = true;
            test::test_num_exceptions<
                hpx::parallel::task_execution_policy, IteratorTag
            >::call(hpx::parallel::task, e);
        }
        catch(...) {
            HPX_TEST(false);
        }

        HPX_TEST(caught_exception);
        HPX_TEST(returned_from_algorithm);
    }

    {
        bool caught_exception = false;
        bool returned_from_algorithm = false;

        try {
            hpx::future<std::pair<decorated_iterator, decorated_iterator> > f =
                hpx::parallel::minmax_element(hpx::parallel::task,
                    decorated_iterator(
                        boost::begin(c),
                        [](){ throw std::runtime_error("test"); }),
                    decorated_iterator(boost::end(c)));

            returned_from_algorithm = true;

            f.get();

            HPX_TEST(false);
        }
        catch(hpx::exception_list const& e) {
            caught_exception = true;
            test::test_num_exceptions<
                hpx::parallel::task_execution_policy, IteratorTag
            >::call(hpx::parallel::task, e);
        }
        catch(...) {
            HPX_TEST(false);
        }

        HPX_TEST(caught_exception);
        HPX_TEST(returned_from_algorithm);
    }
}

template <typename IteratorTag>
void test_minmax_element_exception()
{
    using namespace hpx::parallel;

    // If the execution policy object is of type vector_execution_policy,
    // std::terminate shall be called. therefore we do not test exceptions
    // with a vector execution policy
    test_minmax_element_exception(seq, IteratorTag());
    test_minmax_element_exception(par, IteratorTag());
    test_minmax_element_exception(task, IteratorTag());

    test_minmax_element_exception(execution_policy(seq), IteratorTag());
    test_minmax_element_exception(execution_policy(par), IteratorTag());
    test_minmax_element_exception(execution_policy(task), IteratorTag());
}

void minmax_element_exception_test()
{
    test_minmax_element_exception<std::random_access_iterator_tag>();
    test_minmax_element_exception<std::forward_iterator_tag>();
}

///////////////////////////////////////////////////////////////////////////////
template <typename ExPolicy, typename IteratorTag>
void test_minmax_element_bad_alloc(ExPolicy const& policy, IteratorTag)
{
    BOOST_STATIC_ASSERT(hpx::parallel::is_execution_policy<ExPolicy>::value);

    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::decorated_iterator<base_iterator, IteratorTag>
        decorated_iterator;

    std::vector<std::size_t> c = test::random_iota(10007);

    {
        bool caught_exception = false;
        try {
            hpx::parallel::minmax_element(policy,
                decorated_iterator(
                    boost::begin(c),
                    [](){ throw std::bad_alloc(); }),
                decorated_iterator(boost::end(c)),
                std::less<std::size_t>());

            HPX_TEST(false);
        }
        catch(std::bad_alloc const&) {
            caught_exception = true;
        }
        catch(...) {
            HPX_TEST(false);
        }
        HPX_TEST(caught_exception);
    }

    {
        bool caught_exception = false;
        try {
            hpx::parallel::minmax_element(policy,
                decorated_iterator(
                    boost::begin(c),
                    [](){ throw std::bad_alloc(); }),
                decorated_iterator(boost::end(c)));

            HPX_TEST(false);
        }
        catch(std::bad_alloc const&) {
            caught_exception = true;
        }
        catch(...) {
            HPX_TEST(false);
        }
        HPX_TEST(caught_exception);
    }
}

template <typename IteratorTag>
void test_minmax_element_bad_alloc(hpx::parallel::task_execution_policy, IteratorTag)
{
    typedef std::vector<std::size_t>::iterator base_iterator;
    typedef test::decorated_iterator<base_iterator, IteratorTag>
        decorated_iterator;

    std::vector<std::size_t> c = test::random_iota(10007);

    {
        bool returned_from_algorithm = false;
        bool caught_exception = false;

        try {
            hpx::future<std::pair<decorated_iterator, decorated_iterator> > f =
                hpx::parallel::minmax_element(hpx::parallel::task,
                    decorated_iterator(
                        boost::begin(c),
                        [](){ throw std::bad_alloc(); }),
                    decorated_iterator(boost::end(c)),
                    std::less<std::size_t>());

            returned_from_algorithm = true;

            f.get();

            HPX_TEST(false);
        }
        catch(std::bad_alloc const&) {
            caught_exception = true;
        }
        catch(...) {
            HPX_TEST(false);
        }

        HPX_TEST(caught_exception);
        HPX_TEST(returned_from_algorithm);
    }

    {
        bool caught_exception = false;
        bool returned_from_algorithm = false;

        try {
            hpx::future<std::pair<decorated_iterator, decorated_iterator> > f =
                hpx::parallel::minmax_element(hpx::parallel::task,
                    decorated_iterator(
                        boost::begin(c),
                        [](){ throw std::bad_alloc(); }),
                    decorated_iterator(boost::end(c)));

            returned_from_algorithm = true;

            f.get();

            HPX_TEST(false);
        }
        catch(std::bad_alloc const&) {
            caught_exception = true;
        }
        catch(...) {
            HPX_TEST(false);
        }

        HPX_TEST(caught_exception);
        HPX_TEST(returned_from_algorithm);
    }
}

template <typename IteratorTag>
void test_minmax_element_bad_alloc()
{
    using namespace hpx::parallel;

    // If the execution policy object is of type vector_execution_policy,
    // std::terminate shall be called. therefore we do not test exceptions
    // with a vector execution policy
    test_minmax_element_bad_alloc(seq, IteratorTag());
    test_minmax_element_bad_alloc(par, IteratorTag());
    test_minmax_element_bad_alloc(task, IteratorTag());

    test_minmax_element_bad_alloc(execution_policy(seq), IteratorTag());
    test_minmax_element_bad_alloc(execution_policy(par), IteratorTag());
    test_minmax_element_bad_alloc(execution_policy(task), IteratorTag());
}

void minmax_element_bad_alloc_test()
{
    test_minmax_element_bad_alloc<std::random_access_iterator_tag>();
    test_minmax_element_bad_alloc<std::forward_iterator_tag>();
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main()
{
    minmax_element_test();
    minmax_element_exception_test();
    minmax_element_bad_alloc_test();
    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    // By default this test should run on all available cores
    std::vector<std::string> cfg;
    cfg.push_back("hpx.os_threads=" +
        boost::lexical_cast<std::string>(hpx::threads::hardware_concurrency()));

    // Initialize and run HPX
    HPX_TEST_EQ_MSG(hpx::init(argc, argv, cfg), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}

