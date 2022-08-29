#pragma once
#include <initializer_list>
#include <iostream>
#include <vector>
#include <cmath>
namespace lb {
    template<typename type>
    struct node{
        node<type>* _xnp;
        type* _array;
        int _size;  
    };
    template<typename type>
    struct nodeptr{
        node<type>* _prev;
        node<type>* _current;
        int _offset;
    };

    template<typename type>
    class dynamic_array{
        public:
        dynamic_array(){}

        dynamic_array(dynamic_array<type>& T){
            for(int i = 0; i < T.size(); i++){
                push_back(T[i]);
            }
            _space=T._space;
            _alloc_size=T._alloc_size;
        }
        dynamic_array(dynamic_array<type>&& T){
            _nodes=std::move(T._nodes);
            _size=T._size;
            _space=T._space;
            _alloc_size=T._alloc_size;
        }
        dynamic_array(const std::initializer_list<type>& T){
            for(auto i = T.begin(); i != T.end(); i++){
                push_back(*i);
            }
        }
        dynamic_array<type>& operator=(dynamic_array<type>& T){
            clear();
            for(int i = 0; i < T.size(); i++){
                push_back(T[i]);
            }
            _space=T._space;
            _alloc_size=T._alloc_size;
            return *this;
        }
        dynamic_array<type>& operator=(dynamic_array<type>&& T){
            clear();
            _nodes=std::move(T._nodes);
            _size=T._size;
            _space=T._space;
            _alloc_size=T._alloc_size;
            return *this;
        }
        dynamic_array<type>& operator=(std::initializer_list<type>& T){
            clear();
            for(auto i = T.begin(); i != T.end(); i++){
                push_back(*i);
            }
            return *this;
        }
        ~dynamic_array() {
            lb::node<int>* first = _nodes[0]._current;
            lb::node<int>* second = nullptr;
            while(first!=nullptr){
                lb::node<int>* buffer=first;
                first=XOR(second, first->_xnp);
                second=buffer;
                delete[] second->_array;
                delete second;
            }
        }

        type& operator[](int pos){
            int index1=pos/_space, offset=pos%_space+_nodes[index1]._offset;
            nodeptr<type> first = _nodes[index1]; node<type>* buffer;
            while(first._current->_size<=offset){
                offset-=first._current->_size;
                buffer=first._current;
                first._current=XOR(first._prev, first._current->_xnp);
                first._prev=buffer;
            }
            return *(first._current->_array+offset);
        }

        // prepare memory for read/write 
        void prep_flags(){
            // stop if only start and end are present
            if(_nodes.size()==2){
                _space=_size;
                return;
            }
            // calculate space between flags
            _space = (int)(_size/(_nodes.size()-1)); _space++;

            // position flags
            int index=1, offset=0;
            lb::node<int>* first=_nodes[0]._current;
            lb::node<int>* second=nullptr;
            while(first != nullptr && index<_nodes.size()-1){
                int r=first->_size-offset; // calculate if there is space for the flag
                // if there is
                if(r > _space){
                    offset+=_space; // update offset to point to the next location for flag

                    // save the flag and inc index for the next flag
                    _nodes[index]._current=first;
                    _nodes[index]._prev=second;
                    _nodes[index]._offset=offset;
                    index++;
                }
                // if there isn't
                else {
                    offset=-r; // add r as bonus space for next calculation
                    
                    // goto next node
                    lb::node<int>* buffer=first;
                    first = XOR(second, first->_xnp);
                    second=buffer;
                }
            }
        }

        // append value T at the end
        void push_back(const type &T){
            nodeptr<type>* pnode = &_nodes[_nodes.size()-1]; // pointer to last node

            // if last node exists
            // set pointer to last node
            // allocate if the last node is full
            // pnode->_offset is used as counter only for the last node
            if(pnode->_current!=nullptr){
                if(pnode->_offset >= pnode->_current->_size){
                    // allocate new node
                    node<type>* nnode = new node<type>{XOR(pnode->_current, nullptr), new type[_alloc_size], _alloc_size};

                    // attach new node
                    pnode->_current->_xnp=XOR(pnode->_prev, nnode);

                    // update last nodeptr
                    pnode->_prev=pnode->_current;
                    pnode->_current=nnode;
                    pnode->_offset=0;
                }
            }
            // if last node doesn't exist
            // allocate start node and end node which are equal for now
            else {
                // allocate new node
                node<type>* nnode = new node<type>{nullptr, new type[_alloc_size], _alloc_size};

                // add start node
                _nodes[0]=nodeptr<type>{nullptr, nnode, 0};

                // add end node
                _nodes[1]=nodeptr<type>{nullptr, nnode, 0};

                // update pnode
                pnode = &_nodes[_nodes.size()-1];
            }
            
            // set closest allocated but unused space to T
            *(pnode->_current->_array+pnode->_offset)=T;
            // update counter
            pnode->_offset++;
            // update size
            _size++;
        }
        void insert(int pos, const type& value){
            int index1=pos/_space, offset1=pos%_space+_nodes[index1]._offset;
            nodeptr<type> first = _nodes[index1]; node<type>* buffer;
            while(first._current->_size<=offset1){
                offset1-=first._current->_size;
                buffer=first._current;
                first._current=XOR(first._prev, first._current->_xnp);
                first._prev=buffer;
            }
            type* narray=new type[first._current->_size+1]; bool offset2=0;
            for(int i = 0; i < first._current->_size+1; i++){
                if(i==offset1){
                    *(narray+i)=value;
                    offset2=1;
                    continue;
                }
                *(narray+i)=*(first._current->_array+i-offset2);
            }
            delete[] first._current->_array;
            first._current->_array=narray;
            first._current->_size++;
            _size++;
        }

        void insert(int pos, dynamic_array<type>& values, int start, int end){
            // make a copy of the values
            dynamic_array<type> ndarray; ndarray.prep_flags();
            for(int i = start; i <= end; i++){
                ndarray.push_back(values[i]);
            }
            
            // find node
            int index1=pos/_space, offset1=pos%_space+_nodes[index1]._offset;
            nodeptr<type> first = _nodes[index1]; node<type>* buffer;
            while(first._current->_size<=offset1){
                offset1-=first._current->_size;
                buffer=first._current;
                first._current=XOR(first._prev, first._current->_xnp);
                first._prev=buffer;
            }

            // gen new arrays
            type* lnarray = new type[offset1];
            for(int i = 0; i < offset1; i++){
                *(lnarray+i)=*(first._current->_array+i);
            }

            int rnarray_size_1=first._current->_size-offset1;
            int rnarray_size_2=ndarray._nodes[ndarray._nodes.size()-1]._offset;
            type* rnarray = new type[rnarray_size_1+rnarray_size_2];
            for(int i = 0; i < rnarray_size_2; i++){
                *(rnarray+i)=*(ndarray._nodes[ndarray._nodes.size()-1]._current->_array+i);
            }
            for(int i = 0; i < rnarray_size_1; i++){
                *(rnarray+ndarray._nodes[ndarray._nodes.size()-1]._offset+i)=*(first._current->_array+offset1+i);
            }

    
            // delete and update larray
            delete[] first._current->_array;
            first._current->_size=offset1;
            first._current->_array=lnarray;

            // delete and update larray
            delete[] ndarray._nodes[ndarray._nodes.size()-1]._current->_array;
            ndarray._nodes[ndarray._nodes.size()-1]._current->_array=rnarray;
            ndarray._nodes[ndarray._nodes.size()-1]._current->_size=rnarray_size_1+rnarray_size_2;

            /*
            for(int i = 0; i < first._current->_size; i++){
                std::cout << first._current->_array[i] << '\n';
            }
            for(int i = 0; i < rnarray_size_1+rnarray_size_2; i++){
                std::cout << *(ndarray._nodes[ndarray._nodes.size()-1]._current->_array+i) << '\n';
            }
            */
            
            // link
            node<type>* second=XOR(first._prev, first._current->_xnp);
            if(second!=nullptr){
                second->_xnp=XOR(ndarray._nodes[ndarray._nodes.size()-1]._current, XOR(first._current, second->_xnp));
            }
            first._current->_xnp=XOR(first._prev, ndarray._nodes[0]._current);

            ndarray._nodes[0]._current->_xnp=XOR(first._current, XOR(nullptr, ndarray._nodes[0]._current->_xnp));
            ndarray._nodes[0]._prev=first._current;
            ndarray._nodes[ndarray._nodes.size()-1]._current->_xnp=XOR(XOR(ndarray._nodes[ndarray._nodes.size()-1]._current->_xnp, nullptr), second);

            ndarray._nodes[0]._current=nullptr;
            _size+=end-start+1;
        }

        void pop_back(){
            nodeptr<type>* pnode = &_nodes[_nodes.size()-1];
            if(pnode->_offset==0){
                delete[] pnode->_current->_array;
                delete pnode->_current;
                node<type>* buffer = pnode->_current;
                pnode->_current=pnode->_prev;
                pnode->_prev=XOR(pnode->_prev->_xnp, buffer);
                pnode->_current->_xnp=XOR(pnode->_prev, nullptr);
                pnode->_offset=pnode->_current->_size;
            }
            pnode->_offset--;
            _size--;
        }
        void erase(int start, int end){
            // get logical offsets
            int index1=start/_space, offset1=start%_space+_nodes[index1]._offset;
            int index2=end/_space, offset2=end%_space+_nodes[index2]._offset;

            // get actual offsets
            nodeptr<type> first=_nodes[index1], second=_nodes[index2];
            node<type>* buffer;
            while(first._current->_size<=offset1){
                offset1-=first._current->_size;
                buffer=first._current;
                first._current=XOR(first._prev, first._current->_xnp);
                first._prev=buffer;
            }            
            while(second._current->_size<=offset2){
                offset2-=second._current->_size;
                buffer=second._current;
                second._current=XOR(second._prev, second._current->_xnp);
                second._prev=buffer;
            }

            // allocate new array
            int narray_size = offset1+second._current->_size-offset2-1;

            type* narray = new type[narray_size];
            // copy the unaffected part of the first array
            int i = 0;
            for(; i < offset1; i++){
                *(narray+i)=*(first._current->_array+i);
            }
            // copy the unaffected part of the second array cursed loop edition
            for(int o = offset2+1; o < second._current->_size; o++, i++){
                *(narray+i)=*(second._current->_array+o);
            }

            // attach array and nodes
            delete[] first._current->_array;
            first._current->_array=narray;
            first._current->_size=narray_size;
            buffer = XOR(second._prev, second._current->_xnp);
            first._current->_xnp=XOR(first._prev, buffer);
            if(buffer!=nullptr){
                buffer->_xnp=XOR(first._current, XOR(second._current, buffer->_xnp));
            }
            else {
                _nodes[_nodes.size()-1]._current=first._current;
                _nodes[_nodes.size()-1]._prev=first._prev;
                _nodes[_nodes.size()-1]._offset+=offset1-offset2-1;
            }

            // delete nodes in between
            while(second._current!=first._current){
                delete[] second._current->_array;
                delete second._current;
                buffer=second._current;
                second._current=second._prev;
                second._prev=XOR(second._prev->_xnp, buffer);
            }

            // update size
            _size-=end-start+1;
        }

        void set_flags(int number){
            std::vector<nodeptr<type>> nnodes(number+2);
            nnodes[0] = _nodes[0]; nnodes[number+1] = _nodes[_nodes.size()-1];
            _nodes=std::move(nnodes);
        }

        void set_alloc_size(int size){
            _alloc_size=size;
        }

        void debug(){
            std::cout << "flags:\n";
            for(int i = 0 ; i < _nodes.size(); i++){
                std::cout << _nodes[i]._current << ' ' << _nodes[i]._offset << '\n';
            }
            std::cout << "\ndata:\n";
            lb::node<int>* first = _nodes[0]._current;
            lb::node<int>* second = nullptr;
            while(first!=nullptr){
                std::cout << first << ' ' << first->_size << ' ' << first->_xnp << '\n';
                for(int i = 0; i < first->_size; i++){
                    std::cout << *(first->_array+i) << '\n';
                }
                lb::node<int>* buffer=first;
                first=XOR(second, first->_xnp);
                second=buffer;
            }
            std::cout << '\n';
        }

        inline int size(){
            return _size;
        }

        void clear(){
            lb::node<int>* first = _nodes[0]._current;
            lb::node<int>* second = nullptr;
            while(first!=nullptr){
                lb::node<int>* buffer=first;
                first=XOR(second, first->_xnp);
                second=buffer;
                delete[] second->_array;
                delete second;
            }
            _nodes[0]._current=nullptr;
            _nodes[_nodes.size()-1]._current=nullptr;
            _size=0;
        }
        
        public:
        std::vector<nodeptr<type>> _nodes = {{nodeptr<type>{nullptr, nullptr, 0}}, {nodeptr<type>{nullptr, nullptr, 0}}};
        int _space=0, _size=0;
        int _alloc_size=10;
        
        inline node<type>* XOR(node<type>* a, node<type>* b){
            return (node<type>*)((uintptr_t)a^(uintptr_t)b);
        }
    };
}
