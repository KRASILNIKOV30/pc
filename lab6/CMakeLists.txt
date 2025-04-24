
add_executable(ticket-office_tests
        ticketOffice/TicketOffice.h
        tests/TicketOffice_tests.cpp
)

target_link_libraries(ticket-office_tests PUBLIC catch2)

#find_package(TBB REQUIRED)
#target_link_libraries(ticket-office PRIVATE TBB::tbb)