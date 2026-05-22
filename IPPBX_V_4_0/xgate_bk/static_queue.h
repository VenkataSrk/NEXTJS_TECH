/* $Header: /xGate/static_queue.h 3     10/23/02 5:01p Bennylp $
 *
 * Queue with minimal (almost none) dynamic memory operations.
 * When capacity is correctly set, there will be NO dynamic memory allocations/
 * deallocations during the queue operations.
 *
 * $Log: /xGate/static_queue.h $
 * 
 * 3     10/23/02 5:01p Bennylp
 * Patch to make it thread safe
 * 
 * 2     10/05/01 1:38p Bennylp
 * Initial version
 * 
 * 1     10/05/01 12:18p Bennylp
 * Added to source control
 */
#include <assert.h>
#include <iostream>
using namespace std;

template <class T>
class static_queue {
public:
    typedef T value_type;
    typedef unsigned int size_type;

    enum {
        //Linux capacity needs increase for Host media processing
	//Narasimham increased
		default_capacity = 1500 /*32*/
    };

    static_queue(size_t init_capacity = default_capacity) : full_(false) {
		head_ = tail_ = buffer_ = new value_type[init_capacity];
		end_ptr_ = buffer_ + init_capacity ;
	//	end_ptr_ = buffer_ + init_capacity  -1;
    }

    ~static_queue() {
		delete [] buffer_;
    }

    bool empty() const {
		return full_ ? false : (head_ == tail_) ;
    }

    size_type size() const {
		return full_ ? capacity() : ((tail_ + capacity()) - head_) % capacity();
    }

    size_type capacity() const {
		return (end_ptr_ - buffer_);
    }

    value_type & top() {
		assert(size() != 0);
		return *head_;
    }

    const value_type & top() const {
		assert(size() != 0);
		return *head_;
    }

    value_type & front() {
		return top();
    }

    const value_type & front() const {
		return top();
    }

    void push_back(const value_type & val) {
//		cout << "Inside push_back ******" << endl;
	
		assert(size() < capacity());
		*tail_++ = val;
		if (tail_ == end_ptr_){
//			cout << "*** tail_ is equal to end_ptr ***" << endl;
		    tail_ = buffer_;
		}

		if (tail_ == head_)
		    full_ = true;
//		cout << "head is "<< hex << head_ << " and tail is " << hex << tail_ << endl;
    }

    void push(const value_type & val) {
		push_back(val);
    }

    void pop_front() {
//		cout << "Inside pop_front , head is " << hex << head_ << " ******" << endl;
		assert(!empty());
		if (++head_ == end_ptr_){
//			cout << "*** head is equal to end_ptr ***" << endl;
		    head_ = buffer_;
		}
		full_  = false;
	}

	void pop() {
		pop_front();
	}

private:
	bool full_;
	value_type * head_, *tail_, *end_ptr_;
	value_type * buffer_;
};

