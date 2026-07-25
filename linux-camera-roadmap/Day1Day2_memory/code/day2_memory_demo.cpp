#include <iostream>

void modifyByValue(int value) {
    value = 100;
}

void modifyByPointer(int* value) {
    if (value != nullptr) {
        *value = 200;
    }
}

void modifyByReference(int& value) {
    value = 300;
}

int main() {
    int number = 10;

    modifyByValue(number);
    std::cout << "After value: " << number << '\n';

    modifyByPointer(&number);
    std::cout << "After pointer: " << number << '\n';

    modifyByReference(number);
    std::cout << "After reference: " << number << '\n';

    return 0;
}