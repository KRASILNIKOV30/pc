add_executable(bank
        bank.cpp
        bank.h
)

add_executable(bank_tests
        tests/bank_tests.cpp
)

target_link_libraries(bank_tests PUBLIC catch2)