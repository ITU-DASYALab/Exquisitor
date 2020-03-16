//
// Created by Omar Shahbaz Khan on 15/03/2020.
//

#ifndef EXQUISITOR_EXQARRAY_H
#define EXQUISITOR_EXQARRAY_H

namespace exq {

    /**
     * @class minimalistic dynamic array to hold compressed representations
     */
    template<typename T>
    class ExqArray {
    public:
        ExqArray(T* arr, int n) {
            this->size = n;
            this->arr = arr;
        };

        ExqArray(int n) {
            this->size = n;
            this->arr = new T[n];
        };

        ~ExqArray() {
            delete[] this->arr;
        };

        int getSize() {
            return this->size;
        };

        void setItem(T item, int i) {
            this->arr[i] = item;
        };

        T getItem(int i) {
            return this->arr[i];
        };

        T* getArray() {
            return arr;
        };

        friend class ExqArrDescriptor;
    private:
        int size;
        T* arr;
    };

}

#endif //EXQUISITOR_EXQARRAY_H
