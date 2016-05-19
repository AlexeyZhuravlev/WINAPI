// Автор: Алексей Журавлев
// Описание: Начальная точка входа в программу. Запуск тестов

#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}