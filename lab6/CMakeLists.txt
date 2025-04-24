
add_executable(ticket-office_tests
        ticketOffice/TicketOffice.h
        tests/TicketOffice_tests.cpp
)

add_executable(atomic-max_tests
        atomicMax/AtomicMax.h
        tests/AtomicMax_tests.cpp
)

target_link_libraries(ticket-office_tests PUBLIC catch2)
target_link_libraries(atomic-max_tests PUBLIC catch2)

#find_package(TBB REQUIRED)
#target_link_libraries(ticket-office PRIVATE TBB::tbb)