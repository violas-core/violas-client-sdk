//
//
//
address Std {
    
module Map {
    use Std::Compare;
    use Std::BCS;
    //use 0x1::Option::{Self, Option};
    use Std::Vector;

    // Element with key and value
    struct Node<Key, Value> has copy, drop, store { 
        key_ : Key,
        value_ : Value,
    }
    
    struct Map<Key, Value> has copy, drop, store { 
        // A vector holds some nodes
        nodes: vector<Node<Key, Value>> 
    }

    public fun empty<Key, Value>(): Map<Key, Value> {
        Map { nodes: Vector::empty<Node<Key, Value>>() }
    }

    public fun size<Key, Value>(map: &Map<Key, Value>): u64 {
       Vector::length(&map.nodes)
    }

    public fun borrow<Key, Value>(map: &Map<Key, Value>, index: u64): (&Key, &Value) {
        // Borrow an element by index
        let e = Vector::borrow(&map.nodes, index);
        
        (&e.key_, &e.value_)
    }

    public fun borrow_mut<Key, Value>(map: &mut Map<Key, Value>, index: u64): (&Key, &mut Value) {
        // Borrow an element by index
        let e = Vector::borrow_mut(&mut map.nodes, index);
        
        (&e.key_, &mut e.value_)
    }

    public fun find<Key, Value>(map: &Map<Key, Value>, key_: &Key): (u64, bool) {
        // BCS seriliazes for Key
        let key_bcs = BCS::to_bytes(key_);
        let nodes = &map.nodes;

        // use binary search to locate `node` (if it exists)
        let left = 0;
        let len =  Vector::length(nodes);
        if (len == 0) {
            return (0, false)
        };
        let right = len - 1;

        while (left <= right) {
            let mid = (left + right) / 2;
            let cmp = Compare::cmp_bcs_bytes(&BCS::to_bytes(&Vector::borrow(nodes, mid).key_), &key_bcs);
            
            if (cmp == 0u8) {
                return (mid, true)
            } else if (cmp == 1u8) {
                left = mid + 1
            } else { // cmp == 2u8
                if (mid == 0) {
                    return (0, false)
                };
                assert(mid != 0, 88);
                right = mid -1
            }
        };

        (left, false)
    }

    // return true if `node` is already present in `t`, abort otherwise
    public fun insert<Key: drop, Value : drop>(map: &mut Map<Key, Value>, key_: Key, value_: Value) : bool {
        let (insert_at, found) = find(map, &key_);
        if (found) {
            false
        } else {
            let nodes = &mut map.nodes;
            // TODO: Vector::insert(index, e) would be useful here.
            let i = Vector::length(nodes);
            // add e to the end and then move it  to the left until we hit `insert_at`
            Vector::push_back(nodes, Node<Key, Value> { key_, value_, });
            
            while (i > insert_at) {
                Vector::swap(nodes, i, i - 1);
                i = i - 1;
            };

            true
        }                
    }

    public fun update<Key, Value: drop+copy>(map: &mut Map<Key, Value>, key_: &Key, value: Value): bool {
        let (index, found) = find(map, key_);
        if(found) {
            let (_, _value) = borrow_mut(map, index);            
            *_value = value; 
        };

        found
    }

    public fun erase<Key: drop, Value : drop>(map: &mut Map<Key, Value>, key_: &Key) : bool {
        let (i, found) = find(map, key_);
        if (found) {
            false
        } else {
            let nodes = &mut map.nodes;                        
            let length = Vector::length(nodes);

            // move node to the end and keep order for the vecotr
            while (i < length-2) {
                Vector::swap(nodes, i, i + 1);
                i = i + 1;
            };
            
            Vector::pop_back(nodes);

            true
        }  
    }

    // public fun get<Key, Value: copy>(map: &Map<Key, Value>, key: &Key) : Option<&Value> {
    //     let (index, found) = find(map, key);
    //     if(found) {
    //         let (_, value) = borrow(map, index);
    //         Option::some<&Value>(value)
    //     } else {
    //         Option::none<&Value>()
    //     }
    // }

    
}
}
