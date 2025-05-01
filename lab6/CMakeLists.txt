
add_executable(ticket-office_tests
        ticketOffice/TicketOffice.h
        tests/TicketOffice_tests.cpp
)

add_executable(atomic-max_tests
        atomicMax/AtomicMax.h
        tests/AtomicMax_tests.cpp
        threadPool/main.cpp
        tests/ThreadPoolLockFree_tests.cpp
)

add_executable(thread-pool
        main.cpp
        threadPool/ThreadPoolLockBased.h
        threadPool/ThreadPoolLockFree.h
)

add_executable(thread-pool_tests
        threadPool/ThreadPoolLockBased.h
        threadPool/ThreadPoolLockFree.h
        tests/ThreadPoolLockFree_tests.cpp
        tests/ThreadPoolLockBased_tests.cpp
)

set(BOOST_ROOT "/usr/include/boost")
find_package(Boost 1.53.0 REQUIRED COMPONENTS system thread)

target_link_libraries(ticket-office_tests PUBLIC catch2)
target_link_libraries(atomic-max_tests PUBLIC catch2)
target_link_libraries(thread-pool Boost::system Boost::thread)
target_link_libraries(thread-pool_tests Boost::system Boost::thread catch2)

#find_package(TBB REQUIRED)
#target_link_libraries(ticket-office PRIVATE TBB::tbb)