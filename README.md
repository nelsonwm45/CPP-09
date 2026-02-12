# CPP Module 09

## 📝 Overview

This module dives into the **Standard Template Library (STL)**, focusing on **Container** usage and algorithms. It explores how to choose the right container for a specific problem and how to implement complex algorithms efficiently.

## 📂 Project Structure

### ex00: Bitcoin Exchange
*   **Goal**: Create a program that outputs the value of a certain amount of bitcoin on a certain date.
*   **Implementation**:
    *   **Container**: `std::map<std::string, double>` to store the database (date -> exchange rate). Map is chosen for its efficient key-based lookups (`O(log n)`).
    *   **Input Parsing**: Reads a `data.csv` database file and a user-provided input file.
    *   **Date Handling**: Validates dates and format (YYYY-MM-DD).
    *   **Logic**: For each input date, finds the closest lower or equal date in the database (`lower_bound` logic) to calculate the value.
*   **Executable**: `btc`

### ex01: RPN
*   **Goal**: Evaluate a mathematical expression in **Reverse Polish Notation** (Postfix).
*   **Implementation**:
    *   **Container**: `std::stack` (using `std::list` as the underlying container). Stack is ideal for the LIFO nature of RPN evaluation.
    *   **Logic**:
        *   Iterate through tokens.
        *   If number: Push to stack.
        *   If operator: Pop top two operands, perform operation, push result back.
    *   **Operators**: Supports `+`, `-`, `*`, `/`.
*   **Executable**: `RPN`

### ex02: PmergeMe
*   **Goal**: Sort a sequence of positive integers using the **Ford-Johnson algorithm** (Merge-Insert Sort).
*   **Implementation**:
    *   **Containers**: Uses both `std::vector` and `std::deque` to compare performance.
    *   **Algorithm**:
        1.  Pair elements and sort pairs.
        2.  Recursively sort the larger elements of the pairs.
        3.  Insert the smaller elements into the sorted sequence using **Jacobsthal numbers** to optimize binary search insertion points.
    *   **Performance**: Measures and displays the time taken for sorting with each container type.
*   **Executable**: `PmergeMe`

## 🛠️ Compilation & Usage

Each exercise has its own `Makefile`.

### ex00
```bash
cd ex00
make
./btc input.txt
```

### ex01
```bash
cd ex01
make
./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
```
Expected output: `42`

### ex02
```bash
cd ex02
make
./PmergeMe 3 5 9 7 4
```
Expected output:
```
Before: 3 5 9 7 4
After:  3 4 5 7 9
Time to process a range of 5 elements with std::vector : ... us
Time to process a range of 5 elements with std::deque : ... us
```

## 🧹 Housekeeping
To clean up object files and executables in any directory:
```bash
make clean
make fclean
```
