//
// Created by Omar Shahbaz Khan on 15/03/2020.
//

#ifndef EXQUISITOR_EXQARRAY_H
#define EXQUISITOR_EXQARRAY_H

#pragma once

#include <iostream>

namespace exq {

    using std::cout;
    using std::endl;
    /**
     * @class Minimalistic dynamic array. Less memory usage than vector.
     */
    template<typename T>
    class ExqArray {
    public:
        /// Constructor. Takes an existing array and converts it into a dynamic array.
        /// \param arr - Array
        /// \param n - Array size
        ExqArray(T* arr, int n) {
            _size = n;
            _arr = arr;
        };

        /// Constructor. Creates an array of given size n.
        /// \param n - Initial array size
        ExqArray(int n) {
            _size = n;
            _arr = new T[n];
        };

        /// Constructor. 0 size array.
        ExqArray() {
            _size = 0;
            _arr = nullptr;
        }

        /// copy constructor
        ExqArray(const ExqArray<T>& org) noexcept : _size(0) {
            _size = org._size;
            _arr = new T[_size];
            for (int i = 0; i < _size; i++)
                _arr[i] = org._arr[i];
        }

        /// copy assignment operator
        /// \param that
        /// \return
        ExqArray<T>& operator=(const ExqArray<T>& that) noexcept {
            _size = that._size;
            _arr = new T[_size];
            for (int i = 0; i < _size; i++)
                _arr[i] = that._arr[i];
            return *this;
        }

        ~ExqArray() {
            delete[] _arr;
        };

        int getSize() {
            return _size;
        };

        void setItem(T item, int i) {
            _arr[i] = item;
        };

        T getItem(int i) {
            return _arr[i];
        };

        T* getArray() {
            return _arr;
        };

    protected:
        int _size;
        T* _arr;
    };

}

#endif //EXQUISITOR_EXQARRAY_H
