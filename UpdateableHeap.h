//
// Created by juan on 27/01/21.
//

#ifndef DATASTRUCTURES_UPDATEABLEHEAP_H
#define DATASTRUCTURES_UPDATEABLEHEAP_H

#include "map"
#include "vector"
#include "iostream"

using namespace std;


template<typename TPriority, typename TKey>
class PQElement{
public:
    TKey id;
    TPriority priority;
    PQElement(TKey id, TPriority priority){
        this->priority = priority;
        this->id = id;
    }
};


template<typename TPriority, typename TKey>
class UpdateableHeap{
private:
    vector<PQElement<TPriority, TKey>> data;
    map<TKey, int>ids;
    void TopDownHeapify(int k);
    void BottomUpHeapify(int k);
    void swap(int i, int j);
    int left(int i);
    int right(int i);
    int parent(int i);
public:
    UpdateableHeap();
    void insertOrUpdate(TKey id, TPriority priority);
    void erase(TKey id);
    void showStatus();
    int getSize();
    void pop();
    bool isInserted(const TKey &id);
    pair<TKey, TPriority> top();
    pair<TKey, TPriority> stableTop();  // gets element by max priority. If a bunch of them have the same, then by greatest key
    void stableRemove();                // removes an element by priority, if a bunch of them have the same priority then by greatest key
};


/* ************************************************************** CODE IMPLEMENTATION ******************************************************************** */

template<typename TPriority, typename TKey>
UpdateableHeap<TPriority, TKey>::UpdateableHeap() {}

//left child
template<typename TPriority, typename TKey>
int UpdateableHeap<TPriority, TKey>::left(int i) {
    return 2*i + 1;
}

// right child
template<typename TPriority, typename TKey>
int UpdateableHeap<TPriority, TKey>::right(int i) {
    return 2*(i + 1);
}

// parent
template<typename TPriority, typename TKey>
int UpdateableHeap<TPriority, TKey>::parent(int i) {
    return (i-1)/2;
}

// size
template<typename TPriority, typename TKey>
int UpdateableHeap<TPriority, TKey>::getSize() {
    return data.size();
}

// does element exist
template<typename TPriority, typename TKey>
bool UpdateableHeap<TPriority, TKey>::isInserted(const TKey &id) {
    return ids.find(id) != ids.end();
}


//swap 2 elements in the data vector given the index of both
template<typename TPriority, typename TKey>
void UpdateableHeap<TPriority, TKey>::swap(int i, int j) {
    PQElement<TPriority, TKey> tmp = data[i];
    data[i] = data[j];
    data[j] = tmp;

    //update positions
    ids.find(data[i].id)->second = i;
    ids.find(data[j].id)->second = j;
}

// verify max-heap property
template<typename TPriority, typename TKey>
void UpdateableHeap<TPriority, TKey>::BottomUpHeapify(int k) {
    while(true){
        int p = parent(k);
        if ( p!=k && data[p].priority < data[k].priority){          // priority is greater than parent's priority (switch elements)
            swap(k, p);
            k = p;
        }else if(p!=k && data[p].priority == data[k].priority && data[p].id < data[k].id){
            swap(k, p);
            k = p;
        }
        else
            break;
    }
}

//verify max-heap property
template<typename TPriority, typename TKey>
void UpdateableHeap<TPriority, TKey>::TopDownHeapify(int k) {

    while (true){
        int l = left(k);
        int r = right(k);

        int max_index = k;
        if (r < data.size() && data[r].priority > data[max_index].priority)
            max_index = r;

        if (r < data.size() && data[r].priority == data[max_index].priority && data[r].id > data[max_index].id)
            max_index = r;

        if (l < data.size() && data[l].priority > data[max_index].priority)
            max_index = l;

        if (l < data.size() && data[l].priority == data[max_index].priority && data[l].id > data[max_index].id)
            max_index = l;

        if (max_index != k){
            swap(k, max_index);
            k = max_index;
        }else
            break;
    }
}

// insertOrUpdate element with its priority
template<typename TPriority, typename TKey>
void UpdateableHeap<TPriority, TKey>::insertOrUpdate(TKey id, TPriority priority) {

    // update info and return
    if (isInserted(id)){
        int pos = ids.find(id)->second;
        data[pos].priority = priority;

        if (pos != 0 && data[pos].priority > data[parent(pos)].priority)
            BottomUpHeapify(pos);
        else
            TopDownHeapify(pos);

        return;
    }

    // CREATE AND INSERT NEW ELEMENT
    PQElement<TPriority, TKey> element(id, priority);
    data.push_back(element);

    // insertOrUpdate id to ids table with initial position
    ids.insert(pair<TKey, int>(id, data.size() - 1));

    // verify max-heap property
    BottomUpHeapify(data.size()-1);
}

// function assumes that id exists
template<typename TPriority, typename TKey>
void UpdateableHeap<TPriority, TKey>::erase(TKey id) {

    // if element does not exist
    if (!isInserted(id))
        return;

    // get position of element to remove
    int pos = ids.find(id)->second;

    // get last element
    PQElement<TPriority, TKey> last = *(data.rbegin());

    // insertOrUpdate last element in position to delete that element and update position
    data[pos] = last;
    ids.find(last.id)->second = pos;

    // remove deleted element information
    ids.erase(id);

    // remove duplicated last element
    data.pop_back();

    // verify heap property
    if (pos!= 0 && data[pos].priority > data[parent(pos)].priority )
        BottomUpHeapify(pos);
    else
        TopDownHeapify(pos);
}

template<typename TPriority, typename TKey>
void UpdateableHeap<TPriority, TKey>::stableRemove() {
    if (getSize() == 0)
        return;

    TPriority greatest_priority = data[0].priority;
    int position = 0, i = 1;

    while(i < getSize()){
        if (data[i].priority == greatest_priority && data[position].id < data[i].id)
            position = i;
        i++;
    }

    erase(data[position].id);
}

template<typename TPriority, typename TKey>
void UpdateableHeap<TPriority, TKey>::pop(){

    // no elements
    if (data.empty())
        return;

    // remove element from ids table
    ids.erase(data[0].id);

    // more than 1 element
    if (data.size()> 1) {
        // move last element to head
        data[0] = *(data.rbegin());

        // update position of new head
        ids.find(data[0].id)->second = 0;

        // verify heap property
        TopDownHeapify(0);
    }

    // remove last element
    data.pop_back();
}

template <typename  TPriority, typename TKey>
pair<TKey, TPriority> UpdateableHeap<TPriority, TKey>::top() {
    if (data.empty())               // no elements
        return pair<TKey, TPriority>(NULL, NULL);

    // element to be returned
    pair<TKey, TPriority> head(data[0].id, data[0].priority);
    return head;
}

template <typename  TPriority, typename TKey>
pair<TKey, TPriority> UpdateableHeap<TPriority, TKey>::stableTop() {
    if (data.empty())               // no elements
        return pair<TKey, TPriority>(NULL, NULL);

    TPriority greatest_priority = data[0].priority;
    int position = 0, i = 1;

    while(i < getSize()){
        if (data[i].priority == greatest_priority && data[position].id < data[i].id)
            position = i;
        i++;
    }

    return pair<TKey, TPriority>(data[position].id, data[position].priority);
}

// print array content
template<typename TPriority, typename TKey>
void UpdateableHeap<TPriority, TKey>::showStatus() {
    bool ok_flag = true;
    for (int i = 0; i< data.size(); i++){

        if (left(i) < data.size())
            ok_flag = ok_flag && data[i].priority >= data[left(i)].priority;

        if (right(i) < data.size())
            ok_flag = ok_flag && data[i].priority >= data[right(i)].priority;

        cout << "id :" << data[i].id << " posicion: " << ids.find(data[i].id)->second << " prioridad: " << data[i].priority <<  endl;
    }

    cout << "Is everything okay? : " << ok_flag << endl;
}

#endif //DATASTRUCTURES_UPDATEABLEHEAP_H
