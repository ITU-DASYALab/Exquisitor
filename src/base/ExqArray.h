//
// Created by Omar Shahbaz Khan on 15/03/2020.
//

#ifndef EXQUISITOR_EXQARRAY_H
#define EXQUISITOR_EXQARRAY_H

namespace exq {

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

    private:
        int _size;
        T* _arr;
    };

}

#endif //EXQUISITOR_EXQARRAY_H
