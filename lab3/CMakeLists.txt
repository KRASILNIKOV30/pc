add_executable(bank
        bank/Bank.h
        bank/CharactersBase.h
        bank/Characters.h
        bank/Simulation.h
        bank/main.cpp
)

add_executable(bank_tests
        bank/Bank.h
        bank/tests/Bank_tests.cpp
)

add_executable(warehouse
        warehouse/Warehouse.h
        warehouse/main.cpp
        warehouse/Supplier.h
        warehouse/Client.h
        warehouse/Auditor.h
)

set_target_properties(bank PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(warehouse PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(bank_tests PUBLIC catch2)