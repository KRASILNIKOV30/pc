add_executable(bank
        Bank.h
        CharactersBase.h
        Characters.h
        Simulation.h
        main.cpp
)

add_executable(bank_tests
        Bank.h
        tests/Bank_tests.cpp
)

set_target_properties(bank PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(bank_tests PUBLIC catch2)